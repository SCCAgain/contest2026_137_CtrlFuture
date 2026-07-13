############################################################################
# app/edgesight/CMakeLists.txt
#
# SPDX-License-Identifier: Apache-2.0
#
############################################################################

if(CONFIG_APP_EDGESIGHT)

nuttx_add_application(
  NAME edgesight
  PRIORITY ${CONFIG_EDGESIGHT_PRIORITY}
  STACKSIZE ${CONFIG_EDGESIGHT_STACKSIZE}
  SRCS edgesight_main.c fall_detect.c postprocess.c
       npu_hal.c camera_hal.c display_hal.c recorder_hal.c network_hal.c
       event_log.c config.c perf_stats.c alert_msg.c
       sensor_fusion.c env_sensor.c npu_pipeline.c edgesight_cmd.c
)

endif()
