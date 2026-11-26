/* mock_hal.c -- HAL giả lập cho test PC */
#include "main.h"

GPIO_TypeDef mock_ports[3] = { {0}, {1}, {2} };
static uint16_t in_lvl[3]  = { 0xFFFF, 0xFFFF, 0xFFFF };  /* mặc định mức 1 (nút nhả, kéo lên) */
static uint16_t out_lvl[3] = { 0, 0, 0 };

GPIO_PinState HAL_GPIO_ReadPin(GPIO_TypeDef *port, uint16_t pin)
{
    return (in_lvl[port->id] & pin) ? GPIO_PIN_SET : GPIO_PIN_RESET;
}

void HAL_GPIO_WritePin(GPIO_TypeDef *port, uint16_t pin, GPIO_PinState s)
{
    if (s) out_lvl[port->id] |= pin; else out_lvl[port->id] &= (uint16_t)~pin;
}

void mock_set_input(GPIO_TypeDef *port, uint16_t pin, int level)
{
    if (level) in_lvl[port->id] |= pin; else in_lvl[port->id] &= (uint16_t)~pin;
}

int mock_get_output(GPIO_TypeDef *port, uint16_t pin)
{
    return (out_lvl[port->id] & pin) ? 1 : 0;
}
