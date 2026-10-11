#pragma once
#include <stdint.h>

#define BITCASTOLD(to_type, value)                                                                                                         \
	({                                                                                                                                     \
		union                                                                                                                              \
		{                                                                                                                                  \
			__typeof__(value) from;                                                                                                        \
			to_type			  to;                                                                                                          \
		} _bitcast = {.from = (value)};                                                                                                    \
		_bitcast.to;                                                                                                                       \
	})

#define UINT_OF_SIZE(n) __typeof__(_Generic((char (*)[n])0, char (*)[1]: (uint8_t)0, char (*)[2]: (uint16_t)0, char (*)[4]: (uint32_t)0))

#define BITCAST(to_type, ...)                                                                                                              \
	({                                                                                                                                     \
		_Static_assert(sizeof(to_type) == sizeof((__VA_ARGS__)), "BITCAST: size mismatch");                                                \
		union                                                                                                                              \
		{                                                                                                                                  \
			__typeof__((__VA_ARGS__)) from;                                                                                                \
			to_type					  to;                                                                                                  \
		} _bitcast = {.from = (__VA_ARGS__)};                                                                                              \
		_bitcast.to;                                                                                                                       \
	})

#define WRITE32(addr, value)                                                                                                               \
	({                                                                                                                                     \
		_Static_assert(                                                                                                                    \
			__builtin_types_compatible_p(__typeof__(*(addr)), __typeof__(value)), "WRITE32: input type must match destination type");      \
		*(volatile uint32_t *)(uintptr_t)(addr) = BITCAST(uint32_t, value);                                                                \
	})

#define WRITE16(addr, value)                                                                                                               \
	({                                                                                                                                     \
		_Static_assert(                                                                                                                    \
			__builtin_types_compatible_p(__typeof__(*(addr)), __typeof__(value)), "WRITE16: input type must match destination type");      \
		*(volatile uint16_t *)(uintptr_t)(addr) = BITCAST(uint16_t, value);                                                                \
	})

#define WRITE8(addr, value)                                                                                                                \
	({                                                                                                                                     \
		_Static_assert(                                                                                                                    \
			__builtin_types_compatible_p(__typeof__(*(addr)), __typeof__(value)), "WRITE8: input type must match destination type");       \
		*(volatile uint8_t *)(uintptr_t)(addr) = BITCAST(uint8_t, value);                                                                  \
	})

#define READ32(addr)                                                                                                                       \
	({                                                                                                                                     \
		uint32_t _raw = *(volatile uint32_t *)(uintptr_t)(addr);                                                                           \
		BITCAST(__typeof__(*(addr)), _raw);                                                                                                \
	})

#define READ16(addr)                                                                                                                       \
	({                                                                                                                                     \
		uint16_t _raw = *(volatile uint16_t *)(uintptr_t)(addr);                                                                           \
		BITCAST(__typeof__(*(addr)), _raw);                                                                                                \
	})

#define READ8(addr)                                                                                                                        \
	({                                                                                                                                     \
		uint8_t _raw = *(volatile uint8_t *)(uintptr_t)(addr);                                                                             \
		BITCAST(__typeof__(*(addr)), _raw);                                                                                                \
	})

#define FORCE_WRITE32(addr, value) (*(volatile uint32_t *)(uintptr_t)(addr) = BITCAST(uint32_t, value))

#define FORCE_WRITE16(addr, value) (*(volatile uint16_t *)(uintptr_t)(addr) = BITCAST(uint16_t, value))

#define FORCE_WRITE8(addr, value) (*(volatile uint8_t *)(uintptr_t)(addr) = BITCAST(uint8_t, value))
