#include "string.h"
#include "string_helper.h"
#include <math.h>
size_t strlen(const char *str)
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

// Returns the length of the string written.
size_t itoa(int value, char *buffer)
{
	size_t pos		= 0;
	int	   negative = 0;

	if (value == 0)
	{
		buffer[pos++] = '0';
		buffer[pos]	  = '\0';
		return pos;
	}

	if (value < 0)
	{
		negative = 1;
		value	 = -value; // careful: INT_MIN may overflow, but for small kernel code often fine
	}

	// Write digits in reverse
	size_t start = pos;
	while (value > 0)
	{
		buffer[pos++] = '0' + (value % 10);
		value /= 10;
	}

	if (negative)
	{
		buffer[pos++] = '-';
	}

	buffer[pos] = '\0';

	// Reverse the string
	size_t end = pos - 1;
	while (start < end)
	{
		char tmp	  = buffer[start];
		buffer[start] = buffer[end];
		buffer[end]	  = tmp;
		start++;
		end--;
	}

	return pos;
}

size_t uitoa(uint32_t value, char *buffer)
{
	size_t pos = 0;

	if (value == 0)
	{
		buffer[pos++] = '0';
		buffer[pos]	  = '\0';
		return pos;
	}

	// Write digits in reverse
	size_t start = pos;
	while (value > 0)
	{
		buffer[pos++] = '0' + (value % 10);
		value /= 10;
	}

	buffer[pos] = '\0';

	// Reverse the string
	size_t end = pos - 1;
	while (start < end)
	{
		char tmp	  = buffer[start];
		buffer[start] = buffer[end];
		buffer[end]	  = tmp;
		start++;
		end--;
	}

	return pos;
}

size_t ftoa_scientific(float value, char *buffer, uint32_t precision)
{
	int32_t exponent = 0;

	// Inspect IEEE-754 binary32 representation.
	typedef struct
	{
		uint32_t mantissa : 23;
		uint32_t exponent : 8;
		uint32_t sign_negative : 1;
	} float_t;
	union
	{
		float_t float_struct;
		float	f;
	} bits = {.f = value};

	bool	 sign_negative = bits.float_struct.sign_negative;
	uint8_t	 exp_bits	   = bits.float_struct.exponent;
	uint32_t mantissa_bits = bits.float_struct.mantissa;

	char *out = buffer;
	if (exp_bits == 0xFF)
	{
		if (mantissa_bits != 0)
		{
			return strwrite(buffer, "nan");
		}
		else
		{
			if (sign_negative)
			{
				return strwrite(buffer, "-inf");
			}
			else
			{
				return strwrite(buffer, "inf");
			}
		}

		if (sign_negative)
		{
			*out++ = '-';
			value  = -value;
		}

		if (value == 0.0f)
		{
			*out++ = '0';
		}
		if (precision > 0)
		{
			*out++ = '.';
			for (uint32_t i = 0; i < precision; ++i)
				*out++ = '0';
		}

		*out++ = 'e';
		*out++ = '+';
		*out++ = '0';
		*out++ = '0';
		*out   = '\0';

		return (size_t)(out - buffer);
	}

	// Normalize: 1 <= value < 10.
	while (value >= 10.0f)
	{
		value /= 10.0f;
		++exponent;
	}

	while (value < 1.0f)
	{
		value *= 10.0f;
		--exponent;
	}

	// Generate the leading digit.
	uint32_t digit = (uint32_t)value;
	*out++		   = (char)('0' + digit);
	value		   = (value - (float)digit) * 10.0f;

	char *fraction_start = out;

	if (precision > 0)
		*out++ = '.';

	// Generate fractional digits.
	for (uint32_t i = 0; i < precision; ++i)
	{
		digit = (uint32_t)value;

		*out++ = (char)('0' + digit);
		value  = (value - (float)digit) * 10.0f;
	}

	// Round using the next digit.
	digit = (uint32_t)value;

	if (digit >= 5)
	{
		char *p = out - 1;

		while (p >= fraction_start)
		{
			if (*p == '.')
			{
				--p;
				continue;
			}

			if (*p < '9')
			{
				++(*p);
				break;
			}

			*p = '0';
			--p;
		}

		// Carry propagated past the leading digit.
		if (p < fraction_start)
		{
			// The leading digit was 9; rounding made it 10.
			// Shift the exponent and retain a leading 1.
			buffer[sign_negative ? 1 : 0] = '1';
			++exponent;
		}
	}

	// Scientific exponent: e+00, e-03, e+38, etc.
	*out++ = 'e';
	*out++ = (exponent < 0) ? '-' : '+';

	uint32_t abs_exponent = (exponent < 0) ? (uint32_t)(-exponent) : (uint32_t)exponent;

	*out++ = (char)('0' + (abs_exponent / 10) % 10);
	*out++ = (char)('0' + abs_exponent % 10);

	*out = '\0';
	return (size_t)(out - buffer);
}

