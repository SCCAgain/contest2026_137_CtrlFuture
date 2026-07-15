/****************************************************************************
 * arch/arm/src/stm32n6/stm32n6_start.c
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to you under the Apache License, Version
 * 2.0 (the "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied.  See the License for the specific language governing
 * permissions and limitations under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <nuttx/init.h>

#include <stdint.h>

#include <arch/board/board.h>

#include "arm_internal.h"
#include "nvic.h"
#include "stm32n6_rcc.h"
#include "stm32n6_lowputc.h"

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

void stm32_boardinitialize(void);

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define IDLE_STACK \
  ((uintptr_t)_ebss + CONFIG_IDLETHREAD_STACKSIZE)

/* VTOR register address */

#define NVIC_VECTAB        0xe000ed08

/****************************************************************************
 * Public Data
 ****************************************************************************/

const uintptr_t g_idle_topstack = IDLE_STACK;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: showprogress
 *
 * Description:
 *   Print a character on the UART to show boot progress.
 *
 ****************************************************************************/

#ifdef CONFIG_DEBUG_FEATURES
static inline void showprogress(char c)
{
  arm_lowputc(c);
}
#else
#  define showprogress(c)
#endif

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: stm32_reset_dispatch
 *
 * Description:
 *   The STM32N6 boot ROM sets MSPLIM/PSPLIM stack limit registers.
 *   We must clear them BEFORE any compiler-generated prologue runs,
 *   otherwise the first stack access triggers a UsageFault.
 *   This naked function is the true reset vector entry point.
 *
 ****************************************************************************/

void __attribute__((naked, noreturn)) stm32_reset_dispatch(void)
{
  __asm__ __volatile__
    (

      /* Clear MSPLIM and PSPLIM so compiler prologues don't fault */

      "mov r0, #0\n\t"
      "msr msplim, r0\n\t"
      "msr psplim, r0\n\t"

      /* Fall through to __start */

      "b __start\n\t"
    );
}

/****************************************************************************
 * Name: __start
 *
 * Description:
 *   This is the main reset entry point (called from naked dispatch).
 *
 ****************************************************************************/

void __start(void)
{
  const uint32_t *src;
  uint32_t *dest;

  /* Relocate vector table to our SRAM load address */

  *(volatile uint32_t *)NVIC_VECTAB = (uint32_t)_stext;

  /* Disable SysTick and clear pending exceptions
   * (boot ROM may have left them active)
   */

  *(volatile uint32_t *)0xe000e010 = 0;  /* SYST_CSR = 0 */
  *(volatile uint32_t *)0xe000e014 = 0;  /* SYST_RVR = 0 */
  *(volatile uint32_t *)0xe000ed04 = (1ul << 25);  /* ICSR.PENDSTCLR */

  /* Data/instruction sync barriers */

  __asm__ __volatile__ ("dsb sy\n\t" "isb sy\n\t");

  /* Clear .bss */

  for (dest = (uint32_t *)_sbss; dest < (uint32_t *)_ebss; )
    {
      *dest++ = 0;
    }

  /* Copy .data from flash to SRAM */

  src = (const uint32_t *)_eronly;
  dest = (uint32_t *)_sdata;
  for (; dest < (uint32_t *)_edata; )
    {
      *dest++ = *src++;
    }

  /* Configure clocks */

  stm32n6_clockconfig();

  /* Enable SYSCFG and BSEC clocks per errata ES0620 */

  *(volatile uint32_t *)0x440200f0 |= (1 << 1) | (1 << 16);

  /* Set I/O compensation and supply-valid bits
   * (required for I/O pins to work at high speed)
   */

  *(volatile uint32_t *)0x44020900 |= (1 << 0);  /* VDDIO2 valid */
  *(volatile uint32_t *)0x44020904 |= (1 << 0);  /* VDDIO3 valid */
  *(volatile uint32_t *)0x44020908 |= (1 << 0);  /* VDDIO4 valid */
  *(volatile uint32_t *)0x4402090c |= (1 << 0);  /* VDDIO5 valid */

  /* Route USART1 kernel clock to HSI (64MHz) */

  *(volatile uint32_t *)0x44020da0 &= ~(0x7 << 0);
  *(volatile uint32_t *)0x44020da0 |= (0x3 << 0);

  /* Configure the UART for early debug output */

  stm32n6_lowsetup();
  showprogress('A');

  /* Call board early initialization */

  stm32_boardinitialize();
  showprogress('B');

#ifdef USE_EARLYSERIALINIT
  arm_earlyserialinit();
#endif
  showprogress('C');

  /* Start NuttX */

  showprogress('\r');
  showprogress('\n');
  nx_start();

  for (; ; );
}
