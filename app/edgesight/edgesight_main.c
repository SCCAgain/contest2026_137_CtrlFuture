/****************************************************************************
 * app/edgesight/edgesight_main.c
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * EdgeSight - AI Edge Sentinel main application entry point.
 *
 * Processing pipeline:
 *   Camera -> ISP -> NPU inference -> Fall detection
 *   -> Sensor fusion -> Display overlay -> Event recording
 *   -> Network alerts
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
#include "alert_msg.h"
#include "sensor_fusion.h"
#include "env_sensor.h"
#include "npu_pipeline.h"
#include "edgesight_cmd.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define EDGESIGHT_VERSION  "0.2.0"

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct edgesight_app_s
{
  struct edgesight_config_s config;
  struct fall_detector_s fall_ctx;
  struct camera_context_s camera;
  struct npu_context_s npu;
  struct display_context_s display;
  struct recorder_context_s recorder;
  struct network_context_s network;
  struct event_log_s log;
  struct perf_stats_s perf;
  struct fusion_context_s fusion;
  struct env_sensor_context_s env_sensor;
  struct npu_pipeline_s npu_pipe;
  struct edgesight_cmd_context_s cmd_ctx;
  volatile bool running;
  volatile bool recording;
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

static void edgesight_banner(void)
{
  printf("========================================\n");
  printf("  EdgeSight AI Edge Sentinel v%s\n",
         EDGESIGHT_VERSION);
  printf("  STM32N647 | NuttX | NPU 600Gops\n");
  printf("========================================\n");
}

static int edgesight_hw_init(void)
{
  struct camera_pipe_config_s display_pipe;
  struct camera_pipe_config_s nn_pipe;
  struct display_config_s display_cfg;
  int ret;

  printf("[edgesight] Initializing hardware...\n");

<<<<<<< HEAD
  /* Initialize camera (DCMIPP dual-pipe) */
=======
  /* Camera (DCMIPP dual-pipe) */
>>>>>>> ba5eee5 (app: integrate sensor fusion, env sensor, NPU pipeline and NSH command)

  ret = camera_hal_init(&g_app.camera, 30);
  if (ret < 0)
    {
      printf("[edgesight] Camera init failed: %d\n", ret);
      return ret;
    }

<<<<<<< HEAD
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
=======
  display_pipe.width = 800;
  display_pipe.height = 480;
  display_pipe.format = CAM_FMT_RGB565;
  display_pipe.bpp = 2;
  display_pipe.enable_isp = true;
  camera_hal_config_display(&g_app.camera,
                            &display_pipe);

  nn_pipe.width = 480;
  nn_pipe.height = 480;
  nn_pipe.format = CAM_FMT_RGB888;
  nn_pipe.bpp = 3;
  nn_pipe.enable_isp = true;
  camera_hal_config_nn(&g_app.camera, &nn_pipe);

  /* Display (LTDC dual-layer) */

  display_cfg.screen_width = 800;
  display_cfg.screen_height = 480;
  display_cfg.bg_format = 0;
  display_cfg.fg_format = 0;
>>>>>>> ba5eee5 (app: integrate sensor fusion, env sensor, NPU pipeline and NSH command)
  ret = display_hal_init(&g_app.display, &display_cfg);
  if (ret < 0)
    {
      printf("[edgesight] Display init failed: %d\n", ret);
      return ret;
    }

<<<<<<< HEAD
  /* NPU init — requires stedgeai runtime + model loading */
=======
  /* NPU */
>>>>>>> ba5eee5 (app: integrate sensor fusion, env sensor, NPU pipeline and NSH command)

  ret = npu_hal_init(&g_app.npu);
  if (ret < 0)
    {
      printf("[edgesight] NPU init failed: %d\n", ret);
<<<<<<< HEAD

      /* Continue without NPU for demo mode */
    }
=======
    }

  /* Environmental sensors */

  env_sensor_init(&g_app.env_sensor);
>>>>>>> ba5eee5 (app: integrate sensor fusion, env sensor, NPU pipeline and NSH command)

  printf("[edgesight] Hardware init complete.\n");
  return 0;
}

static int edgesight_ai_init(void)
{
  int ret;

  printf("[edgesight] Loading AI models...\n");

  ret = npu_pipeline_init(&g_app.npu_pipe);
  if (ret < 0)
    {
      printf("[edgesight] Pipeline init failed: %d\n", ret);
      return ret;
    }

  ret = npu_pipeline_load_models(&g_app.npu_pipe);
  if (ret < 0)
    {
      printf("[edgesight] Model load failed: %d\n", ret);
      return ret;
    }

  printf("[edgesight] AI models loaded.\n");
  return 0;
}

