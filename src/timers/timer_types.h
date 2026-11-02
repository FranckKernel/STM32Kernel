#pragma once

#include <stdint.h>
enum TIMER_NUMBER
{
	TIMER2 = 0,
	TIMER3,
	TIMER4,
	TIMER5,

};

enum TIMER_CHANNEL
{
	ch1 = 1,
	ch2 = 2,
	ch3 = 3,
	ch4 = 4,
};

struct TIMMER_CHANNEL_ARRAY {
	enum TIMER_CHANNEL channels[4];
	uint8_t channel_count;
};
