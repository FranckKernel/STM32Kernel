#pragma once
#include "static_assert.h"
#include <stdint.h>

#define UNQUAL_TYPEOF_DEREF(addr) __typeof_unqual__(*(addr))
// #define UNQUAL_TYPEOF_DEREF(addr) __typeof__(((void)0, *(addr)))

#define BITCAST(to_type, ...)                                                                                                              \
	({                                                                                                                                     \
		STATIC_ASSERT(sizeof(to_type) == sizeof((__VA_ARGS__)), "BITCAST: size mismatch");                                                 \
		union                                                                                                                              \
		{                                                                                                                                  \
			__typeof_unqual__((__VA_ARGS__)) from;                                                                                         \
			__typeof_unqual__(to_type) to;                                                                                                 \
		} _bitcast = {.from = (__VA_ARGS__)};                                                                                              \
		_bitcast.to;                                                                                                                       \
	})

#define UINT_OF_SIZE(n) __typeof__(_Generic((char (*)[n])0, char (*)[1]: (uint8_t)0, char (*)[2]: (uint16_t)0, char (*)[4]: (uint32_t)0))

#define WRITE32(addr, value)                                                                                                               \
	({                                                                                                                                     \
		STATIC_ASSERT(                                                                                                                     \
			__builtin_types_compatible_p(__typeof__(*(addr)), __typeof__(value)), "WRITE32: input type must match destination type");      \
		*(volatile uint32_t *)(uintptr_t)(addr) = BITCAST(uint32_t, value);                                                                \
	})

#define WRITE16(addr, value)                                                                                                               \
	({                                                                                                                                     \
		STATIC_ASSERT(                                                                                                                     \
			__builtin_types_compatible_p(__typeof__(*(addr)), __typeof__(value)), "WRITE16: input type must match destination type");      \
		*(volatile uint16_t *)(uintptr_t)(addr) = BITCAST(uint16_t, value);                                                                \
	})

#define WRITE8(addr, value)                                                                                                                \
	({                                                                                                                                     \
		STATIC_ASSERT(                                                                                                                     \
			__builtin_types_compatible_p(__typeof__(*(addr)), __typeof__(value)), "WRITE8: input type must match destination type");       \
		*(volatile uint8_t *)(uintptr_t)(addr) = BITCAST(uint8_t, value);                                                                  \
	})

#define READ32(addr)                                                                                                                       \
	({                                                                                                                                     \
		STATIC_ASSERT(sizeof(*(addr)) == sizeof(uint32_t), "READ32: size mismatch");                                                       \
		uint32_t _raw = *(volatile uint32_t *)(uintptr_t)(addr);                                                                           \
		BITCAST(UNQUAL_TYPEOF_DEREF(addr), _raw);                                                                                          \
	})

#define READ16(addr)                                                                                                                       \
	({                                                                                                                                     \
		STATIC_ASSERT(sizeof(*(addr)) == sizeof(uint16_t), "READ16: size mismatch");                                                       \
		uint16_t _raw = *(volatile uint16_t *)(uintptr_t)(addr);                                                                           \
		BITCAST(UNQUAL_TYPEOF_DEREF(addr), _raw);                                                                                          \
	})

#define READ8(addr)                                                                                                                        \
	({                                                                                                                                     \
		STATIC_ASSERT(sizeof(*(addr)) == sizeof(uint8_t), "READ8: size mismatch");                                                         \
		uint8_t _raw = *(volatile uint8_t *)(uintptr_t)(addr);                                                                             \
		BITCAST(UNQUAL_TYPEOF_DEREF(addr), _raw);                                                                                          \
	})

#define FORCE_WRITE32(addr, value) (*(volatile uint32_t *)(uintptr_t)(addr) = BITCAST(uint32_t, value))

#define FORCE_WRITE16(addr, value) (*(volatile uint16_t *)(uintptr_t)(addr) = BITCAST(uint16_t, value))

#define FORCE_WRITE8(addr, value) (*(volatile uint8_t *)(uintptr_t)(addr) = BITCAST(uint8_t, value))
