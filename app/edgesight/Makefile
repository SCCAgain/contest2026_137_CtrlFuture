############################################################################
# app/edgesight/Makefile
#
# SPDX-License-Identifier: Apache-2.0
#
############################################################################

include $(APPDIR)/Make.defs

PROGNAME  = edgesight
PRIORITY  = $(CONFIG_EDGESIGHT_PRIORITY)
STACKSIZE = $(CONFIG_EDGESIGHT_STACKSIZE)
MODULE    = $(CONFIG_APP_EDGESIGHT)

MAINSRC = edgesight_main.c

CSRCS  = fall_detect.c postprocess.c npu_hal.c camera_hal.c \
         display_hal.c recorder_hal.c network_hal.c \
         event_log.c config.c perf_stats.c alert_msg.c \
<<<<<<< HEAD
         npu_pipeline.c edgesight_cmd.c sensor_fusion.c \
         env_sensor.c
=======
         sensor_fusion.c env_sensor.c npu_pipeline.c \
         edgesight_cmd.c
>>>>>>> ba5eee5 (app: integrate sensor fusion, env sensor, NPU pipeline and NSH command)

include $(APPDIR)/Application.mk
