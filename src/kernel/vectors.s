.syntax unified
.cpu cortex-m4
.thumb

.global vector_table
.global __Vectors_End
.global __Vectors_Size


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

    .word Timer3Handler    /* 29 TIM3 */
    .word Default_Handler    /* 30 TIM4 */
	    .word Default_Handler    /* 31 I2C1_EV */
    .word Default_Handler    /* 32 I2C1_ER */
    .word Default_Handler    /* 33 I2C2_EV */
    .word Default_Handler    /* 34 I2C2_ER */
    .word Default_Handler    /* 35 SPI1 */
    .word Default_Handler    /* 36 SPI2 */
    .word Default_Handler    /* 37 USART1 */
    .word Default_Handler    /* 38 USART2 */
    .word 0                  /* 39 Reserved */
    .word Default_Handler    /* 40 EXTI15_10 */
    .word Default_Handler    /* 41 RTC_Alarm */
    .word Default_Handler    /* 42 OTG_FS_WKUP */
    .word 0                  /* 43 Reserved */
    .word 0                  /* 44 Reserved */
    .word 0                  /* 45 Reserved */
    .word 0                  /* 46 Reserved */
    .word Default_Handler    /* 47 DMA1 Stream 7 */
    .word 0                  /* 48 Reserved */
    .word Default_Handler    /* 49 SDIO */
    .word Timer5Handler    /* 50 TIM5 */
    .word Default_Handler    /* 51 SPI3 */
    .word 0                  /* 52 Reserved */
    .word 0                  /* 53 Reserved */
    .word 0                  /* 54 Reserved */
    .word 0                  /* 55 Reserved */
    .word Default_Handler    /* 56 DMA2 Stream 0 */
    .word Default_Handler    /* 57 DMA2 Stream 1 */
    .word Default_Handler    /* 58 DMA2 Stream 2 */
    .word Default_Handler    /* 59 DMA2 Stream 3 */
    .word Default_Handler    /* 60 DMA2 Stream 4 */
    .word 0                  /* 61 Reserved */
    .word 0                  /* 62 Reserved */
    .word 0                  /* 63 Reserved */
    .word 0                  /* 64 Reserved */
    .word 0                  /* 65 Reserved */
    .word 0                  /* 66 Reserved */
    .word Default_Handler    /* 67 OTG_FS */
    .word Default_Handler    /* 68 DMA2 Stream 5 */
    .word Default_Handler    /* 69 DMA2 Stream 6 */
    .word Default_Handler    /* 70 DMA2 Stream 7 */
    .word Default_Handler    /* 71 USART6 */
    .word Default_Handler    /* 72 I2C3_EV */
    .word Default_Handler    /* 73 I2C3_ER */
    .word 0                  /* 74 Reserved */
    .word 0                  /* 75 Reserved */
    .word 0                  /* 76 Reserved */
    .word 0                  /* 77 Reserved */
    .word 0                  /* 78 Reserved */
    .word 0                  /* 79 Reserved */
    .word 0                  /* 80 Reserved */
    .word Default_Handler    /* 81 FPU */
    .word 0                  /* 82 Reserved */
    .word 0                  /* 83 Reserved */
    .word Default_Handler    /* 84 SPI4 */
    .word Default_Handler    /* 85 SPI5 */

__Vectors_End:
.size vector_table, __Vectors_End - vector_table
.set __Vectors_Size, __Vectors_End - vector_table

