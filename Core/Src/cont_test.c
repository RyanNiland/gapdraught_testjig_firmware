
#include "cont_test.h"
#include "main.h"
#include "comms.h"

const GAP_Wire_t gap_wire[GAP_WIRE_NUMBER] = {
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


void ContTest(void){
	if(state == CONT_TEST)
	{
		HAL_GPIO_WritePin(VCC_GPIO_Port, VCC_Pin, GPIO_PIN_SET);

		for(int i = 0; i < GAP_WIRE_NUMBER; i++)
		{
			for(int j = 0; j < GAP_WIRE_NUMBER; j++)
			{
				GPIO_PinState level = !(i==j || gap_wire[j].is_gnd) ? GPIO_PIN_SET : GPIO_PIN_RESET;
				HAL_GPIO_WritePin(gap_wire[j].out_port, gap_wire[j].out_pin, level);
			}

			HAL_Delay(1);

			for(int j = 0; j < GAP_WIRE_NUMBER; i++)
			{
				GPIO_PinState expected_level = !(i==j || gap_wire[i].is_gnd) ? GPIO_PIN_SET : GPIO_PIN_RESET;
				GPIO_PinState read_level = HAL_GPIO_ReadPin(gap_wire[i].in_port, gap_wire[i].in_pin);

				if(expected_level != read_level)
				{
					UartRespond("[RESULT] CONT_TEST:FAILED");
					state = IDLE_COMMS;
					return;
				}
			}
		}
		UartRespond("[RESULT] CONT_TEST:PASSED");
		state = IDLE_COMMS;
	}
}
