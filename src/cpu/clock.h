#pragma once

// RTC : Reset and Clock Control
// RTC: Real Time Clock
// SW: System Clock switch
// Trapeze symbol: Multiplexer (Selector)
// AHB Presc: Advanced High Performance Bus Prescalor. A Prescaler divide a clock and outputs a slower version of it
// HCLK: AHB Clock. The clock that drives everything sitting on ahb bys. The core, memory interface and DMA.
// APB = Advanced Peripheral Bus. Simpler slower bus for peripherals (timers, uart, spi, i2c, etc).
//		| It's clock are PCLK1 (APB1), and PCLK2 APB2)
// 		| APB1 : Low speed, APB2 (Max 50Mhz): High speed (Max 100 Mhz)

// The sysclock can come from the hsi, the hse or the pllclk
// The pll clock can be sourced from the hse or the hsi

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
