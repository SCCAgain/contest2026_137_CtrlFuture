/****************************************************************************
 * arch/arm/src/stm32n6/stm32n6_ethernet.h
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * STM32N6 Ethernet GMAC driver header.
 *
 ****************************************************************************/

#ifndef __ARCH_ARM_SRC_STM32N6_STM32N6_ETHERNET_H
#define __ARCH_ARM_SRC_STM32N6_STM32N6_ETHERNET_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <stdint.h>

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

/**
 * @brief Initialize Ethernet GMAC
 * @param mac_addr MAC address (6 bytes), NULL for default
 * @return 0 on success
 */

int stm32n6_ethernet_init(uint8_t *mac_addr);

/**
 * @brief Deinitialize Ethernet GMAC
 * @return 0 on success
 */

int stm32n6_ethernet_deinit(void);

#endif /* __ARCH_ARM_SRC_STM32N6_STM32N6_ETHERNET_H */
