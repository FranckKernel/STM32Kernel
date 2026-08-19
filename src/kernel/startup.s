.syntax unified
.cpu cortex-m4
.thumb

.global _reset

.section .text

.thumb_func
.type _reset, %function
_reset:
    bl main

hang:
    b hang

.size _reset, .-_reset
