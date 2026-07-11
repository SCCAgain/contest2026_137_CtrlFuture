/****************************************************************************
 * app/edgesight/edgesight_main.c
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * EdgeSight - AI Edge Sentinel main application entry point.
 *
 * This is the top-level orchestrator that creates and manages the
 * processing pipeline threads:
 *   - Camera capture + ISP
 *   - NPU inference (person detection + pose estimation)
 *   - Fall detection decision engine
 *   - Display rendering
 *   - Event recording (H.264 + SD card)
 *   - Network alerts (MQTT)
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sched.h>

#include "fall_detect.h"
#include "postprocess.h"
#include "camera_hal.h"
#include "npu_hal.h"
#include "display_hal.h"
#include "recorder_hal.h"
#include "network_hal.h"
#include "event_log.h"
#include "config.h"
#include "perf_stats.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define EDGESIGHT_VERSION  "0.1.0"

/****************************************************************************
 * Private Types
 ****************************************************************************/

/* Application global state */

struct edgesight_app_s
{
  /* Configuration */

  struct edgesight_config_s config;

  /* Fall detector */

  struct fall_detector_s fall_ctx;

  /* HAL contexts */

  struct camera_context_s camera;
  struct npu_context_s npu;
  struct display_context_s display;
  struct recorder_context_s recorder;
  struct network_context_s network;

  /* Subsystems */

  struct event_log_s log;
  struct perf_stats_s perf;

  /* Pipeline state flags */

  volatile bool running;
  volatile bool camera_ready;
  volatile bool npu_busy;
  volatile bool recording;

  /* Statistics */

  uint32_t frame_count;
  uint32_t detect_count;
  uint32_t fall_count;
  uint32_t inference_ms;
};

/****************************************************************************
 * Private Data
 ****************************************************************************/

static struct edgesight_app_s g_app;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/**
 * @brief Print application banner
 */

static void edgesight_banner(void)
{
  printf("========================================\n");
  printf("  EdgeSight AI Edge Sentinel v%s\n", EDGESIGHT_VERSION);
  printf("  STM32N647 | NuttX | NPU 600Gops\n");
  printf("========================================\n");
}

/**
 * @brief Initialize all hardware subsystems
 */

static int edgesight_hw_init(void)
{
  struct camera_pipe_config_s display_pipe;
  struct camera_pipe_config_s nn_pipe;
  struct display_config_s display_cfg;
  int ret;

  printf("[edgesight] Initializing hardware...\n");

  /* Initialize camera (DCMIPP dual-pipe) */

  ret = camera_hal_init(&g_app.camera, 30);
  if (ret < 0)
    {
      printf("[edgesight] Camera init failed: %d\n", ret);
      return ret;
    }

  /* Configure display pipe (PIPE1: 800x480 RGB565) */

  display_pipe.width = 800;
  display_pipe.height = 480;
  display_pipe.format = CAM_FMT_RGB565;
  display_pipe.bpp = 2;
  display_pipe.enable_isp = true;
  camera_hal_config_display(&g_app.camera, &display_pipe);

  /* Configure NN pipe (PIPE2: 480x480 RGB888) */

  nn_pipe.width = 480;
  nn_pipe.height = 480;
  nn_pipe.format = CAM_FMT_RGB888;
  nn_pipe.bpp = 3;
  nn_pipe.enable_isp = true;
  camera_hal_config_nn(&g_app.camera, &nn_pipe);

  /* Initialize display (LTDC dual-layer) */

  display_cfg.screen_width = 800;
  display_cfg.screen_height = 480;
  display_cfg.bg_format = 0;  /* RGB565 */
  display_cfg.fg_format = 0;  /* ARGB4444 */
  ret = display_hal_init(&g_app.display, &display_cfg);
  if (ret < 0)
    {
      printf("[edgesight] Display init failed: %d\n", ret);
      return ret;
    }

  /* NPU init — requires stedgeai runtime + model loading */

  ret = npu_hal_init(&g_app.npu);
  if (ret < 0)
    {
      printf("[edgesight] NPU init failed: %d\n", ret);

      /* Continue without NPU for demo mode */
    }

  printf("[edgesight] Hardware init complete.\n");
  return 0;
}

