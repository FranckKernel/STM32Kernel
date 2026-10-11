#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

	size_t strlen(const char *str);
	size_t itoa(int value, char *buffer);
	size_t uitoa(uint32_t value, char *buffer);
	size_t ftoa(float value, char *buffer, uint32_t precision);

	size_t uint_to_hex(uint32_t value, char *buffer, int uppercase);
	size_t ptr_to_hex(void *ptr, char *buffer, int uppercase);

#define IOTA_BUFFER_SIZE_MAX 12
#define UITO_BUFFER_SIZE_MAX 11

#define FLOAT32_MANTISSA_BIT 23
#define FTOA_MAX_NORMAL_NUMBER                                                                                                             \
	((2 << FLOAT32_MANTISSA_BIT) - 1) // 16.7 Million. That is the max number floats can represent without err > 1. After skip whole number
// Removing 1 to be safe and still be in the range where step size < 1
#define LOG_10_16MILLION_ROUNDED_UP 8

#define FTOA_MAX_PRECISION 8
#define FTOA_MAX_WHOLE_NUMBER_LENGTH (LOG_10_16MILLION_ROUNDED_UP)
	// abvc.xyz\0  The dot takes 1 character, the \0 takes another. 1+1 = 2

#define FTOA_BUFFER_SIZE_MAX (FTOA_BUFFER_SIZE_MAX + 2 + FTAO_MAX_PRECISION)

#define CHAR_TO_UINT_INVALID 11

	uint32_t string_to_uint(const char *str);

#ifdef __cplusplus
}
#endif
