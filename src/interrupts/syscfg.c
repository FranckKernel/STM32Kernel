#include "syscfg.h"

volatile syscfg_t *const syscfg = (volatile syscfg_t *const)SYSCFG_BASE;

void route(uint8_t exti_number, syscfg_pin_letter_t pin_letter, uint8_t pin_number)
{
}
