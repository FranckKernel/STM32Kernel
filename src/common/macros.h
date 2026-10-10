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

#define WRITE32(addr, ...) (*(volatile uint32_t *)(uintptr_t)(addr) = BITCAST(uint32_t, __VA_ARGS__))
#define WRITE16(addr, ...) (*(volatile uint16_t *)(uintptr_t)(addr) = BITCAST(uint16_t, __VA_ARGS__))
#define WRITE8(addr, ...) (*(volatile uint8_t *)(uintptr_t)(addr) = BITCAST(uint8_t, __VA_ARGS__))

#define READ32(addr, type)                                                                                                                 \
	({                                                                                                                                     \
		uint32_t _raw = *(volatile uint32_t *)(uintptr_t)(addr);                                                                           \
		BITCAST(type, _raw);                                                                                                               \
	})

#define READ16(addr, type)                                                                                                                 \
	({                                                                                                                                     \
		uint16_t _raw = *(volatile uint16_t *)(uintptr_t)(addr);                                                                           \
		BITCAST(type, _raw);                                                                                                               \
	})

#define READ8(addr, type)                                                                                                                  \
	({                                                                                                                                     \
		uint8_t _raw = *(volatile uint8_t *)(uintptr_t)(addr);                                                                             \
		BITCAST(type, _raw);                                                                                                               \
	})