static void edgesight_loop(struct edgesight_app_s *app)
{
  struct detection_output_s detections;
  struct pose_result_s poses[PP_MAX_DETECTIONS];
  struct fall_output_s fall_out;
  struct display_stats_s stats;
<<<<<<< HEAD
  struct display_bbox_s bbox;
  uint32_t i;

  /* Start camera display pipe (continuous) */
=======
  struct fusion_decision_s decision;
  struct env_sensor_data_s env_data;
  struct fusion_input_s fusion_input;
  struct display_bbox_s bbox;
  bool fall_detected;
  uint32_t i;

  /* Start camera display pipe */
>>>>>>> ba5eee5 (app: integrate sensor fusion, env sensor, NPU pipeline and NSH command)

  camera_hal_start(&app->camera, 0, NULL,
                   CAM_MODE_CONTINUOUS);

  printf("[edgesight] Entering main loop\n");

  while (app->running)
    {
      perf_stats_frame_begin(&app->perf);
<<<<<<< HEAD

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
=======

      /* ISP update */

      camera_hal_isp_update(&app->camera);

      /* Run NPU pipeline (detect + pose + fall) */

      npu_pipeline_run(&app->npu_pipe, NULL, 0, 0,
                       &detections, poses, &fall_out,
                       &fall_detected);
>>>>>>> ba5eee5 (app: integrate sensor fusion, env sensor, NPU pipeline and NSH command)

      /* Update sensor fusion with vision result */

      if (fall_detected)
        {
          fusion_input.type = FUSION_INPUT_VISION;
          fusion_input.confidence = fall_out.confidence;
          fusion_input.value = 0;
          fusion_input.valid = true;
          fusion_update_input(&app->fusion, &fusion_input);
        }

      /* Read environmental sensors */

      env_sensor_read(&app->env_sensor, &env_data);

      if (env_data.valid[ENV_SENSOR_PIR])
        {
          fusion_input.type = FUSION_INPUT_PIR;
          fusion_input.confidence =
            env_data.pir_motion ? 0.8f : 0.0f;
          fusion_input.value = 0;
          fusion_input.valid = true;
          fusion_update_input(&app->fusion, &fusion_input);
        }

      /* Run fusion decision */

      if (fusion_decide(&app->fusion, &decision))
        {
          app->fall_count++;

          event_log_write(&app->log, EVENT_LEVEL_ALERT,
                          app->frame_count,
                          "%s conf=%.2f",
                          decision.description,
                          (double)decision.confidence);

          if (!app->recording)
            {
              app->recording = true;
            }

          display_hal_show_alert(&app->display,
                                 decision.description);
        }

      /* Update display overlay */

      display_hal_clear_fg(&app->display);

      for (i = 0; i < detections.count; i++)
        {
<<<<<<< HEAD
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

=======
          memset(&bbox, 0, sizeof(bbox));
          display_hal_draw_bbox(&app->display, &bbox);
        }

>>>>>>> ba5eee5 (app: integrate sensor fusion, env sensor, NPU pipeline and NSH command)
      memset(&stats, 0, sizeof(stats));
      stats.fps = perf_stats_get_fps(&app->perf);
      stats.inference_ms = app->inference_ms;
      stats.persons_detected = detections.count;
      stats.fall_count = app->fall_count;
      stats.alert_active =
        (app->fall_ctx.state == FALL_STATE_FALLEN);

      display_hal_draw_stats(&app->display, &stats);
<<<<<<< HEAD

      /* Swap foreground double-buffer (tear-free) */

      display_hal_swap(&app->display);
=======
      display_hal_swap(&app->display);

      /* Recording feed */
>>>>>>> ba5eee5 (app: integrate sensor fusion, env sensor, NPU pipeline and NSH command)

      if (app->recording)
        {
          /* TODO: recorder_hal_feed_frame */
        }

<<<<<<< HEAD
      if (app->recording)
        {
          /* recorder_hal_feed_frame(&app->recorder,
           *                        yuv_frame, frame_size);
           */
        }

      /* Step 7: Network keepalive */
=======
      /* Network keepalive */
>>>>>>> ba5eee5 (app: integrate sensor fusion, env sensor, NPU pipeline and NSH command)

      /* network_hal_poll(&app->network); */

      /* Update statistics */

      app->frame_count++;
      app->detect_count += detections.count;
      app->perf.detect_count = app->detect_count;
      app->perf.fall_count = app->fall_count;

      if ((app->frame_count % 100) == 0)
        {
          event_log_flush(&app->log);
        }

      usleep(33000);
    }

<<<<<<< HEAD
  /* Stop camera */

=======
>>>>>>> ba5eee5 (app: integrate sensor fusion, env sensor, NPU pipeline and NSH command)
  camera_hal_stop(&app->camera, 0);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int main(int argc, char *argv[])
{
  int ret;

  edgesight_banner();

  memset(&g_app, 0, sizeof(g_app));
  g_app.running = true;

  /* Load configuration */

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
  fusion_init(&g_app.fusion);

  /* Init NSH command interface */

  g_app.cmd_ctx.running = &g_app.running;
  g_app.cmd_ctx.recording = &g_app.recording;
  g_app.cmd_ctx.frame_count = g_app.frame_count;
  g_app.cmd_ctx.detect_count = g_app.detect_count;
  g_app.cmd_ctx.fall_count = g_app.fall_count;
  g_app.cmd_ctx.fall_ctx = &g_app.fall_ctx;
  g_app.cmd_ctx.perf = &g_app.perf;
  g_app.cmd_ctx.log = &g_app.log;
  edgesight_cmd_init(&g_app.cmd_ctx);

  event_log_write(&g_app.log, EVENT_LEVEL_INFO, 0,
                  "EdgeSight v%s starting",
                  EDGESIGHT_VERSION);

  /* Hardware initialization */

  ret = edgesight_hw_init();
  if (ret < 0)
    {
      event_log_write(&g_app.log, EVENT_LEVEL_ERROR, 0,
                      "HW init failed: %d", ret);
      printf("[edgesight] ERROR: HW init failed: %d\n",
             ret);
      return EXIT_FAILURE;
    }

  /* AI model initialization */

  ret = edgesight_ai_init();
  if (ret < 0)
    {
      event_log_write(&g_app.log, EVENT_LEVEL_ERROR, 0,
                      "AI init failed: %d", ret);
      printf("[edgesight] ERROR: AI init failed: %d\n",
             ret);
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

  printf("[edgesight] Shutting down. Frames: %lu "
         "Falls: %lu\n",
         (unsigned long)g_app.frame_count,
         (unsigned long)g_app.fall_count);

  return EXIT_SUCCESS;
}
