/****************************************************************************
 * arch/arm/src/stm32n6/stm32n6_i2c.h
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * STM32N6 I2C driver header.
 * Adapted from STM32H7 NuttX reference.
 *
 ****************************************************************************/

#ifndef __ARCH_ARM_SRC_STM32N6_STM32N6_I2C_H
#define __ARCH_ARM_SRC_STM32N6_STM32N6_I2C_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <nuttx/i2c/i2c_master.h>

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

/**
 * @brief Initialize I2C bus
 * @param bus_num I2C bus number (1-4)
 * @return I2C master device, or NULL on error
 */

struct i2c_master_s *stm32n6_i2cbus_initialize(int bus_num);

/**
 * @brief Uninitialize I2C bus
 * @param dev I2C master device
 * @return 0 on success
 */

int stm32n6_i2cbus_uninitialize(struct i2c_master_s *dev);

#endif /* __ARCH_ARM_SRC_STM32N6_STM32N6_I2C_H */
