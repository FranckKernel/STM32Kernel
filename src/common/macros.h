#define BITCAST(to_type, value)                                                                                                            \
	({                                                                                                                                     \
		union                                                                                                                              \
		{                                                                                                                                  \
			__typeof__(value) from;                                                                                                        \
			to_type			  to;                                                                                                          \
		} _bitcast = {.from = (value)};                                                                                                    \
		_bitcast.to;                                                                                                                       \
	})
