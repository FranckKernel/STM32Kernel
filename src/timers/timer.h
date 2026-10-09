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

// ================== PSC (TIMx Prescaler)
typedef struct
{
	uint16_t prescaler_value; // (rw), default = 0
	// The counter clock frequency = f_ck_psc / (prescaler_value +1 )
	uint16_t reserved; // (r only), default = 0

} timer_prescaler_t;

// ARR (TIMER x Auto-reload Register)

typedef struct
{
	union
	{
		struct
		{
			uint16_t arr16; // (rw), default = 0xFFFF
			uint16_t reserved;
		};
		uint32_t arr32; // (rw), default = 0xFFFF FFFF
	};
} timer_autoreload_t;
_Static_assert(sizeof(timer_autoreload_t) == sizeof(uint32_t), "timer_autoreload_t is not the proper size of 32 bit!");

// ============================= SR (TIMER x Status Register)

typedef struct
{
	// rc_w0: Read or clear by writting 0. Hardware write 1
	uint16_t update_interrupt_pending : 1; // bit 0
	uint16_t capture_compared_1 : 1;	   // bit 1 (rc_w0), default 0
	uint16_t capture_compared_2 : 1;	   // bit 2 (rc_w0), default 0
	uint16_t capture_compared_3 : 1;	   // bit 3 (rc_w0), default 0
	uint16_t capture_compared_4 : 1;	   // bit 4 (rc_w0), default 0
	uint16_t reserved1 : 1;				   // bit 5, (r only), default 0
	uint16_t trigger_interrupt_flag : 1;   // bit 6, (rc_w0)
	uint16_t _reserved2 : 2;			   // bit 7-8, (r only), default 0

	uint16_t capture_compared_1_overcapture : 1; // bit 9 (rc_w0), default 0
	uint16_t capture_compared_2_overcapture : 1; // bit 10 (rc_w0), default 0
	uint16_t capture_compared_3_overcapture : 1; // bit 11 (rc_w0), default 0
	uint16_t capture_compared_4_overcapture : 1; // bit 12 (rc_w0), default 0

	uint16_t _reserved3 : 3; // bit 13-15, (r only), default 0

	uint16_t reserved;

} timer_status_t;
_Static_assert(sizeof(timer_status_t) == sizeof(uint32_t), "timer_status_t is not the proper size of 32 bit!");

// ============================= DIER (TIMER x Interrupt Enable Register)

typedef struct
{

	uint16_t update_interrupt_enable : 1;			 // bit 0, (rw), default 0
	uint16_t capture_compare_1_interrupt_enable : 1; // bit 1, (rw), default 0
	uint16_t capture_compare_2_interrupt_enable : 1; // bit 2, (rw), default 0
	uint16_t capture_compare_3_interrupt_enable : 1; // bit 3, (rw), default 0
	uint16_t capture_compare_4_interrupt_enable : 1; // bit 4, (rw), default 0
	uint16_t _reserved1 : 1;						 // bit 5, (r only), default 0

	uint16_t trigger_interrupt_enable : 1; // bit 6, (rw), default 0
	uint16_t _reserved2 : 1;			   // bit 7, (r only), default 0

	uint16_t update_dma_request_enable : 1;			   // bit 8, (rw), default 0
	uint16_t capture_compare_1_dma_request_enable : 1; // bit 9, (rw), default 0
	uint16_t capture_compare_2_dma_request_enable : 1; // bit 10, (rw), default 0
	uint16_t capture_compare_3_dma_request_enable : 1; // bit 11, (rw), default 1
	uint16_t capture_compare_4_dma_request_enable : 1; // bit 12, (rw), default 0
	uint16_t _reserved3 : 1;						   // bit 13, (r only), default 0

	uint16_t trigger_dma_request_enable : 1; // bit 14, (rw), default 0
	uint16_t _reserved4 : 1;				 // bit 15, (r only), default 0

	uint16_t reserved;

} timer_interrupt_enable_t;
_Static_assert(sizeof(timer_interrupt_enable_t) == sizeof(uint32_t), "timer_interrupt_enable_t is not the proper size of 32 bit!");

// ============================= CNT (TIMER x Interrupt Counter)
typedef struct
{
	union
	{
		struct
		{
			uint16_t count16; // (rw), default = 0xFFFF
			uint16_t reserved;
		};
		uint32_t count32; // (rw), default = 0xFFFF FFFF
	};
} timer_counter_t;
_Static_assert(sizeof(timer_counter_t) == sizeof(uint32_t), "timer_counter_t is not the proper size of 32 bit!");

