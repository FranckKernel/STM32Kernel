#pragma once
#include <stdint.h>

static const uint32_t clock_frequency = 96 * 1000 * 1000;
static const uint32_t timer_prescaler = clock_frequency / 1000'000;
