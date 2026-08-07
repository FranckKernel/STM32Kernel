.syntax unified
.cpu cortex-m4
.thumb

.global reset_handler
.global vector_table

.section .isr_vector
vector_table:

.word _estack
.word reset_handler


.section .text

reset_handler:

    bl main

hang:
    b hang
