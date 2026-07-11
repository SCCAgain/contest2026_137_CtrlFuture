/****************************************************************************
 * arch/arm/src/stm32n6/stm32n6_ltdc.h
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * STM32N6 LTDC display controller driver header.
 *
 ****************************************************************************/

#ifndef __ARCH_ARM_SRC_STM32N6_STM32N6_LTDC_H
#define __ARCH_ARM_SRC_STM32N6_STM32N6_LTDC_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <stdint.h>

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

/**
 * @brief Initialize LTDC display controller
 * @param width Display width
 * @param height Display height
 * @param bg_buf Background framebuffer (camera output)
 * @param fg_buf1 Foreground framebuffer 1 (overlay)
 * @param fg_buf2 Foreground framebuffer 2 (double-buffer)
 * @return OK on success
 */

int stm32n6_ltdc_init(uint32_t width, uint32_t height,
                        void *bg_buf, void *fg_buf1,
                        void *fg_buf2);

/**
 * @brief Update background layer buffer address
 * @param buffer New framebuffer address
 * @return OK on success
 */

int stm32n6_ltdc_set_bg_buffer(void *buffer);

/**
 * @brief Swap foreground double buffer
 * @return Pointer to buffer now safe to write
 */

void *stm32n6_ltdc_swap_fg_buffer(void);

/**
 * @brief Fill a rectangle on the foreground layer
 * @param x X coordinate
 * @param y Y coordinate
 * @param w Width
 * @param h Height
 * @param color RGB565 color value
 * @return OK on success
 */

int stm32n6_ltdc_fill_fg_rect(uint32_t x, uint32_t y,
                                uint32_t w, uint32_t h,
                                uint16_t color);

/**
 * @brief Clear foreground layer to transparent
 */

void stm32n6_ltdc_clear_fg(void);

/**
 * @brief Get pointer to safe-to-write foreground buffer
 * @return Buffer pointer
 */

void *stm32n6_ltdc_get_fg_buffer(void);

#endif /* __ARCH_ARM_SRC_STM32N6_STM32N6_LTDC_H */
