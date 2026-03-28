#include <pressure_test.h>
#include "main.h"
#include "comms.h"
#include <stdio.h>

#define PRESSURE_PRIMING_PERIOD 1000

typedef enum {
 PRESSURISE, SEAL, MONITOR, DEPRESSURISE
} PressureState_t;

PressureState_t p_state = PRESSURISE;

uint8_t pressure_sample_index = 0;
uint32_t last_pressure_sample = 0;
uint32_t ema = 0;
static uint32_t last_solenoid_switch = 0;

PressureResult_t p_result = { 0 };

void PressuriseSystem(void);
void SealSystem(void);
uint32_t ReadADC(void);
void ReportPressureResults(void);
void DepressuriseSystem(void);
void MonitorSystem(void);

void PressureTest(void) {

	if (state == PRESSURE_TEST) {
		switch (p_state) {

		case PRESSURISE:
			PressuriseSystem();
			break;
		case SEAL:
			SealSystem();
			break;
		case MONITOR:
			MonitorSystem();
			break;
		case DEPRESSURISE:
			DepressuriseSystem();
			ReportPressureResults();
			break;
		}

	}
}

void PressuriseSystem(void) {
	HAL_GPIO_WritePin(W_SOL_EN_GPIO_Port, W_SOL_EN_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(D_SOL_EN_GPIO_Port, D_SOL_EN_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(A_SOL_EN_GPIO_Port, A_SOL_EN_Pin, GPIO_PIN_SET);
	last_solenoid_switch = HAL_GetTick();

	p_state = SEAL;
}

void SealSystem(void) {

	if (HAL_GetTick() - last_solenoid_switch >= PRESSURE_PRIMING_PERIOD) {
		HAL_GPIO_WritePin(A_SOL_EN_GPIO_Port, A_SOL_EN_Pin, GPIO_PIN_RESET);

		p_state = MONITOR;
		pressure_sample_index = 0;
	}
}

void MonitorSystem(void) {
	if ((pressure_sample_index < TOTAL_SAMPLES)
			&& (HAL_GetTick() - last_pressure_sample >= SAMPLE_RATE_MS)) {

		uint32_t val = ReadADC();
		last_pressure_sample = HAL_GetTick();
#if APPLY_EMA_FILTER
		ema = 0.7*ema + 0.3*val;
		val = ema;
#endif
		if (val > p_result.max)
			p_result.max = val;
		p_result.last = val;


#if PRESSURE_DEBUG_MODE
		p_result.samples[pressure_sample_index] = val;
#endif
		pressure_sample_index++;
	}
}

void DepressuriseSystem(void) {
	HAL_GPIO_WritePin(D_SOL_EN_GPIO_Port, D_SOL_EN_Pin, GPIO_PIN_SET);
}

uint32_t ReadADC(void) {
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 10);
	uint32_t val = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_Stop(&hadc1);

	return val;
}

void ReportPressureResults(void) {
	char txBuffer[UART_TX_BUFFER_SIZE];

	snprintf(txBuffer, sizeof(txBuffer), "[RESULT] P_MAX:%lu,P_LAST:%lu\n",
			(unsigned long) p_result.max, (unsigned long) p_result.last);
	UartRespond(txBuffer);

#if PRESSURE_DEBUG_MODE
	for (int i = 0; i < TOTAL_SAMPLES; i++) {
		snprintf(txBuffer, sizeof(txBuffer), "[RESULT] SAMPLE_%d:%lu\n", i,
				(unsigned long) p_result.samples[i]);
		UartRespond(txBuffer);
	}
#endif

	p_state = PRESSURISE;
	state = IDLE_COMMS;
}
