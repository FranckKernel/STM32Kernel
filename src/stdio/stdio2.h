#pragma once
#include "ppnargs.h"
#include "printf_check.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

	void kprintf_argc(const uint32_t argc, const char *fmt, ...);
	void kprintf2_argc_check(uint32_t argc, const char *fmt, ...);
	void vkprintf(const char *fmt, va_list args);
	void kprintf2(const char *fmt, ...);

	// #define USE_OLD_KPRINTF_ARGC
	// #define CHECK_ARGC

	enum PRINTF_FMT_OPTION
	{
		FMT_OPTION_NONE				 = 0,
		FMT_OPTION_NOT_IMPLEMENTED	 = 1,
		FMT_OPTION_COULD_NOT_CONVERT = 2,
		FMT_OPTION_PAD				 = ':',
		FMT_OPTION_PRECISION		 = '.',
		FMT_OPTION_STRLEN			 = ';'
	};

	enum PRINTF_TYPE
	{
		PRINTF_TAG_INVALID	= 0,
		PRINTF_TAG_CHAR		= 'c',
		PRINTF_TAG_STRING	= 's',
		PRINTF_TAG_INT		= 'd',
		PRINTF_TAG_UINT32_T = 'u',
		PRINTF_TAG_FLOAT	= 'f',
		PRINTF_TAG_HEX		= 'h',
		PRINTF_TAG_BINARY	= 'b',
	};

	struct PRINTF_FIELD_PROPERTIES
	{
		enum PRINTF_TYPE	   type;
		enum PRINTF_FMT_OPTION option;
		uint8_t				   option_num;
		uint8_t				   len;
		uint16_t			   pos;
		/*
		"%s:056"
		The option num is uint8_t 56;
		the len is 7
		the pos is the offset of % in the bigger array.
		it's a string
		and it uses padding
		*/
	};

	// Function to print a string to your terminal

	void test_printf2(void);
	void test_assert(void);

#ifdef __cplusplus
}
#endif
