.syntax unified
.cpu cortex-m4
.thumb

.global vector_table

.extern _reset
.extern Default_Handler

.section .isr_vector
.type vector_table, %object

// STM32 Reference Manual, Section10 : Interrupts and Events 
// Table 37: Vector table for STM32F411xC/E
 

vector_table:

    /* Cortex-M4 core exceptions */

    .word __stack_end
    .word _reset

    .word Default_Handler    /* NMI */
    .word Default_Handler    /* HardFault */
    .word Default_Handler    /* MemManage */
    .word Default_Handler    /* BusFault */
    .word Default_Handler    /* UsageFault */

    .word 0
    .word 0
    .word 0
    .word 0

    .word Default_Handler    /* SVC */
    .word Default_Handler    /* DebugMonitor */

    .word 0

    .word Default_Handler    /* PendSV */
    .word Default_Handler    /* SysTick */


    /* STM32F411 external interrupts */

    .word Default_Handler    /* 0  WWDG */
    .word Default_Handler    /* 1  PVD */
    .word Default_Handler    /* 2  TAMP_STAMP */
    .word Default_Handler    /* 3  RTC_WKUP */
    .word Default_Handler    /* 4  FLASH */
    .word Default_Handler    /* 5  RCC */
    .word Default_Handler    /* 6  EXTI0 */
    .word Default_Handler    /* 7  EXTI1 */
    .word Default_Handler    /* 8  EXTI2 */
    .word Default_Handler    /* 9  EXTI3 */
    .word Default_Handler    /* 10 EXTI4 */

    .word Default_Handler    /* 11 DMA1 Stream 0 */
    .word Default_Handler    /* 12 DMA1 Stream 1 */
    .word Default_Handler    /* 13 DMA1 Stream 2 */
    .word Default_Handler    /* 14 DMA1 Stream 3 */
    .word Default_Handler    /* 15 DMA1 Stream 4 */
    .word Default_Handler    /* 16 DMA1 Stream 5 */
    .word Default_Handler    /* 17 DMA1 Stream 6 */

    .word Default_Handler    /* 18 ADC */

    .word 0                  /* 19 reserved */
    .word 0                  /* 20 reserved */
    .word 0                  /* 21 reserved */
    .word 0                  /* 22 reserved */

    .word Default_Handler    /* 23 EXTI9_5 */

    .word Default_Handler    /* 24 TIM1_BRK / TIM9 */
    .word Default_Handler    /* 25 TIM1_UP / TIM10 */
    .word Default_Handler    /* 26 TIM1_TRG_COM / TIM11 */
    .word Default_Handler    /* 27 TIM1_CC */

    .word Timer2Handler    /* 28 TIM2 */

    .word Default_Handler    /* 29 TIM3 */
    .word Default_Handler    /* 30 TIM4 */

    /* ... rest of vector table ... */
