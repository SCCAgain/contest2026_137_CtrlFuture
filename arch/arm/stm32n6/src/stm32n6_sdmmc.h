/****************************************************************************
 * arch/arm/src/stm32n6/stm32n6_sdmmc.h
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * STM32N6 SDMMC driver header.
 *
 ****************************************************************************/

#ifndef __ARCH_ARM_SRC_STM32N6_STM32N6_SDMMC_H
#define __ARCH_ARM_SRC_STM32N6_STM32N6_SDMMC_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <nuttx/mmcsd/mmcsd.h>

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

/**
 * @brief Initialize SDMMC and register MMC/SD device
 * @param bus_num SDMMC instance (1 or 2)
 * @return 0 on success
 */

int stm32n6_sdmmc_initialize(int bus_num);

/**
 * @brief Deinitialize SDMMC
 * @param bus_num SDMMC instance
 * @return 0 on success
 */

int stm32n6_sdmmc_deinitialize(int bus_num);

#endif /* __ARCH_ARM_SRC_STM32N6_STM32N6_SDMMC_H */
