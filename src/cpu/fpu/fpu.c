#include "fpu.h"
#include "macros.h"

void fpu_enable()
{
	coprocessor_access_control_register_t config = {0};
	config.cp10									 = cpn_full_access;
	config.cp11									 = cpn_full_access;
	// No need for a read or write. We know the default value

	WRITE32(fpu_cacr, config);

	__asm volatile("dsb" ::: "memory");
	__asm volatile("isb" ::: "memory");
}
