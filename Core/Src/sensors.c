
#include "sensors.h"
#include "main.h"
#include "cmsis_os.h"
#include "comms.h"
#include <stdio.h>



void StartPressureTask(void *argument)
{

	for (;;)
	{
		osThreadFlagsWait(0x0001, osFlagsWaitAny, osWaitForever);
		PressureResult_t p_result = {0};
		HAL_GPIO_WritePin(W_SOL_EN_GPIO_Port, W_SOL_EN_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(D_SOL_EN_GPIO_Port, D_SOL_EN_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(A_SOL_EN_GPIO_Port, A_SOL_EN_Pin, GPIO_PIN_SET);
		osDelay(pdMS_TO_TICKS(1000));
		HAL_GPIO_WritePin(A_SOL_EN_GPIO_Port, A_SOL_EN_Pin, GPIO_PIN_RESET);

		for (int i = 0; i < TOTAL_SAMPLES; i++)
		{
			HAL_ADC_Start(&hadc1);
			HAL_ADC_PollForConversion(&hadc1, 10);
			uint32_t val = HAL_ADC_GetValue(&hadc1);
			HAL_ADC_Stop(&hadc1);
			osDelay(pdMS_TO_TICKS(SAMPLE_RATE_MS));

			//potentially add SFW Filter

			if(p_result.max < val) p_result.max = val;
			if(i == TOTAL_SAMPLES-1) p_result.last=val;

#if PRESSURE_DEBUG_MODE
			p_result.samples[i] = val;
#endif
		}
#if PRESSURE_DEBUG_MODE
		osMessageQueuePut(pressureResultQueueHandle, &p_result, 0U, 0U);
#endif
		HAL_GPIO_WritePin(D_SOL_EN_GPIO_Port, D_SOL_EN_Pin, GPIO_PIN_SET);
		char txBuffer[UART_TX_BUFFER_SIZE];
		snprintf(txBuffer, sizeof(txBuffer), "[RESULT]P_MAX:%lu,P_LAST:%lu\n",
				(unsigned long) p_result.max,
				(unsigned long) p_result.last);
		UartRespond(txBuffer);

	}

}