// ============================= EGR (TIMER x Event Generation Register)
typedef struct
{
	// Write only struct
	uint16_t update_generation : 1; // bit 0 (w only), default 0. Automatically cleared by hardware. Can be set by software.  Reinitialise
									// the counter and generate an update of the registers
	uint16_t capture_compare_1_generation
		: 1; // bit 1 (w only), default 0 Set by software to generate an event. Automatically cleared by hardware.
	// If used as output, generate interrupt or DMA request if enable.
	// If used as input, value of the counter is captured in CCR1 register
	uint16_t capture_compare_2_generation : 1; // bit 2 (w only), default 0
	uint16_t capture_compare_3_generation : 1; // bit 3 (w only), default 0
	uint16_t capture_compare_4_generation : 1; // bit 4 (w only), default 0

	uint16_t _reserved1 : 1;		 // bit 5, (r only), default 0
	uint16_t trigger_generation : 1; // bit 6 (w only), default 0. (related to trigger interrupt flag. )
	// Set by software, auto cleared by hardware. Geneate a interrupt / dma transfer if TIF is set

	uint16_t _reserved2 : 9; // bit 7-15, (r only), default 0
	uint16_t reserved;
} timer_event_generation_t;
_Static_assert(sizeof(timer_event_generation_t) == sizeof(uint32_t), "timer_event_generation_t is not the proper size of 32 bit!");

// ============================= EGR (TIMER x Event Generation Register)
enum cc1s_values_1
{
	// capture compare channel 1
	cc1_output	  = 0b00,
	cc1_input_ti1 = 0b01,
	cc1_input_ti2 = 0b10,
	cc1_input_trc = 0b11, // works only if internal trigger input selected through TS bit (SMCR register)
};

enum cc2s_values_1
{
	// capture compare channel 1
	cc2_output	  = 0b00,
	cc2_input_ti2 = 0b01,
	cc2_input_ti1 = 0b10,
	cc2_input_trc = 0b11, // works only if internal trigger input selected through TS bit (SMCR register)
};

enum ocm_values
{
	ocm_frozen							= 0b000,
	ocm_active_on_match					= 0b001,
	ocm_inactive_on_match				= 0b010,
	ocm_toggle							= 0b011,
	ocm_force_inactive_level			= 0b100,
	ocm_force_active_level				= 0b101,
	ocm_pwm_mode_active_ccr1_over_arr	= 0b110, // The goto mode. PWM Mode 1
	ocm_pwm_mode_inactive_ccr1_over_arr = 0b110,
};

typedef struct __attribute__((packed))
{
	// This whole thing is rw
	enum cc1s_values_1 cc1_selection : 2; // bit 0-1, rw (only writable when cc1e = 0 in ccer). Must be 0
	uint16_t		   output_compare1_fast_enable : 1;
	uint16_t		   output_compare1_preload_enable : 1;
	enum ocm_values	   output_compare1_mode : 3;		 // bit 4-6. (rw)
	uint16_t		   output_compare1_clear_enable : 1; // bit 7, (rw), default 0,
	// if 1 OC1Ref is cleared when ETRF input high, else not affected

	enum cc2s_values_1 cc2_selection : 2;				   // bit 8-9, rw
	uint16_t		   output_compare2_fast_enable : 1;	   // bit 10
	uint16_t		   output_compare2_preload_enable : 1; // bit 11
	enum ocm_values	   output_compare2_mode : 3;		   // bit 12-14. (rw)
	uint16_t		   output_compare2_clear_enable : 1;   // bit 15, (rw), default 0

} timer_ccmr1_output_t;

_Static_assert(sizeof(timer_ccmr1_output_t) == sizeof(uint16_t), "timer_ccr1_input_t is not the proper size of 16 bit!");

enum input_capture_prescalers_values
{
	icp_1 = 0b00, // no prescaler. Capture is done each time an edge is detected on the capture input
	icp_2 = 0b01, // capture input is done once every 2 events
	icp_4 = 0b10, // capture input is done once every 4 events
	icp_8 = 0b11, // capture input is done once every 8 events

};

enum input_capture_filter_values
{
	icf_no_filter = 0b0000, /* f_DTS,        N=none */
	icf_ck_int_n2 = 0b0001, /* f_CK_INT,     N=2    */
	icf_ck_int_n4 = 0b0010, /* f_CK_INT,     N=4    */
	icf_ck_int_n8 = 0b0011, /* f_CK_INT,     N=8    */
	icf_dts_2_n6  = 0b0100, /* f_DTS/2,      N=6    */
	icf_dts_2_n8  = 0b0101, /* f_DTS/2,      N=8    */
	icf_dts_4_n6  = 0b0110, /* f_DTS/4,      N=6    */
	icf_dts_4_n8  = 0b0111, /* f_DTS/4,      N=8    */
	icf_dts_8_n6  = 0b1000, /* f_DTS/8,      N=6    */
	icf_dts_8_n8  = 0b1001, /* f_DTS/8,      N=8    */
	icf_dts_16_n5 = 0b1010, /* f_DTS/16,     N=5    */
	icf_dts_16_n6 = 0b1011, /* f_DTS/16,     N=6    */
	icf_dts_16_n8 = 0b1100, /* f_DTS/16,     N=8    */
	icf_dts_32_n5 = 0b1101, /* f_DTS/32,     N=5    */
	icf_dts_32_n6 = 0b1110, /* f_DTS/32,     N=6    */
	icf_dts_32_n8 = 0b1111, /* f_DTS/32,     N=8 */

};

