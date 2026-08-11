ENTRY(_reset)

MEMORY
{
    FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 512K
    RAM   (rwx): ORIGIN = 0x20000000, LENGTH = 128K
}

_estack = ORIGIN(RAM) + LENGTH(RAM);

/* Set minimum size for stack and dynamic memory. */
/* (The linker will generate an error if there is
 * less than this much RAM leftover.) */
/* (16KB) */
_Min_Leftover_RAM = 0x4000;

SECTIONS
{

	.vector_table :
	{
		. = ALIGN(4);
        /* KEEP(*(.isr_vector)) */
		. = ALIGN(4);
	} >FLASH

    .text :
    {
		. = ALIGN(4);
        KEEP(*(.isr_vector))
		*(.text)
		*(.text*)
		. = ALIGN(4);
    } > FLASH

	.rodata :
	{
		. = ALIGN(4);
		*(.rodata)
		*(.rodata*)
		. = ALIGN(4);
	} >FLASH


	.data :
	{
		. = ALIGN(4);
		_sdata = .;
		*(.data)
		*(.data*)
		_edata = .;
		. = ALIGN(4);
	} >RAM AT>FLASH
	_sidata = LOADADDR(.data);

    .bss :
    {
		. = ALIGN(4);
        _sbss = .;
        *(.bss)
        *(.bss*)
        *(COMMON)

		. = ALIGN(4);
        _ebss = .;
    } > RAM

	  /* Space set aside for the application's heap/stack. */
	.dynamic_allocations :
	{
		. = ALIGN(4);
		_end = .;
		/* end is temporary, used for heap memory */
		_ssystem_ram = .;
		. = . + _Min_Leftover_RAM;
		. = ALIGN(4);
		_esystem_ram = .;
	} >RAM
}
