
#include "sensors.h"
#include "main.h"
#include "comms.h"
#include <stdio.h>

#define PRESSURE_PRIMING_PERIOD 1000

uint8_t pressure_sample_index = 0;
uint32_t last_pressure_sample = 0;
uint32_t last_solenoid_switch = 0;

PressureResult_t p_result= {0};

void PressureTest(void){

	if(pressure_sample_index <= TOTAL_SAMPLES){

		if(pressure_sample_index == -2){
			HAL_GPIO_WritePin(W_SOL_EN_GPIO_Port, W_SOL_EN_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(D_SOL_EN_GPIO_Port, D_SOL_EN_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(A_SOL_EN_GPIO_Port, A_SOL_EN_Pin, GPIO_PIN_SET);
			last_solenoid_switch = HAL_GetTick();
			pressure_sample_index = -1;
		}

		if(pressure_sample_index == -1 && last_solenoid_switch - HAL_GetTick() > PRESSURE_PRIMING_PERIOD){
				HAL_GPIO_WritePin(A_SOL_EN_GPIO_Port, A_SOL_EN_Pin, GPIO_PIN_RESET);
				last_solenoid_switch = HAL_GetTick();
				pressure_sample_index = 0;
			}


		if(pressure_sample_index >= 0 && pressure_sample_index < TOTAL_SAMPLES && last_pressure_sample - HAL_GetTick() >= SAMPLE_RATE_MS){

					HAL_ADC_Start(&hadc1);
					HAL_ADC_PollForConversion(&hadc1, 10);
					uint32_t val = HAL_ADC_GetValue(&hadc1);
					HAL_ADC_Stop(&hadc1);

					last_pressure_sample = HAL_GetTick();

					if(val > p_result.max) p_result.max = val;
					p_result.last = val;

#if PRESSURE_DEBUG_MODE
					p_result.samples[pressure_sample_index] = val;
#endif
					pressure_sample_index++;
				}
		if(pressure_sample_index == TOTAL_SAMPLES){
			HAL_GPIO_WritePin(D_SOL_EN_GPIO_Port, D_SOL_EN_Pin, GPIO_PIN_SET);
			char txBuffer[UART_TX_BUFFER_SIZE];
			snprintf(txBuffer, sizeof(txBuffer), "[RESULT] P_MAX:%lu,P_LAST:%lu\n",
					(unsigned long) p_result.max,
					(unsigned long) p_result.last);
			UartRespond(txBuffer);
#if PRESSURE_DEBUG_MODE
			for(int i = 0; i < TOTAL_SAMPLES; i++){
				snprintf(txBuffer, sizeof(txBuffer), "[RESULT] SAMPLE_%d:%lu\n",
							i, (unsigned long) p_result.samples[i]);
				UartRespond(txBuffer);
			}
#endif
			pressure_sample_index++;
		}
	}
}
