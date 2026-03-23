// Inc/cont_test.h

#ifndef __CONT_TEST_H
#define __CONT_TEST_H
#include "main.h"

#define GAP_WIRE_NUMBER 4

typedef struct {
	const char *name;
	GPIO_TypeDef *in_port;
	uint16_t in_pin;
	GPIO_TypeDef *out_port;
	uint16_t out_pin;
	GPIO_TypeDef *out_relay_port;
	uint16_t out_relay_pin;
} GAP_Wire_t;

const GAP_Wire_t gap_wire[GAP_WIRE_NUMBER] = {
		[PWR] = { "PWR", CONT_PWR_I_GPIO_Port, CONT_PWR_I_Pin,
			CONT_PWR_O_GPIO_Port, CONT_PWR_O_Pin, CONT_O_PWR_RLY_GPIO_Port,
			CONT_O_PWR_RLY_Pin },
		[GND] = { "GND", CONT_GND_I_GPIO_Port, CONT_GND_I_Pin,
			CONT_GND_O_GPIO_Port, CONT_GND_O_Pin, CONT_O_GND_RLY_GPIO_Port,
			CONT_O_GND_RLY_Pin },
		[A] = { "A", CONT_A_I_GPIO_Port, CONT_A_I_Pin, CONT_A_O_GPIO_Port,
			CONT_A_O_Pin, CONT_O_A_RLY_GPIO_Port, CONT_O_A_RLY_Pin },
		[B] = { "A", CONT_B_I_GPIO_Port, CONT_B_I_Pin, CONT_B_O_GPIO_Port,
			CONT_B_O_Pin, CONT_O_B_RLY_GPIO_Port, CONT_O_B_RLY_Pin }
};

#endif #endif /* __CONT_TEST_H */
