#pragma once
// Mainly use STM32f411re reference manual

// RCC : Reset and Clock Control
// RTC: Real Time Clock
// SW: System Clock switch
// Trapeze symbol: Multiplexer (Selector)
// AHB Presc: Advanced High Performance Bus Prescalor. A Prescaler divide a clock and outputs a slower version of it
// HCLK: AHB Clock. The clock that drives everything sitting on ahb bys. The core, memory interface, gpio and DMA.
// APB = Advanced Peripheral Bus. Simpler slower bus for peripherals (timers, uart, spi, i2c, etc).
//		| It's clock are PCLK1 (APB1), and PCLK2 APB2)
// 		| APB1 : Low speed, APB2 (Max 50Mhz): High speed (Max 100 Mhz)

// The sysclock can come from the hsi, the hse or the pllclk
// The pll clock can be sourced from the hse or the hsi

/*
	Register			What you use it for												Priority 					Done
========================================================================================================================
	RCC->AHB1ENR		Enable clocks for GPIO, DMA, etc.								⭐⭐⭐⭐⭐					Yes
	RCC->APB1ENR		Enable clocks for timers/UART/I2C/etc. on APB1					⭐⭐⭐⭐⭐					Yes
	RCC->APB2ENR		Enable clocks for timers/UART/SPI/etc. on APB2					⭐⭐⭐⭐⭐					Yes
	RCC->CFGR			Select/configure system clock and bus prescalers				⭐⭐⭐⭐					Yes
	RCC->CR				Turn HSI/HSE/PLL on/off and check whether they're ready			⭐⭐⭐⭐					Yes. (More needed)
	RCC->PLLCFGR		Configure the PLL												⭐⭐⭐						No
	RCC->CSR			LSI, reset flags, etc.											⭐⭐						No
	RCC->CIR			Clock interrupts												⭐							No
	RCC->BDCR			RTC/LSE clock													⭐							No

More needed: More work/learning needed about it
*/

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// ========= HSI CLOCK (High Speed Internal Clock) ================
// ========= HSE CLOCK (High Speed External Clock) ================
// ========= Main PLL CLOCK (Phase-Locked Loop) ================

// =========  LSI (Low Speed Internal Clock) ==================
/* 32 KHz
   Drives the InDependant WatchDog Clock (IDWDG CLK)
   optionally, the the RTC used for auto-wakeup from the stop/standby mode
   RC : Resistor Capacitor is used to drive the clock.
*/

// =========  LSE (Low Speed External Clock) ==================
/* 32.768 KHz
   Otionally drives the RTC Clock (RTCCLK)
   Crystal: More accurate then RC type clock
*/

// 1 ============================ RCC Clock Control register (RCC_CR)

typedef struct
{
	uint32_t hsi_on : 1;	// bit 0, rw
	uint32_t hsi_ready : 1; // bit 1, r

	uint32_t _reserved1 : 1; // bit 2, n/a

	uint32_t hsi_trim_0_4 : 5; // bit 3-7, rw
	uint32_t hsi_cal_0_7 : 8;  // bit 8-15, rw

	uint32_t hse_on : 1;	 // bit 16, rw
	uint32_t hse_ready : 1;	 // bit 17, r
	uint32_t hse_bypass : 1; // bit 18, rw
							 // 1 : clock is bypassed by an external clock. Can only be set when hse disabled

	uint32_t clock_security_system_enable : 1; // bit 19, rw

	uint32_t _reserved_2 : 4; // bit 20-23

	uint32_t pll_on : 1;	// bit 24, rw
	uint32_t pll_ready : 1; // bit 25, r

	uint32_t plli2s_on : 1;	   // bit 26, rw
	uint32_t plli2s_ready : 1; // bit 27, r

	uint32_t _reserved3 : 4; // bit 28-31
} rcc_cr_t;

_Static_assert(sizeof(rcc_cr_t) == sizeof(uint32_t), "rcc_cr_t : The struct is the wrong size!");

// 2 ============================ RCC PLL configuration register (RCC_PLLCFGR)

