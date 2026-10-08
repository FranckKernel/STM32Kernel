#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/*
 * Rank  Register     Importance  	Why
 * 1     CR1          ⭐⭐⭐⭐⭐  	Fundamental timer control: enable, counter direction,
 *                                	alignment, etc.

 * 2     PSC          ⭐⭐⭐⭐⭐     Controls timer clock division. Essential for setting
 *                                 timer frequency.

 * 3     ARR          ⭐⭐⭐⭐⭐     Sets the counter period. Essential for periodic
 *                                 interrupts and PWM frequency.

 * 4     SR           ⭐⭐⭐⭐⭐     Contains timer status/interrupt flags. Essential
 *                                 when using interrupts.

 * 5     DIER         ⭐⭐⭐⭐⭐     Enables timer interrupts/DMA requests. Essential for
 *                                 interrupt-driven timers.

 * 6     CNT          ⭐⭐⭐⭐      Current counter value. Important for reading/manually
 *                                 manipulating timer state.

 * 7     EGR          ⭐⭐⭐⭐      Forces timer events, especially useful when
 *                                 initializing/reloading configuration.

 * 8     CCMR1        ⭐⭐⭐⭐      Channel 1/2 configuration; important for PWM and
 *                                 input capture.

 * 9     CCMR2        ⭐⭐⭐⭐      Same as above for channels 3/4.
 * 10    CCER         ⭐⭐⭐⭐      Enables/disables capture/compare channels and
 *                                 controls polarity.

 * 11    CCR1         ⭐⭐⭐⭐      Compare/capture value for channel 1; crucial for PWM.
 * 12    CCR2         ⭐⭐⭐⭐      Same for channel 2.
 * 13    CCR3         ⭐⭐⭐⭐      Same for channel 3.
 * 14    CCR4         ⭐⭐⭐⭐      Same for channel 4.
 * 15    CR2          ⭐⭐⭐       Timer synchronization/trigger/output configuration.
 *                                 Less important initially.

 * 16    SMCR         ⭐⭐⭐       Slave mode/external clock/synchronization. Important
 *                                 for advanced timer use.

 * 17    DCR          ⭐⭐        DMA burst configuration. Ignore initially unless
 *                                 using DMA.

 * 18    DMAR         ⭐⭐        DMA register access. Same: ignore until DMA.

 * 19    TIM2_OR      ⭐          Timer 2 option routing. Very specialized.
 * 20    TIM5_OR      ⭐          Timer 5 option routing. Very specialized.
 */
static const uint32_t TIMERS_MMIO_ADDRESS_BASE = 0x40000000;
static const uint32_t TIMERS_MMIO_NUMBER_DIFF  = 0x400;

static const uint32_t TIMER2_MMIO_BASE = TIMERS_MMIO_ADDRESS_BASE;
static const uint32_t TIMER3_MMIO_BASE = TIMER2_MMIO_BASE + 1 * 0x0400;
static const uint32_t TIMER4_MMIO_BASE = TIMER2_MMIO_BASE + 2 * 0x0400;
static const uint32_t TIMER5_MMIO_BASE = TIMER2_MMIO_BASE + 3 * 0x0400;

// Control Register 1 (CR1) ===========================

enum cr1_direction
{
	upcounter	= 0b0,
	downcounter = 0b1,
};

enum cr1_center_aligned_mode_selection
{
	edge_aligned_mode				 = 0b00,
	center_aligned_mode_compare_down = 0b01, // These are used for output compare
	center_aligned_mode_compare_up	 = 0b10, // They bounce up and down between 0 and arr
	center_aligned_mode_compare_both = 0b11,
	// Can only switch mode while counter is disabled
};

enum cr1_clock_division
{
	ckd_times_1 = 0b00,
	ckd_times_2 = 0b01,
	ckd_times_4 = 0b10,
};

typedef struct
{
	uint32_t counter_enable : 1;		// bit 0 (rw)
	uint32_t update_disable : 1;		// bit 1 (rw)
	uint32_t update_request_source : 1; // bit  2 ( rw)
	/*
	   0: counter under/overflow, setting the UG bit or  update generation through slave mode controller ... can generate an update
	   interrupt or DMA request

	   1: Only counter under/overflow generates an interrupt or DMA request
	*/
	uint32_t							   one_pulse_mode : 1;		// bit 3 (rw), set so it's one shot mode, else counter loop
	enum cr1_direction					   direction : 1;			// bit 4 (rw). 0 = upcounter, 1 = downcounter
	enum cr1_center_aligned_mode_selection center_aligned_mode : 2; // bit (5-6) (rw)
	uint32_t schedule_arr_change_not_right_now : 1; // bit 7 (rw) {When you change arr, change it right now, or at the next update event}
	enum cr1_clock_division clock_division : 2;		// bit 8-9 (rw). Keep it at 0b00, or x1 for me

	uint32_t _reserved : 22; // bit 10 - 31
} timer_cr1_t;
_Static_assert(sizeof(timer_cr1_t) == sizeof(uint32_t), "timer_cr1_t is not the proper size of 32 bit!");

// The meta struct
typedef struct
{
	timer_cr1_t cr1;		  // Control register 1
	uint32_t	cr2;		  // Control register 2
	uint32_t	smcr;		  // Slave mode control
	uint32_t	dier;		  // Dma / interrupt enable
	uint32_t	sr;			  // Status register
	uint32_t	egr;		  // Event generation
	uint16_t	ccmr1_output; // Capture/compare mode 1
	uint16_t	ccmr1_input;  // Capture/compare mode 1
	uint16_t	ccmr2_output; // Capture/compare mode 2
	uint16_t	ccmr2_input;  // Capture/compare mode 2
	uint32_t	ccer;		  // Capture/compare enable
	uint32_t	cnt;		  // Counter
	uint32_t	psc;		  // Prescaler
	uint32_t	arr;		  // Auto-reload. (Simple 32 bit value. 16 bit for tim3 and tim4)
	uint32_t	reserved0;
	uint32_t	ccr1; // Capture/compare 1
	uint32_t	ccr2; // Capture/compare 2
	uint32_t	ccr3; // Capture/compare 3
	uint32_t	ccr4; // Capture/compare 4
	uint32_t	reserved1;
	uint32_t	dcr;  // Dma control
	uint32_t	dmar; // Dma address
	uint32_t	tim2_or;
	uint32_t	tim5_or;
} timer_registers_t;

extern volatile timer_registers_t *const timers[];

#include "timer_types.h"
// functions using timer_types.h

void configure_timer2();
