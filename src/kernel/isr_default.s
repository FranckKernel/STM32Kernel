.syntax unified
.cpu cortex-m4
.thumb

.global Default_Handler

.section .text

.thumb_func
.type Default_Handler, %function
Default_Handler:
hang:
    b hang
.size Default_Handler, .-Default_Handler
