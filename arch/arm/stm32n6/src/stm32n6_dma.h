/****************************************************************************
 * arch/arm/src/stm32n6/stm32n6_dma.h
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * STM32N6 GPDMA driver header.
 *
 ****************************************************************************/

#ifndef __ARCH_ARM_SRC_STM32N6_STM32N6_DMA_H
#define __ARCH_ARM_SRC_STM32N6_STM32N6_DMA_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <stdint.h>

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

/**
 * @brief Initialize DMA channel
 * @param channel Channel number (0-15)
 * @return 0 on success
 */

int stm32n6_dma_init(int channel);

/**
 * @brief Start DMA transfer
 * @param channel Channel number
 * @param src Source address
 * @param dst Destination address
 * @param size Transfer size in bytes
 * @return 0 on success
 */

int stm32n6_dma_start(int channel, uint32_t src, uint32_t dst,
                       uint32_t size);

/**
 * @brief Wait for DMA transfer complete
 * @param channel Channel number
 * @param timeout_ms Timeout in milliseconds
 * @return 0 on complete, -ETIMEDOUT on timeout
 */

int stm32n6_dma_wait(int channel, int timeout_ms);

/**
 * @brief Deinitialize DMA channel
 * @param channel Channel number
 */

void stm32n6_dma_deinit(int channel);

#endif /* __ARCH_ARM_SRC_STM32N6_STM32N6_DMA_H */
