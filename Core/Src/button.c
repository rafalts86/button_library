/*
 * button.c
 *
 *  Created on: Feb 15, 2024
 *      Author: RAFAŁ
 */

#include <stdbool.h>
#include "button.h"


void button_init(button_t *button_struct, GPIO_TypeDef *port, uint16_t pin, uint32_t debounce_time);

void button_pressed_callback_register(button_t *button_struct, button_long_press_fun_t pressed_fun);
void button_long_press_callback_register(button_t *button_struct, button_long_press_fun_t long_press_fun);
void button_repeat_callback_register(button_t *button_struct, button_repeat_fun_t repeat_fun_t);

void button_set_long_press_time(button_t *button_struct, uint32_t time_ms);
void button_set_repeat_time(button_t *button_struct, uint32_t time_ms);

void button_check_task(button_t *button_struct);

void button_idle_routine(button_t *button_struct);
void button_debouncing_routine(button_t *button_struct);
void button_pressed_routine(button_t *button_struct);
void button_repeat_routine(button_t *button_struct);

static bool is_button_pressed(button_t *button_struct);


void button_init(button_t *button_struct, GPIO_TypeDef *port, uint16_t pin, uint32_t debounce_time)
{
	button_struct->port = port;
	button_struct->pin = pin;

	button_struct->state = IDLE;
	button_struct->debounce_time = debounce_time;

	button_struct->button_pressed_fun = NULL;
	button_struct->button_long_press_fun = NULL;
	button_struct->button_repeat_fun = NULL;
}



void button_check_task(button_t *button_struct)
{
	switch(button_struct->state)
	{
		case IDLE:
			button_idle_routine(button_struct);
			break;

		case DEBOUNCING:
			button_debouncing_routine(button_struct);
			break;

		case PRESSED:
			button_pressed_routine(button_struct);
			break;

		case REPEAT:
			button_repeat_routine(button_struct);
			break;
	}
}

bool is_button_pressed(button_t *button_struct)
{
	if(0 == HAL_GPIO_ReadPin(button_struct->port, button_struct->pin))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void button_idle_routine(button_t *button_struct)
{
	if(true == is_button_pressed(button_struct))
	{
		button_struct->state = DEBOUNCING;
		button_struct->last_tick = HAL_GetTick();
	}
}

void button_debouncing_routine(button_t *button_struct)
{
	if(HAL_GetTick() - button_struct->last_tick > button_struct->debounce_time)
	{
		if(true == is_button_pressed(button_struct))
		{
			button_struct->state = PRESSED;
			button_struct->last_tick = HAL_GetTick();

			if(NULL != button_struct->button_pressed_fun)
			{
				button_struct->button_pressed_fun();
			}
		}
		else
		{
			button_struct->state = IDLE;
		}
	}
}

void button_pressed_routine(button_t *button_struct)
{
	if(false == is_button_pressed(button_struct))
	{
		button_struct->state = IDLE;
	}
	else
	{
		if(HAL_GetTick() - button_struct->last_tick > button_struct->long_press_time)
		{
			button_struct->state = REPEAT;
			button_struct->last_tick = HAL_GetTick();

			if(NULL != button_struct->button_long_press_fun)
			{
				button_struct->button_long_press_fun();
			}
		}
	}
}

void button_repeat_routine(button_t *button_struct)
{
	if(true == is_button_pressed(button_struct))
	{
		if(HAL_GetTick() - button_struct->last_tick > button_struct->repeat_time)
		{
			if(NULL != button_struct->button_repeat_fun)
			{
				button_struct->last_tick = HAL_GetTick();
				button_struct->button_repeat_fun();
			}
		}
	}
	else
	{
		button_struct->state = IDLE;
	}
}


void button_pressed_callback_register(button_t *button_struct, button_pressed_fun_t pressed_fun)
{
	button_struct->button_pressed_fun = pressed_fun;
}

void button_long_press_callback_register(button_t *button_struct, button_long_press_fun_t long_press_fun)
{
	button_struct->button_long_press_fun = long_press_fun;
}

void button_repeat_callback_register(button_t *button_struct, button_repeat_fun_t repeat_fun_t)
{
	button_struct->button_repeat_fun = repeat_fun_t;
}

void button_set_long_press_time(button_t *button_struct, uint32_t time_ms)
{
	button_struct->long_press_time = time_ms;
}

void button_set_repeat_time(button_t *button_struct, uint32_t time_ms)
{
	button_struct->repeat_time = time_ms;
}

