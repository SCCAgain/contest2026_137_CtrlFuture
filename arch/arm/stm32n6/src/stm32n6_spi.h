/****************************************************************************
 * arch/arm/src/stm32n6/stm32n6_spi.h
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * STM32N6 SPI driver header.
 *
 ****************************************************************************/

#ifndef __ARCH_ARM_SRC_STM32N6_STM32N6_SPI_H
#define __ARCH_ARM_SRC_STM32N6_STM32N6_SPI_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <stdint.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* SPI mode flags (standard SPI CPOL/CPHA) */

#define SPI_CPOL         (1 << 0)
#define SPI_CPHA         (1 << 1)
#define SPI_MODE_0       0
#define SPI_MODE_1       SPI_CPHA
#define SPI_MODE_2       SPI_CPOL
#define SPI_MODE_3       (SPI_CPOL | SPI_CPHA)

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

/**
 * @brief Initialize SPI bus
 * @param bus_num SPI instance (1 or 2)
 * @return 0 on success
 */

int stm32n6_spi_initialize(int bus_num);

/**
 * @brief SPI transfer (full duplex)
 * @param bus_num SPI instance
 * @param tx TX buffer (NULL = send 0xFF)
 * @param rx RX buffer (NULL = discard)
 * @param len Transfer length in bytes
 * @return 0 on success
 */

int stm32n6_spi_transfer(int bus_num, const uint8_t *tx,
                           uint8_t *rx, uint32_t len);

/**
 * @brief Deinitialize SPI bus
 * @param bus_num SPI instance
 */

void stm32n6_spi_deinit(int bus_num);

#endif /* __ARCH_ARM_SRC_STM32N6_STM32N6_SPI_H */