typedef struct __attribute__((packed))
{
	enum cc1s_values_1					 cc1_selection : 2;			   // bit 0-1, rw (only writable when cc1e = 0 in ccer). Must be 0
	enum input_capture_prescalers_values input_capture1_prescaler : 2; // bit 2-3
	enum input_capture_filter_values	 input_capture1_filter : 4;	   // bit 4-7

	enum cc2s_values_1					 cc2_selection : 2; // bit 8-9, rw
	enum input_capture_prescalers_values input_capture2_prescaler : 2;
	enum input_capture_filter_values	 input_capture2_filter : 4; // bit 12-15

} timer_ccr1_input_t;

_Static_assert(sizeof(timer_ccr1_input_t) == sizeof(uint16_t), "timer_event_generation_t is not the proper size of 16 bit!");

enum cc3s_values_2
{
	// capture compare channel 1
	cc3_output	  = 0b00,
	cc3_input_ti3 = 0b01,
	cc3_input_ti4 = 0b10,
	cc3_input_trc = 0b11, // works only if internal trigger input selected through TS bit (SMCR register)
};

enum cc4s_values_2
{
	// capture compare channel 1
	cc4_output	  = 0b00,
	cc4_input_ti4 = 0b01,
	cc4_input_ti3 = 0b10,
	cc4_input_trc = 0b11, // works only if internal trigger input selected through TS bit (SMCR register)
};

typedef struct __attribute__((packed))
{
	// This whole thing is rw
	enum cc3s_values_2 cc3_selection : 2; // bit 0-1, rw (only writable when cc1e = 0 in ccer). Must be 0
	uint16_t		   output_compare3_fast_enable : 1;
	uint16_t		   output_compare3_preload_enable : 1;
	enum ocm_values	   output_compare3_mode : 3;		 // bit 4-6. (rw)
	uint16_t		   output_compare3_clear_enable : 1; // bit 7, (rw),
	// default 0, if 1 OC1Ref is cleared when ETRF input high, else not affected

	enum cc4s_values_2 cc4_selection : 2;				   // bit 8-9, rw
	uint16_t		   output_compare4_fast_enable : 1;	   // bit 10
	uint16_t		   output_compare4_preload_enable : 1; // bit 11
	enum ocm_values	   output_compare4_mode : 3;		   // bit 12-14. (rw)
	uint16_t		   output_compare4_clear_enable : 1;   // bit 15, (rw), default 0

} timer_ccmr2_output_t;

_Static_assert(sizeof(timer_ccmr2_output_t) == sizeof(uint16_t), "timer_ccr1_input_t is not the proper size of 16 bit!");

typedef struct __attribute__((packed))
{
	enum cc1s_values_1					 cc3_selection : 2;			   // bit 0-1, rw (only writable when cc1e = 0 in ccer). Must be 0
	enum input_capture_prescalers_values input_capture3_prescaler : 2; // bit 2-3
	enum input_capture_filter_values	 input_capture3_filter : 4;	   // bit 4-7

	enum cc2s_values_1					 cc4_selection : 2; // bit 8-9, rw
	enum input_capture_prescalers_values input_capture4_prescaler : 2;
	enum input_capture_filter_values	 input_capture4_filter : 4; // bit 12-15

} timer_ccr2_input_t;
_Static_assert(sizeof(timer_ccr2_input_t) == sizeof(uint16_t), "timer_event_generation_t is not the proper size of 16 bit!");

// ============================================================== THE META STRUCT =====================
typedef struct
{
	timer_cr1_t				 cr1;		   // Control register 1
	uint32_t				 cr2;		   // Control register 2
	uint32_t				 smcr;		   // Slave mode control
	timer_interrupt_enable_t dier;		   // Dma / interrupt enable
	uint32_t				 sr;		   // Status register
	timer_event_generation_t egr;		   // Event generation
	timer_ccmr1_output_t	 ccmr1_output; // Capture/compare mode 1
	timer_ccr1_input_t		 ccmr1_input;  // Capture/compare mode 1
	timer_ccmr1_output_t	 ccmr2_output; // Capture/compare mode 2
	timer_ccr2_input_t		 ccmr2_input;  // Capture/compare mode 2
	uint32_t				 ccer;		   // Capture/compare enable
	timer_counter_t			 cnt;		   // Counter
	timer_prescaler_t		 psc;		   // Prescaler
	timer_autoreload_t		 arr;		   // Auto-reload. (Simple 32 bit value. 16 bit for tim3 and tim4)
	uint32_t				 reserved0;
	uint32_t				 ccr1; // Capture/compare 1
	uint32_t				 ccr2; // Capture/compare 2
	uint32_t				 ccr3; // Capture/compare 3
	uint32_t				 ccr4; // Capture/compare 4
	uint32_t				 reserved1;
	uint32_t				 dcr;  // Dma control
	uint32_t				 dmar; // Dma address
	uint32_t				 tim2_or;
	uint32_t				 tim5_or;
} timer_registers_t;

extern volatile timer_registers_t *const timers[];

#include "timer_types.h"
// functions using timer_types.h

void configure_timer2();
