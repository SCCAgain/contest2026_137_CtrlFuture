/****************************************************************************
 * arch/arm/src/stm32n6/stm32n6_otg.h
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * STM32N6 USB OTG HS driver header.
 * Supports device mode for UVC (webcam) and mass storage.
 *
 ****************************************************************************/

#ifndef __ARCH_ARM_SRC_STM32N6_STM32N6_OTG_H
#define __ARCH_ARM_SRC_STM32N6_STM32N6_OTG_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <stdint.h>

/****************************************************************************
 * Public Types
 ****************************************************************************/

/* USB OTG mode */

enum usb_otg_mode_e
{
  USB_OTG_MODE_DEVICE = 0,
  USB_OTG_MODE_HOST   = 1,
};

/* USB OTG speed */

enum usb_otg_speed_e
{
  USB_OTG_SPEED_FULL = 0,  /* 12 Mbps */
  USB_OTG_SPEED_HIGH = 1,  /* 480 Mbps */
};

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

/**
 * @brief Initialize USB OTG in device mode
 * @param speed Desired speed (FULL or HIGH)
 * @return 0 on success
 */

int stm32n6_otg_device_init(enum usb_otg_speed_e speed);

/**
 * @brief Initialize USB OTG in host mode
 * @return 0 on success
 */

int stm32n6_otg_host_init(void);

/**
 * @brief Deinitialize USB OTG
 * @return 0 on success
 */

int stm32n6_otg_deinit(void);

#endif /* __ARCH_ARM_SRC_STM32N6_STM32N6_OTG_H */
