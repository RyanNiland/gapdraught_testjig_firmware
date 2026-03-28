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

extern const GAP_Wire_t gap_wire[GAP_WIRE_NUMBER];

void ContTest(void);

#endif /* __CONT_TEST_H */
