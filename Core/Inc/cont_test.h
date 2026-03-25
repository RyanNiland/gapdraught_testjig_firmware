// Inc/cont_test.h

#ifndef __CONT_TEST_H
#define __CONT_TEST_H

#include "main.h"
#include <stdint.h>

#define GAP_WIRE_NUMBER 4

#define POWER 0
#define GND 1
#define A   2
#define B   3

typedef struct {
	const char *name;
	GPIO_TypeDef *in_port;
	uint16_t in_pin;
	GPIO_TypeDef *out_port;
	uint16_t out_pin;
	GPIO_TypeDef *out_relay_port;
	uint16_t out_relay_pin;
	uint8_t is_gnd;
} GAP_Wire_t;

static const GAP_Wire_t gap_wire[GAP_WIRE_NUMBER] = {
		[POWER] = { "POWER", CONT_PWR_I_GPIO_Port, CONT_PWR_I_Pin,
			CONT_PWR_O_GPIO_Port, CONT_PWR_O_Pin, CONT_O_PWR_RLY_GPIO_Port,
			CONT_O_PWR_RLY_Pin, 0},
		[GND] = { "GND", CONT_GND_I_GPIO_Port, CONT_GND_I_Pin,
			CONT_GND_O_GPIO_Port, CONT_GND_O_Pin, CONT_O_GND_RLY_GPIO_Port,
			CONT_O_GND_RLY_Pin, 1},
		[A] = { "A", CONT_A_I_GPIO_Port, CONT_A_I_Pin, CONT_A_O_GPIO_Port,
			CONT_A_O_Pin, CONT_O_A_RLY_GPIO_Port, CONT_O_A_RLY_Pin, 0},
		[B] = { "B", CONT_B_I_GPIO_Port, CONT_B_I_Pin, CONT_B_O_GPIO_Port,
			CONT_B_O_Pin, CONT_O_B_RLY_GPIO_Port, CONT_O_B_RLY_Pin, 0}
};

#endif /* __CONT_TEST_H */
