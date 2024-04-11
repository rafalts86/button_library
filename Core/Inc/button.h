/*
 * button.h
 *
 *  Created on: Feb 15, 2024
 *      Author: RAFAŁ
 */

#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

#include "main.h"

typedef void(*button_pressed_fun_t)(void);
typedef void(*button_long_press_fun_t)(void);
typedef void(*button_repeat_fun_t)(void);

typedef enum
{
	IDLE = 0,
	DEBOUNCING,
	PRESSED,
	REPEAT,

}button_state_t;

typedef struct
{
	GPIO_TypeDef *port;
	uint16_t pin;

	button_state_t state;

	uint32_t last_tick;
	uint32_t debounce_time;
	uint32_t long_press_time;
	uint32_t repeat_time;

	button_pressed_fun_t button_pressed_fun;
	button_long_press_fun_t button_long_press_fun;
	button_repeat_fun_t button_repeat_fun;

}button_t;

void button_init(button_t *button_struct, GPIO_TypeDef *port, uint16_t pin, uint32_t debounce_time);
void button_pressed_callback_register(button_t *button_struct, button_pressed_fun_t pressed_fun);
void button_long_press_callback_register(button_t *button_struct, button_long_press_fun_t long_press_fun);
void button_repeat_callback_register(button_t *button_struct, button_repeat_fun_t repeat_fun_t);
void button_set_long_press_time(button_t *button_struct, uint32_t time_ms);
void button_set_repeat_time(button_t *button_struct, uint32_t time_ms);
void button_check_task(button_t *button_struct);

#endif /* INC_BUTTON_H_ */
