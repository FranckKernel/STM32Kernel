.syntax unified
.cpu cortex-m4
.thumb

.global _reset
.global vector_table

.section .isr_vector
vector_table:

.word _estack
.word _reset


.section .text

_reset:
	bl main


hang:
    b hang
