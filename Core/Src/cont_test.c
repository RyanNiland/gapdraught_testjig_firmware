
#include "cont_test.h"
#include "main.h"
#include "comms.h"


void ContTest(void){
	if(state == CONT_TEST)
	{
		HAL_GPIO_WritePin(VCC_GPIO_Port, VCC_Pin, GPIO_PIN_SET);

		for(int i = 0; i < GAP_WIRE_NUMBER; i++)
		{
			for(int j = 0; j < GAP_WIRE_NUMBER; i++)
			{
				GPIO_PinState level = !(i==j || gap_wire[i].is_gnd) ? GPIO_PIN_SET : GPIO_PIN_RESET;
				HAL_GPIO_WritePin(gap_wire[i].out_port, gap_wire[i].out_pin, level);
			}

			HAL_Delay(1);

			for(int j = 0; j < GAP_WIRE_NUMBER; i++)
			{
				GPIO_PinState expected_level = !(i==j || gap_wire[i].is_gnd) ? GPIO_PIN_SET : GPIO_PIN_RESET;
				GPIO_PinState read_level = HAL_GPIO_ReadPin(gap_wire[i].in_port, gap_wire[i].in_pin);

				if(expected_level != read_level)
				{
					UartRespond("[RESULT] CONT_TEST:FAILED");
					return;
				}
			}
		}
		UartRespond("[RESULT] CONT_TEST:PASSED");
		state = IDLE_COMMS;
	}
}