// =================================== PLL: ===============================
/*
   Right after the multiplexer, there's a divider by M. (kinda wierd). It chose M for alphabetic nearby-ness.  MN PQ R

   VCO: Voltage controlled Oscillator. The internal oscillator circuit that runs at a high frequency.
		| VCO is in the middle stage of the calculation
		f_VCO = f_input * N/M
   N : The feedback multiplier. Multiplies the clock frequency by N.

   PRQ. The final stage that goes into the output.

   PLL ClK = F_VCO / P   		| PLL CLK goes to SysClock
   PLL48CLK = F_VCO / Q			| Goes to USB, RNG and SDIO. Goal is to hit 48 MHz so usb works
   PLLR = F_VCO / R				| On other boards, connected to I2S, SAI or DSI Clocks

*/

enum pll_p_values_t
{
	pllp_2 = 0b00,
	pplp_4 = 0b01,
	pllp_6 = 0b10,
	pllp_8 = 0b11
};

enum pll_src_values_t
{
	pll_src_hsi = 0b0,
	pll_src_hse = 0b1,

};

typedef struct
{
	uint32_t m_divider : 6;		  // 0-5 (rw) 0 and 1 are wrong values
	uint32_t n_multiplicator : 9; // 6-14 (rw) 0 and 1 are wrong values
	/*
	   50 ≤PLLN ≤432
	*/
	uint32_t _reserved1 : 1; // 15

	enum pll_p_values_t p_divisor : 2; // 16-17 (rw)

	uint32_t _reserved2 : 4; // (18-21) (r only)

	enum pll_src_values_t src : 1; // 22(rw)

	uint32_t _reserved3 : 1; // 23 (r only)

	uint32_t q_divisor : 4; // 24-27 (rw)

	uint32_t _reserved4 : 4; // 28-31 (r only)
} rcc_pllcfgr_t;

_Static_assert(sizeof(rcc_pllcfgr_t) == sizeof(uint32_t), "rcc_pllcfgr_t : The struct is the wrong size!");

// 3 ============================ RCC clock configuration register (RCC_CFGR)

enum system_clock_switch_t
{
	system_clock_hsi = 0b00,
	system_clock_hse = 0b01,
	system_clock_pll = 0b10,
	// system_clock_not_allowed = 0b11,
};

enum system_clock_switch_status_t
{
	system_clock_status_hsi = 0b00,
	system_clock_status_hse = 0b01,
	system_clock_status_pll = 0b10,
	// system_clock_status_not_applicable = 0b11,
};

// hpre = aHb PREsacler
enum ahb_prescaler_values_t
{
	// Caution :
	/*
		The clocks are divided with the new prescaler factor from 1 to 16 AHB cycles after
		HPRE write.
		// This caution is done by hardware and should be fine to ignore software wise. No need to wait
	*/
	ahb_prescaler_divide_1	 = 0b0000, // not divided. 0b0xxx works too
	ahb_prescaler_divide_2	 = 0b1000,
	ahb_prescaler_divide_4	 = 0b1001,
	ahb_prescaler_divide_8	 = 0b1010,
	ahb_prescaler_divide_16	 = 0b1011,
	ahb_prescaler_divide_64	 = 0b1100,
	ahb_prescaler_divide_128 = 0b1101,
	ahb_prescaler_divide_256 = 0b1110,
	ahb_prescaler_divide_512 = 0b1111,

};

// ppre1 = aPb PREsacler 1 (low)
enum apb1_prescaler_values_t
{
	// Caution :
	/*
	   The software has to set these bits correctly not to exceed 50 MHz on this domain.
		The clocks are divided with the new prescaler factor from 1 to 16 AHB cycles after
		PPRE1 write.
	*/
	apb1_prescaler_divide_1	 = 0b000, // not divided, 0b0xx works too
	apb1_prescaler_divide_2	 = 0b100,
	apb1_prescaler_divide_4	 = 0b101,
	apb1_prescaler_divide_8	 = 0b110,
	apb1_prescaler_divide_16 = 0b111,

};

// ppre2 = aPb PREsacler 2 (high)
enum apb2_prescaler_values_t
{
	// Caution :
	/*
		The software has to set these bits correctly not to exceed 100 MHz on this domain.
		The clocks are divided with the new prescaler factor from 1 to 16 AHB cycles after
		PPRE2 write.
	*/
	apb2_prescaler_divide_1	 = 0b000, // not divided, 0b0xx works too
	apb2_prescaler_divide_2	 = 0b100,
	apb2_prescaler_divide_4	 = 0b101,
	apb2_prescaler_divide_8	 = 0b110,
	apb2_prescaler_divide_16 = 0b111,

};

