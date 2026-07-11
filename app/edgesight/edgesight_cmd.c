/****************************************************************************
 * app/edgesight/edgesight_cmd.c
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * EdgeSight - NSH command interface.
 * Provides runtime commands for EdgeSight management.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "edgesight_cmd.h"
#include "config.h"
#include "perf_stats.h"
#include "event_log.h"
#include "alert_msg.h"

/****************************************************************************
 * Private Data
 ****************************************************************************/

static struct edgesight_cmd_context_s *g_cmd_ctx;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void cmd_status(void)
{
  struct perf_stats_s *perf;
  struct event_log_s *log;

  if (g_cmd_ctx == NULL || !g_cmd_ctx->running)
    {
      printf("EdgeSight: not running\n");
      return;
    }

  perf = g_cmd_ctx->perf;
  log = g_cmd_ctx->log;

  printf("=== EdgeSight Status ===\n");
  printf("State: %s\n",
         g_cmd_ctx->running ? "RUNNING" : "STOPPED");
  printf("Frames: %lu\n",
         (unsigned long)g_cmd_ctx->frame_count);
  printf("Detections: %lu\n",
         (unsigned long)g_cmd_ctx->detect_count);
  printf("Falls: %lu\n",
         (unsigned long)g_cmd_ctx->fall_count);
  printf("Recording: %s\n",
         g_cmd_ctx->recording ? "YES" : "NO");

  if (perf != NULL)
    {
      printf("FPS: %lu\n",
             (unsigned long)perf_stats_get_fps(perf));
    }

  if (log != NULL)
    {
      uint32_t total;
      uint32_t flushed;
      uint16_t dropped;

      event_log_stats(log, &total, &flushed, &dropped);
      printf("Log: %lu total, %lu flushed, %u dropped\n",
             (unsigned long)total,
             (unsigned long)flushed,
             dropped);
    }
}

static void cmd_config(void)
{
  struct edgesight_config_s cfg;

  config_set_defaults(&cfg);
  config_load_file(&cfg, NULL);
  config_dump(&cfg);
}

static void cmd_log(void)
{
  if (g_cmd_ctx == NULL || g_cmd_ctx->log == NULL)
    {
      printf("No log available\n");
      return;
    }

  event_log_dump(g_cmd_ctx->log, 20);
}

static void cmd_reset(void)
{
  if (g_cmd_ctx == NULL)
    {
      printf("EdgeSight not initialized\n");
      return;
    }

  if (g_cmd_ctx->fall_ctx != NULL)
    {
      fall_detect_reset(g_cmd_ctx->fall_ctx);
      printf("Fall detector reset\n");
    }
}

static void cmd_help(void)
{
  printf("EdgeSight commands:\n");
  printf("  edgesight status   - Show runtime status\n");
  printf("  edgesight config   - Show configuration\n");
  printf("  edgesight log      - Show recent events\n");
  printf("  edgesight reset    - Reset fall detector\n");
  printf("  edgesight help     - Show this help\n");
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int edgesight_cmd_init(struct edgesight_cmd_context_s *ctx)
{
  g_cmd_ctx = ctx;
  return 0;
}

int edgesight_cmd_execute(const char *cmd)
{
  if (cmd == NULL)
    {
      return -1;
    }

  if (strcmp(cmd, "status") == 0)
    {
      cmd_status();
    }
  else if (strcmp(cmd, "config") == 0)
    {
      cmd_config();
    }
  else if (strcmp(cmd, "log") == 0)
    {
      cmd_log();
    }
  else if (strcmp(cmd, "reset") == 0)
    {
      cmd_reset();
    }
  else if (strcmp(cmd, "help") == 0)
    {
      cmd_help();
    }
  else
    {
      printf("Unknown command: %s\n", cmd);
      cmd_help();
      return -1;
    }

  return 0;
}
