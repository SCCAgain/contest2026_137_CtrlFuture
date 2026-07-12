/****************************************************************************
 * arch/arm/src/stm32n6/stm32n6_iwdg.h
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * STM32N6 IWDG driver header.
 *
 ****************************************************************************/

#ifndef __ARCH_ARM_SRC_STM32N6_STM32N6_IWDG_H
#define __ARCH_ARM_SRC_STM32N6_STM32N6_IWDG_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <stdint.h>

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

/**
 * @brief Initialize IWDG watchdog
 * @param timeout_ms Watchdog timeout in milliseconds
 * @return 0 on success
 */

int stm32n6_iwdg_initialize(uint32_t timeout_ms);

/**
 * @brief Feed (kick) the watchdog to prevent reset
 */

void stm32n6_iwdg_feed(void);

/**
 * @brief Get configured timeout
 * @return Timeout in milliseconds
 */

uint32_t stm32n6_iwdg_get_timeout(void);

#endif /* __ARCH_ARM_SRC_STM32N6_STM32N6_IWDG_H */