enum hse_division_factor_t
{
	no_clock_1 = 0b00000,
	no_clock_2 = 0b00001,

	hse_over_2 = 0b00010,
	hse_over_3 = 0b00011,
	hse_over_4 = 0b00100,
	// ... Just put the value itself
	hse_over_30 = 0b11110,
	hse_over_31 = 0b11111,
};

enum microcontroller_clock_ouput1_t
{
	// We connected this to a gpio pin. Which clocks feeds/is the source of this.
	// Used for debugging clocks with oscilloscope
	/*
		Clock source selection may generate glitches on MCO1.
		It highly recommended to configure these bits only after reset before enabling the external
		oscillators and PLL
	*/
	mco1_hsi_selected = 0b00,
	mco1_lse_selected = 0b01,
	mco1_hse_selected = 0b10,
	mco1_pll_selected = 0b11,
};

enum i2s_clock_selection_t
{
	//
	/*
	   Set and cleared by software. This bit allows to select the I2S clock source between the
		PLLI2S clock and the external clock. It is highly recommended to change this bit only after
		reset and before enabling the I2S module.
	 */
	i2s_clock_selection_pll2s_clock		  = 0b0,
	i2s_clock_selection_exterbak_i2s_ckin = 0b1,

};

enum mco_prescaler_t
{
	mco_prescaler_1 = 0b000, // No division, 0b0xx works too
	mco_prescaler_2 = 0b100,
	mco_prescaler_3 = 0b101,
	mco_prescaler_4 = 0b110,
	mco_prescaler_5 = 0b111,
};

enum microcontroller_clock_ouput2_t
{
	// We connected this to a gpio pin. Which clocks feeds/is the source of this.
	// Used for debugging clocks with oscilloscope
	/*
		Clock source selection may generate glitches on MCO1.
		It highly recommended to configure these bits only after reset before enabling the external
		oscillators and PLL
	*/
	mco2_system_clock_selected = 0b00,
	mco2_pll2s_selected		   = 0b01,
	mco2_hse_selected		   = 0b10,
	mco2_pll_selected		   = 0b11,
};

typedef struct
{
	enum system_clock_switch_t		  system_clock_switch : 2;		  // bit 0-1
	enum system_clock_switch_status_t system_clock_switch_status : 2; // bit 2-3
	// When you change the system clock source, you must poll with the status right after!
	enum ahb_prescaler_values_t ahb_prescaler : 4; // bit 4-7

	uint32_t _reserved1 : 2; // bit 8-9

	enum apb1_prescaler_values_t apb1_prescaler : 3; // bit 10-12
	enum apb2_prescaler_values_t apb2_prescaler : 3; // bit 13-15

	enum hse_division_factor_t rtc_pre_when_hse : 5; // bit 16 - 20

	enum microcontroller_clock_ouput1_t mco1_source : 2;	  // bit 21-22
	enum i2s_clock_selection_t			i2s_clock_source : 1; // bit 23
	enum mco_prescaler_t				mco1_prescaler : 3;	  // bit 24-26
	enum mco_prescaler_t				mco2_prescaler : 3;	  // bit 27-29
	enum microcontroller_clock_ouput2_t mco2_source : 2;	  // bit 30: 31

} rcc_cfgr_t;

_Static_assert(sizeof(rcc_cfgr_t) == sizeof(uint32_t), "rcc_cfgr_t : The struct is the wrong size!");

// 4 ============================ RCC clock interrupt register (RCC_CIR)

typedef struct
{
	uint32_t raw;
} rcc_cir_t;

// 5 ============================ RCC AHB1 peripheral reset register (RCC_AHB1RSTR)

typedef struct
{
	uint32_t raw;
} rcc_ahb1rstr_t;

// 6 ============================ RCC AHB2 peripheral reset register (RCC_AHB2RSTR)

typedef struct
{
	uint32_t raw;
} rcc_ahb2rstr_t;

// 7 ============================ RCC APB1 peripheral reset register (RCC_APB1RSTR)

typedef struct
{
	uint32_t raw;
} rcc_apb1rstr_t;

