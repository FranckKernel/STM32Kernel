#pragma once
typedef enum
{
	WWDG_IRQn		= 0,
	PVD_IRQn		= 1,
	TAMP_STAMP_IRQn = 2,
	RTC_WKUP_IRQn	= 3,
	FLASH_IRQn		= 4,
	RCC_IRQn		= 5,
	EXTI0_IRQn		= 6,
	EXTI1_IRQn		= 7,
	EXTI2_IRQn		= 8,
	EXTI3_IRQn		= 9,
	EXTI4_IRQn		= 10,

	DMA1_Stream0_IRQn = 11,
	DMA1_Stream1_IRQn = 12,
	DMA1_Stream2_IRQn = 13,
	DMA1_Stream3_IRQn = 14,
	DMA1_Stream4_IRQn = 15,
	DMA1_Stream5_IRQn = 16,
	DMA1_Stream6_IRQn = 17,

	ADC_IRQn = 18,

	EXTI9_5_IRQn = 23,

	TIM1_BRK_TIM9_IRQn		= 24,
	TIM1_UP_TIM10_IRQn		= 25,
	TIM1_TRG_COM_TIM11_IRQn = 26,
	TIM1_CC_IRQn			= 27,

	TIM2_IRQn = 28,

	TIM3_IRQn = 29,
	TIM4_IRQn = 30,
} IRQn_Type;