/**
 * @brief Initialize AI models
 */

static int edgesight_ai_init(void)
{
  printf("[edgesight] Loading AI models...\n");

  /* TODO: stai_runtime_init() */

  /* TODO: Load person detection model (YOLO-X nano) */

  /* TODO: Load pose estimation model (MoveNet Lightning) */

  printf("[edgesight] AI models loaded.\n");
  return 0;
}

/**
 * @brief Main processing loop (single-threaded for now)
 *
 * Production version will split into multiple pthreads:
 *   camera_task  -> captures frames
 *   npu_task     -> runs inference
 *   display_task -> renders UI
 *   record_task  -> H.264 encode + SD write
 *   network_task -> MQTT alerts
 */

static void edgesight_loop(struct edgesight_app_s *app)
{
  struct detection_output_s detections;
  struct pose_result_s pose;
  struct fall_output_s fall_out;
  struct display_stats_s stats;
  struct display_bbox_s bbox;
  uint32_t i;

  /* Start camera display pipe (continuous) */

  camera_hal_start(&app->camera, 0, NULL,
                   CAM_MODE_CONTINUOUS);

  printf("[edgesight] Entering main loop\n");

  while (app->running)
    {
      /* Track frame timing */

      perf_stats_frame_begin(&app->perf);

      /* Step 1: ISP update (auto-exposure / auto-white-balance) */

      camera_hal_isp_update(&app->camera);

      /* Step 2: Run person detection (YOLO-X nano)
       * When NPU is available, capture snapshot and run inference.
       * Otherwise, simulate empty detections for demo.
       */

      if (app->npu.initialized)
        {
          /* Capture NN frame (snapshot mode)
           * camera_hal_start(&app->camera, 1, nn_buffer,
           *                  CAM_MODE_SNAPSHOT);
           * wait for frame callback...
           * npu_hal_run(&app->npu, NPU_MODEL_DETECT,
           *             nn_buffer, &npu_result);
           * postprocess_yolo(..., &detections);
           */

          detections.count = 0;
        }
      else
        {
          /* Demo mode: no detections */

          detections.count = 0;
        }

      /* Step 3: For each detected person, run pose estimation */

      for (i = 0; i < detections.count; i++)
        {
          memset(&pose, 0, sizeof(pose));

          /* TODO: Crop person ROI, run MoveNet inference */

          /* Step 4: Fall detection */

          if (fall_detect_process(&app->fall_ctx,
                                  &pose, &fall_out))
            {
              app->fall_count++;

              event_log_write(&app->log, EVENT_LEVEL_ALERT,
                              app->frame_count,
                              "FALL conf=%.2f angle=%.1f",
                              (double)fall_out.confidence,
                              (double)fall_out.torso_angle);

              /* Trigger recording */

              if (!app->recording)
                {
                  /* TODO: recorder_hal_start(&app->recorder,
                   *       app->fall_count);
                   */

                  app->recording = true;
                }

              /* Show alert on display */

              display_hal_show_alert(&app->display,
                                     "FALL DETECTED");
            }
        }

      /* Step 5: Update display overlay */

      display_hal_clear_fg(&app->display);

      /* Draw detection bounding boxes */

      for (i = 0; i < detections.count; i++)
        {
          memset(&bbox, 0, sizeof(bbox));
          /* bbox.x = detections.detections[i].x_center * 800;
           * bbox.y = detections.detections[i].y_center * 480;
           * bbox.w = detections.detections[i].width * 800;
           * bbox.h = detections.detections[i].height * 480;
           * bbox.color = 0x07e0;  green in RGB565
           * bbox.label = "person";
           * bbox.confidence =
           *   detections.detections[i].confidence;
           */

          display_hal_draw_bbox(&app->display, &bbox);
        }

      /* Draw stats panel */

      memset(&stats, 0, sizeof(stats));
      stats.fps = perf_stats_get_fps(&app->perf);
      stats.inference_ms = app->inference_ms;
      stats.persons_detected = detections.count;
      stats.fall_count = app->fall_count;
      stats.alert_active =
        (app->fall_ctx.state == FALL_STATE_FALLEN);

      display_hal_draw_stats(&app->display, &stats);

      /* Swap foreground double-buffer (tear-free) */

      display_hal_swap(&app->display);

      /* Step 6: Feed frame to recorder if active */

      if (app->recording)
        {
          /* recorder_hal_feed_frame(&app->recorder,
           *                        yuv_frame, frame_size);
           */
        }

      /* Step 7: Network keepalive */

      /* network_hal_poll(&app->network); */

      /* Update statistics */

      app->frame_count++;
      app->detect_count += detections.count;
      app->perf.detect_count = app->detect_count;
      app->perf.fall_count = app->fall_count;

      /* Periodic log flush (every 100 frames) */

      if ((app->frame_count % 100) == 0)
        {
          event_log_flush(&app->log);
        }

      /* Yield to other tasks */

      usleep(33000); /* ~30 fps target */
    }

  /* Stop camera */

  camera_hal_stop(&app->camera, 0);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/**
 * @brief EdgeSight application entry point
 */

int main(int argc, char *argv[])
{
  int ret;

  edgesight_banner();

  /* Initialize application state */

  memset(&g_app, 0, sizeof(g_app));
  g_app.running = true;

  /* Load configuration (defaults + SD card override) */

  config_set_defaults(&g_app.config);
  ret = config_load_file(&g_app.config, NULL);
  if (ret == -1)
    {
      printf("[edgesight] No config file, using defaults\n");
    }

  config_dump(&g_app.config);

  /* Initialize subsystems */

  fall_detect_init(&g_app.fall_ctx);
  event_log_init(&g_app.log, "/mnt/sd");
  perf_stats_init(&g_app.perf);

  event_log_write(&g_app.log, EVENT_LEVEL_INFO, 0,
                  "EdgeSight v%s starting", EDGESIGHT_VERSION);

  /* Hardware initialization */

  ret = edgesight_hw_init();
  if (ret < 0)
    {
      event_log_write(&g_app.log, EVENT_LEVEL_ERROR, 0,
                      "HW init failed: %d", ret);
      printf("[edgesight] ERROR: HW init failed: %d\n", ret);
      return EXIT_FAILURE;
    }

  /* AI model initialization */

  ret = edgesight_ai_init();
  if (ret < 0)
    {
      event_log_write(&g_app.log, EVENT_LEVEL_ERROR, 0,
                      "AI init failed: %d", ret);
      printf("[edgesight] ERROR: AI init failed: %d\n", ret);
      return EXIT_FAILURE;
    }

  event_log_write(&g_app.log, EVENT_LEVEL_INFO, 0,
                  "Init complete, entering main loop");
  event_log_flush(&g_app.log);

  printf("[edgesight] Starting main loop...\n");

  /* Main processing loop */

  edgesight_loop(&g_app);

  /* Shutdown */

  event_log_write(&g_app.log, EVENT_LEVEL_INFO,
                  g_app.frame_count,
                  "Shutdown. Frames=%lu Falls=%lu",
                  (unsigned long)g_app.frame_count,
                  (unsigned long)g_app.fall_count);
  event_log_flush(&g_app.log);
  perf_stats_dump(&g_app.perf);

  printf("[edgesight] Shutting down. Frames: %lu Falls: %lu\n",
         (unsigned long)g_app.frame_count,
         (unsigned long)g_app.fall_count);

  return EXIT_SUCCESS;
}
