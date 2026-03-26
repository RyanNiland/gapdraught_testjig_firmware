#include "flow_cal.h"
#include "stm32g0xx_hal.h"

static TIM_HandleTypeDef *_htim = NULL;

static volatile uint32_t overflow_count = 0;
static volatile uint32_t pulse_count = 0;
static volatile uint32_t first_timestamp = 0;
static volatile uint32_t last_timestamp = 0;
static volatile uint8_t running = 0;


static void FlowMeter_Reset(void);
static uint32_t get_timestamp(void);

void FlowMeter_Init(TIM_HandleTypeDef *htim) {
	_htim = htim;
}

void FlowMeter_Start(void) {
	FlowMeter_Reset();
	running = 1;
	__HAL_TIM_ENABLE_IT(_htim, TIM_IT_UPDATE);
	HAL_TIM_IC_Start_IT(_htim, TIM_CHANNEL_3);

}

static void FlowMeter_Reset(void) {
	overflow_count = 0;
	pulse_count = 0;
	first_timestamp = 0;
	last_timestamp = 0;
	__HAL_TIM_SET_COUNTER(_htim, 0);
}

void FlowMeter_Stop(void) {
	running = 0;
	HAL_TIM_IC_Stop_IT(_htim, TIM_CHANNEL_3);
	__HAL_TIM_DISABLE_IT(_htim, TIM_IT_UPDATE);
}

void FlowMeter_GetResults(uint32_t *count, uint32_t *duration_us) {
	HAL_TIM_IC_Stop_IT(_htim, TIM_CHANNEL_3);
	__HAL_TIM_DISABLE_IT(_htim, TIM_IT_UPDATE);

	*count = pulse_count;
	*duration_us = (pulse_count < 2) ? 0 : (last_timestamp - first_timestamp);

	if (running) {
		__HAL_TIM_ENABLE_IT(_htim, TIM_IT_UPDATE);
		HAL_TIM_IC_Start_IT(_htim, TIM_CHANNEL_3);
	}
}


void FlowMeter_IC_Callback(TIM_HandleTypeDef *htim){
	if(htim->Instance != _htim->Instance) return;
	if(htim->Channel != HAL_TIM_ACTIVE_CHANNEL_3) return;
	if(!running) return;

	uint32_t timestamp = get_timestamp();

	if(pulse_count == 0)
	{
		first_timestamp = timestamp;
	}

	last_timestamp = timestamp;
	pulse_count++;
}

void FlowMeter_OVF_Callback(TIM_HandleTypeDef *htim) {
	if (htim->Instance != _htim->Instance)
		return;
	overflow_count++;
}

static uint32_t get_timestamp(void){
	uint32_t ovf;
	uint16_t cnt;

	do{
		ovf = overflow_count;
		cnt = __HAL_TIM_GET_COUNTER(_htim);
	} while (ovf != overflow_count); //retry if overflow isr triggered mid cnt read

	return (ovf << 16) | cnt;
}
