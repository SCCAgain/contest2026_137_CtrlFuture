/****************************************************************************
 * app/edgesight/alert_msg.c
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * EdgeSight - MQTT alert message formatting implementation.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include "alert_msg.h"
#include <stdio.h>
#include <string.h>

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static const char *type_str(uint8_t type)
{
  switch (type)
    {
      case ALERT_TYPE_FALL:
        return "fall";
      case ALERT_TYPE_RECOVERY:
        return "recovery";
      case ALERT_TYPE_HEARTBEAT:
        return "heartbeat";
      default:
        return "unknown";
    }
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int alert_msg_format(char *buf, uint16_t buflen,
                     const char *device_id,
                     const struct alert_payload_s *payload)
{
  int len;

  if (buf == NULL || buflen < 64 || payload == NULL)
    {
      return -1;
    }

  len = snprintf(buf, buflen,
    "{"
    "\"device\":\"%s\","
    "\"type\":\"%s\","
    "\"ts\":%lu,"
    "\"frame\":%lu,"
    "\"confidence\":%.2f,"
    "\"angle\":%.1f,"
    "\"cog\":%.2f,"
    "\"persons\":%u,"
    "\"uptime\":%lu"
    "}",
    device_id,
    type_str(payload->type),
    (unsigned long)payload->timestamp_ms,
    (unsigned long)payload->frame_number,
    (double)payload->confidence,
    (double)payload->torso_angle,
    (double)payload->cog_height,
    payload->persons_in_scene,
    (unsigned long)payload->uptime_s);

  if (len < 0 || len >= (int)buflen)
    {
      return -1;
    }

  return len;
}

int alert_msg_heartbeat(char *buf, uint16_t buflen,
                        const char *device_id,
                        uint32_t uptime_s,
                        uint32_t frame_count,
                        uint32_t fall_count)
{
  int len;

  if (buf == NULL || buflen < 64)
    {
      return -1;
    }

  len = snprintf(buf, buflen,
    "{"
    "\"device\":\"%s\","
    "\"type\":\"heartbeat\","
    "\"uptime\":%lu,"
    "\"frames\":%lu,"
    "\"falls\":%lu"
    "}",
    device_id,
    (unsigned long)uptime_s,
    (unsigned long)frame_count,
    (unsigned long)fall_count);

  if (len < 0 || len >= (int)buflen)
    {
      return -1;
    }

  return len;
}