// 8 ============================ RCC APB2 peripheral reset register (RCC_APB2RSTR)

typedef struct
{
	uint32_t raw;
} rcc_apb2rstr_t;

// 9 ============================ RCC AHB1 peripheral clock enable register (RCC_AHB1ENR)

typedef struct
{
	uint32_t gpio_a_enable : 1; // bit 0
	uint32_t gpio_b_enable : 1; // bit 1
	uint32_t gpio_c_enable : 1; // bit 2
	uint32_t gpio_d_enable : 1; // bit 3
	uint32_t gpio_e_enable : 1; // bit 4

	uint32_t _reserved_1 : 2; // bit 5-6
	// would be gpio_f, and gpio_h

	uint32_t gpio_h_enable : 1; // bit 7

	uint32_t _reserved_2 : 4; // bit 8-11

	uint32_t crc_clock_enable : 1; // bit 12

	uint32_t _reserved_3 : 8;		// bit 13-20
	uint32_t dma1_clock_enable : 1; // bit 21
	uint32_t dma2_clock_enable : 1; // bit 22

	uint32_t _reserved_4 : 9; // bit 23-31

} rcc_ahb1enr_t;

_Static_assert(sizeof(rcc_ahb1enr_t) == sizeof(uint32_t), "rcc_ahb1enr_t : The struct is the wrong size!");

// 10 ============================ RCC AHB2 peripheral clock enable register (RCC_AHB2ENR)

typedef struct
{
	uint32_t raw;
} rcc_ahb2enr_t;

// 11 ============================ RCC APB1 peripheral clock enable register (RCC_APB1ENR)

typedef struct
{
	uint32_t timer2_enable : 1; // bit 0
	uint32_t timer3_enable : 1; // bit 1
	uint32_t timer4_enable : 1; // bit 2
	uint32_t timer5_enable : 1; // bit 3

	uint32_t _reserved1 : 7; // bit 4-10

	uint32_t window_watchdog_clock_enable : 1; // bit 11

	uint32_t _reserved2 : 2;  // bit 12-13
	uint32_t spi2_enable : 1; // bit 14
	uint32_t spi3_enable : 1; // bit 15

	uint32_t _reserved3 : 1; // bit 16

	uint32_t usart2_enable : 1; // bit 17

	uint32_t _reserved4 : 3; // bit 18-20

	uint32_t i2c1_enable : 1; // bit 21
	uint32_t i2c2_enable : 1; // bit 22
	uint32_t i2c3_enable : 1; // bit 23

	uint32_t _reserved5 : 4; // bit 24-27

	uint32_t power_interface_clock_enable : 1; // bit 28

	uint32_t _reserved6 : 3; // bit 29-31

} rcc_apb1enr_t;

_Static_assert(sizeof(rcc_apb1enr_t) == sizeof(uint32_t), "rcc_apb1enr_t : The struct is the wrong size!");

// 12 ============================ RCC APB2 peripheral clock enable register (RCC_APB2ENR)

typedef struct
{
	uint32_t timer1_enable : 1; // bit 0

	uint32_t _reserved1 : 3; // bit 1-3

	uint32_t usart1_enable : 1; // bit 4
	uint32_t usart6_enable : 1; // bit 5

	uint32_t _reserved2 : 2; // bit 6-7

	uint32_t adc1_enable : 1; // bit 8

	uint32_t _reserved3 : 2; // bit 9-10

	uint32_t sdio_enable : 1; // bit 11

	uint32_t spi1_enable : 1;					  // bit 12
	uint32_t spi4_enable : 1;					  // bit 13
	uint32_t system_config_controller_enable : 1; // bit 14

	uint32_t _reserved4 : 1; // bit 15

	uint32_t timer9_enable : 1;	 // bit 16
	uint32_t timer10_enable : 1; // bit 17
	uint32_t timer11_enable : 1; // bit 18

	uint32_t _reserved5 : 1;  // bit 19
	uint32_t spi5_enable : 1; // bit 20

	uint32_t _reserved6 : 11; // bit 21-31

} rcc_apb2enr_t;

_Static_assert(sizeof(rcc_apb2enr_t) == sizeof(uint32_t), "rcc_apb2enr_t : The struct is the wrong size!");

