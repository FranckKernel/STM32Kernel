#include "clock.h"
#include "gpio_types.h"

volatile rcc_register_t *const rcc = (volatile rcc_register_t *const)RCC_MMIO_ADDRESS_BASE;

void enable_gpio_clock(enum GPIO_PORT_LETTER letter)
{
	volatile rcc_ahb1enr_t *const enable_register	  = &rcc->ahb1_enable_register;
	volatile uint32_t *const	  enable_register_raw = (volatile uint32_t *const)enable_register;
	*enable_register_raw |= (1 << letter);
	// need to use the raw format because of runtime changes.
	// can't use an array of u(<8) and then a enum name like in zig

	if (false)
	{
		// known example that works
		enable_register->gpio_a_enable = true;
		enable_register->gpio_b_enable = true;
		// In zig, we could have a gpio_enable array. Where you do rcc->rcc_ahb1_enable_register.gpio_enable[.a] = 1;
	}
}

void switch_to_pll()
{

	// configure pll
	/*
	WLOG pll src = hsi => src freq = hsi freq
	restraints:
	1: hsi/m £ {1, 2 Mhz}
	2: (hsi/m) *N £ {100, 432 Mhz}
	3: (hsi*N)/(m*p) <= 100 Mhz
	4: (hsi*N)/(m*q) = 48 Mhz for usb otg fs, and <= 48 for sdio clock

	5: M != 0 or 1
	6: 50 <= N <= 432

	(hsi / m) * N    /   p  = SYS CLK
	(hsi / m) * N    /   q  = 48 MHZ
	( 16 / 8 ) * 96  /   2   = 2 * 48 = 96
	16 / 8     * 96  / 5 =  2 * 96 / 4  =


	hsi / m = 2
	2 * N /q = 48
	2 * N/p = max possible
	p = min possible = 2
	N = max possible. PLL Clock = N (since 2/2 = 1 cancels out)

	N / q = 48
	N = 48, q = 1,
	N = 96 , q = 2, clock = 96 MHz
	N = 96 , q = 4, clock = 48 MHz

	HSI = 16 MHz
	M = 8
	N = 96
	P = 2
	Q = 4


	*/
	// We don't turn hsi off, it's the pll source. No need to turn hse off, it's already off

	// 1. flash wait states first (FLASH_ACR LATENCY, prefetch, caches). See ch. 3.

	// 2. PLL config, PLL is still off. HSI stays on (it is the PLL source).
	rcc_pllcfgr_t pll_config   = rcc->pllcfgr;
	pll_config.m_divider	   = 8; // 16 / 8 = 2
	pll_config.n_multiplicator = 96;
	pll_config.p_divisor	   = pllp_2; // (16 / m
	pll_config.src			   = pll_src_hsi;
	pll_config.q_divisor	   = 4;
	rcc->pllcfgr			   = pll_config;

	// 3. prescalers BEFORE the switch (APB1 max is 50 MHz)
	rcc_cfgr_t config	  = rcc->cfgr;
	config.ahb_prescaler  = ahb_prescaler_divide_1;	 // Keep 16 Mhz (Don't make the machine run slower)
	config.apb1_prescaler = apb1_prescaler_divide_2; // forced to divide by 2 so it's lower then 50Mhz in all case
	// then it goes to an automatic *2 (since /2 : 2 != 1), so 2/2 = 1
	// APBx Timer clocks frequency = 96 MHz
	rcc->cfgr = config;

	// 4. PLL on, wait until ready
	rcc_cr_t cr_config = rcc->cr;
	cr_config.pll_on   = 1;
	rcc->cr			   = cr_config;
	// Single read modify write, could be made into one liner
	while (!rcc->cr.pll_ready)
	{
	}

	// 5. select the PLL, wait until the status confirms it
	rcc->cfgr.system_clock_switch = system_clock_pll; // one liner rmw (same effect as above)
	while (rcc->cfgr.system_clock_switch_status != system_clock_status_pll)
	{
	}
}

void enable_timer2_rcc()
{

	rcc->apb1enr.timer2_enable = 1;
}

void enable_timer2_hsi()
{
	rcc->apb1enr.timer2_enable = 1;

	rcc_cfgr_t config		   = rcc->cfgr;
	config.system_clock_switch = system_clock_hsi;		 // 16 Mhz HSI (Useless, since we are already on hsi. Switching from hsi to hsi)
	config.ahb_prescaler	   = ahb_prescaler_divide_1; // Keep 16 Mhz (Don't make the machine run slower)
	config.apb1_prescaler	   = apb1_prescaler_divide_2;
	rcc->cfgr				   = config;
}
