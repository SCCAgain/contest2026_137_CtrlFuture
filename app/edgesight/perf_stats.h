/****************************************************************************
 * app/edgesight/perf_stats.h
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * EdgeSight - Performance statistics tracking.
 * Tracks FPS, inference time, pipeline latency.
 *
 ****************************************************************************/

#ifndef __APP_EDGESIGHT_PERF_STATS_H
#define __APP_EDGESIGHT_PERF_STATS_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdint.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define PERF_WINDOW_SIZE       30   /* Rolling average window */

/****************************************************************************
 * Public Types
 ****************************************************************************/

struct perf_counter_s
{
  uint32_t samples[PERF_WINDOW_SIZE];
  uint16_t index;
  uint16_t count;
  uint32_t min_val;
  uint32_t max_val;
  uint32_t total;
};

struct perf_stats_s
{
  struct perf_counter_s frame_time;    /* ms per frame */
  struct perf_counter_s detect_time;   /* YOLO inference ms */
  struct perf_counter_s pose_time;     /* MoveNet inference ms */
  struct perf_counter_s postproc_time; /* Post-processing ms */
  struct perf_counter_s display_time;  /* Render + swap ms */
  uint32_t frame_count;
  uint32_t detect_count;
  uint32_t fall_count;
  uint32_t last_frame_ts;
};

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

/**
 * @brief Initialize performance stats
 * @param stats Stats context
 */

void perf_stats_init(struct perf_stats_s *stats);

/**
 * @brief Record a sample to a performance counter
 * @param counter Counter to update
 * @param value_ms Duration in milliseconds
 */

void perf_counter_add(struct perf_counter_s *counter,
                      uint32_t value_ms);

/**
 * @brief Get rolling average from a counter
 * @param counter Counter to query
 * @return Average in ms (0 if no samples)
 */

uint32_t perf_counter_avg(
    const struct perf_counter_s *counter);

/**
 * @brief Mark start of a new frame (updates FPS)
 * @param stats Stats context
 */

void perf_stats_frame_begin(struct perf_stats_s *stats);

/**
 * @brief Get current FPS (frames per second)
 * @param stats Stats context
 * @return FPS value
 */

uint32_t perf_stats_get_fps(
    const struct perf_stats_s *stats);

/**
 * @brief Print performance summary to console
 * @param stats Stats context
 */

void perf_stats_dump(const struct perf_stats_s *stats);

#endif /* __APP_EDGESIGHT_PERF_STATS_H */
