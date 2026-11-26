/*
 * main.h GIẢ LẬP -- chỉ dùng khi test trên PC bằng gcc.
 * Thay thế HAL của STM32 bằng mảng biến, để chạy được button.c, traffic_light.c
 * trên máy tính mà không cần mạch.
 */
#ifndef MAIN_H_MOCK
#define MAIN_H_MOCK
#include <stdint.h>

typedef struct { int id; } GPIO_TypeDef;
typedef enum { GPIO_PIN_RESET = 0, GPIO_PIN_SET = 1 } GPIO_PinState;

extern GPIO_TypeDef mock_ports[3];
#define GPIOA (&mock_ports[0])
#define GPIOB (&mock_ports[1])
#define GPIOC (&mock_ports[2])

#define GPIO_PIN_3  ((uint16_t)0x0008)
#define GPIO_PIN_4  ((uint16_t)0x0010)
#define GPIO_PIN_5  ((uint16_t)0x0020)
#define GPIO_PIN_6  ((uint16_t)0x0040)
#define GPIO_PIN_7  ((uint16_t)0x0080)
#define GPIO_PIN_8  ((uint16_t)0x0100)
#define GPIO_PIN_9  ((uint16_t)0x0200)
#define GPIO_PIN_10 ((uint16_t)0x0400)
#define GPIO_PIN_13 ((uint16_t)0x2000)

/* User Label giống như CubeMX sẽ sinh ra (theo bảng chân Nucleo) */
#define TL1_A_GPIO_Port GPIOB
#define TL1_A_Pin       GPIO_PIN_3
#define TL1_B_GPIO_Port GPIOB
#define TL1_B_Pin       GPIO_PIN_5
#define TL2_A_GPIO_Port GPIOB
#define TL2_A_Pin       GPIO_PIN_4
#define TL2_B_GPIO_Port GPIOB
#define TL2_B_Pin       GPIO_PIN_10
#define BTN_1_GPIO_Port GPIOA
#define BTN_1_Pin       GPIO_PIN_8
#define BTN_2_GPIO_Port GPIOA
#define BTN_2_Pin       GPIO_PIN_9
#define BTN_3_GPIO_Port GPIOC
#define BTN_3_Pin       GPIO_PIN_7
#define BTN_4_GPIO_Port GPIOB
#define BTN_4_Pin       GPIO_PIN_6

GPIO_PinState HAL_GPIO_ReadPin(GPIO_TypeDef *port, uint16_t pin);
void          HAL_GPIO_WritePin(GPIO_TypeDef *port, uint16_t pin, GPIO_PinState s);

/* tiện ích cho test */
void mock_set_input(GPIO_TypeDef *port, uint16_t pin, int level);
int  mock_get_output(GPIO_TypeDef *port, uint16_t pin);
#endif