// 13 ============================ RCC AHB1 peripheral clock enable in low power mode register (RCC_AHB1LPENR)

typedef struct
{
	uint32_t raw;
} rcc_ahb1lpenr_t;

// 14 ============================ RCC AHB2 peripheral clock enable in low power mode register (RCC_AHB2LPENR)

typedef struct
{
	uint32_t raw;
} rcc_ahb2lpenr_t;

// 15 ============================ RCC APB1 peripheral clock enable in low power mode register (RCC_APB1LPENR)

typedef struct
{
	uint32_t raw;
} rcc_apb1lpenr_t;

// 16 ============================ RCC APB2 peripheral clock enable in low power mode register (RCC_APB2LPENR)

typedef struct
{
	uint32_t raw;
} rcc_apb2lpenr_t;

// 17 ============================ RCC Backup domain control register (RCC_BDCR)

typedef struct
{
	uint32_t raw;
} rcc_bdcr_t;

// 18 ============================ RCC clock control & status register (RCC_CSR)

typedef struct
{
	uint32_t raw;
} rcc_csr_t;

// 19 ============================ RCC spread spectrum clock generation register (RCC_SSCGR)

typedef struct
{
	uint32_t raw;
} rcc_sscgr_t;

// 20 ============================ RCC PLLI2S configuration register (RCC_PLLI2SCFGR)

typedef struct
{
	uint32_t raw;
} rcc_plli2scfgr_t;

// 21 ============================ RCC Dedicated Clocks Configuration Register (RCC_DCKCFGR)

typedef struct
{
	uint32_t raw;
} rcc_dckcfgr_t;

// ================================================= The META Struct ===================

typedef struct
{
	rcc_cr_t	  cr;	   // 0x00
	rcc_pllcfgr_t pllcfgr; // 0x04
	rcc_cfgr_t	  cfgr;	   // 0x08
	rcc_cir_t	  cir;	   // 0x0C

	rcc_ahb1rstr_t ahb1rstr; // 0x10
	rcc_ahb2rstr_t ahb2rstr; // 0x14

	uint32_t _reserved_18; // 0x18
	uint32_t _reserved_1c; // 0x1C

	rcc_apb1rstr_t apb1rstr; // 0x20
	rcc_apb2rstr_t apb2rstr; // 0x24

	uint32_t _reserved_28; // 0x28
	uint32_t _reserved_2c; // 0x2C

	rcc_ahb1enr_t ahb1_enable_register; // 0x30
	rcc_ahb2enr_t ahb2enr;				// 0x34

	uint32_t _reserved_38; // 0x38
	uint32_t _reserved_3c; // 0x3C

	rcc_apb1enr_t apb1enr; // 0x40
	rcc_apb2enr_t apb2enr; // 0x44

	uint32_t _reserved_48; // 0x48
	uint32_t _reserved_4c; // 0x4C

	rcc_ahb1lpenr_t ahb1lpenr; // 0x50
	rcc_ahb2lpenr_t ahb2lpenr; // 0x54

	uint32_t _reserved_58; // 0x58
	uint32_t _reserved_5c; // 0x5C

	rcc_apb1lpenr_t apb1lpenr; // 0x60
	rcc_apb2lpenr_t apb2lpenr; // 0x64

	uint32_t _reserved_68; // 0x68
	uint32_t _reserved_6c; // 0x6C

	rcc_bdcr_t bdcr; // 0x70
	rcc_csr_t  csr;	 // 0x74

	uint32_t _reserved_78; // 0x78
	uint32_t _reserved_7c; // 0x7C

	rcc_sscgr_t		 sscgr;		 // 0x80
	rcc_plli2scfgr_t plli2scfgr; // 0x84

	uint32_t _reserved_88; // 0x88

	rcc_dckcfgr_t dckcfgr; // 0x8C

} rcc_register_t;

_Static_assert(sizeof(rcc_register_t) == 0x90, "rcc_register_t : The struct is the wrong size!");

static const uint32_t RCC_MMIO_ADDRESS_BASE = 0x40023800;

extern volatile rcc_register_t *const rcc;

#include "gpio_types.h"
void enable_gpio_clock(enum GPIO_PORT_LETTER letter);
void configure_rcc_timers();
void enable_timers_rcc();
void switch_to_pll();