size_t ftoa_normal(float value, char *buffer, uint32_t precision)
{
	uint32_t pos = 0;

	// Handle sign
	if (value < 0.0f)
	{
		buffer[pos++] = '-';
		value		  = -value;
	}

	if (precision > FTOA_MAX_PRECISION)
	{
		precision = FTOA_MAX_PRECISION;
	}

	// Extract integer part
	uint32_t integer_part = (uint32_t)value;

	// Extract fractional part
	float fractional = value - (float)integer_part;

	// Convert integer part
	pos += uitoa(integer_part, buffer + pos);

	// If no requested precision, stop
	if (precision == 0)
	{
		buffer[pos] = '\0';
		return pos;
	}

	// Add decimal point
	buffer[pos++] = '.';

	// Convert fractional digits
	for (uint32_t i = 0; i < precision; i++)
	{
		fractional *= 10.0f;
		uint32_t digit = (uint32_t)fractional;
		buffer[pos++]  = '0' + digit;
		fractional -= digit; // remove the digit we just printed
	}

	buffer[pos] = '\0';
	return pos;
}

size_t ftoa(float value, char *buffer, uint32_t precision)
{
	if (value >= (float)FTOA_MAX_NORMAL_NUMBER)
	{
		return ftoa_scientific(value, buffer, precision);
	}
	return ftoa_normal(value, buffer, precision);
}

static inline int is_digit(char c)
{
	return (c >= '0' && c <= '9');
}

uint8_t char_to_uint(const char c)
{
	if (!is_digit(c))
	{
		return CHAR_TO_UINT_INVALID;
	}

	return c - '0';
}

uint32_t string_to_uint(const char *str)
{
	uint32_t power = 1;
	uint32_t sum   = 0;

	uint8_t str_length = strlen(str);
	str += str_length - 1;

	while (*str)
	{
		uint8_t digit = char_to_uint(*str);
		if (digit == CHAR_TO_UINT_INVALID)
		{
			return -1;
		}
		sum += digit * power;
		power *= 10;
		str--;
	}
	return sum;
}

size_t uint_to_hex(uint32_t value, char *buffer, int uppercase)
{
	const char *digits = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";
	int			i;

	// Handle zero explicitly
	if (value == 0)
	{
		buffer[0] = '0';
		buffer[1] = '\0';
		return 1;
	}

	char temp[9]; // Max 8 hex digits + null terminator
	temp[8] = '\0';

	// Convert each nibble
	for (i = 7; i >= 0; i--)
	{
		uint8_t nibble = value & 0xF;
		temp[i]		   = digits[nibble];
		value >>= 4;
	}

	// Copy skipping leading zeros
	i = 0;
	while (temp[i] == '0')
		i++;
	int j = 0;
	while (temp[i] != '\0')
	{
		buffer[j++] = temp[i++];
	}
	buffer[j] = '\0';

	return (size_t)j;
}

size_t ptr_to_hex(void *ptr, char *buffer, int uppercase)
{
	const char *digits = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";
	uintptr_t	value  = (uintptr_t)ptr; // cast pointer to integer
	int			i;

	// Handle null pointer explicitly
	if (value == 0)
	{
		buffer[0] = '0';
		buffer[1] = '\0';
		return 1;
	}

	char temp[2 * sizeof(void *) + 1]; // enough for pointer in hex + null
	temp[2 * sizeof(void *)] = '\0';

	// Fill from the end
	for (i = 2 * sizeof(void *) - 1; i >= 0; i--)
	{
		temp[i] = digits[value & 0xF];
		value >>= 4;
	}

	// Skip leading zeros
	i = 0;
	while (temp[i] == '0')
		i++;

	int j = 0;
	while (temp[i] != '\0')
	{
		buffer[j++] = temp[i++];
	}
	buffer[j] = '\0';

	return (size_t)j;
}
