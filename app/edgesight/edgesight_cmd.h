/****************************************************************************
 * app/edgesight/edgesight_cmd.h
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * EdgeSight - NSH command interface header.
 *
 ****************************************************************************/

#ifndef __APP_EDGESIGHT_CMD_H
#define __APP_EDGESIGHT_CMD_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdint.h>
#include <stdbool.h>

#include "fall_detect.h"
#include "perf_stats.h"
#include "event_log.h"

/****************************************************************************
 * Public Types
 ****************************************************************************/

/* Command context (shared with main app) */

struct edgesight_cmd_context_s
{
  volatile bool running;
  volatile bool recording;
  uint32_t frame_count;
  uint32_t detect_count;
  uint32_t fall_count;
  struct fall_detector_s *fall_ctx;
  struct perf_stats_s *perf;
  struct event_log_s *log;
};

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

/**
 * @brief Initialize NSH command interface
 * @param ctx Command context (shared with main app)
 * @return 0 on success
 */

int edgesight_cmd_init(struct edgesight_cmd_context_s *ctx);

/**
 * @brief Execute an EdgeSight command
 * @param cmd Command string (status/config/log/reset/help)
 * @return 0 on success, -1 on error
 */

int edgesight_cmd_execute(const char *cmd);

#endif /* __APP_EDGESIGHT_CMD_H */
