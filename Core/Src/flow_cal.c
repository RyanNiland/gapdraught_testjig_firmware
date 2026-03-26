#include "flow_cal.h"
#include "stm32g0xx_hal.h"
#include "main.h"

#define FRIST_POUR_VOL_L 1000
#define SECOND_POR_VOL_L 350
//#define TARGET_FR_L_PER_S IDK
#define MS_TO_US 1000
#define MIN_FLOW_CHECK_PERIOD_US 100 * MS_TO_US
#define TURBINE_FM_PPL 5040
#define PRIMING_VOL_L 0.5
#define NR_FLOW_TESTS 2
#define FLOW_METER_SETTLING_TIME_MS 5000
#define ACTIVE_FLOW_TIMEOUT_MS 60000
#define PURGE_PERIOD_MS 1000

typedef enum {
	IDLE, PRIMING, READY, FLOW, POST_FLOW_SETTLING, PURGING,
} FlowCalState_t;

typedef struct {
	uint32_t pulses;
	uint32_t duration_us;
} FlowResult_t;

FlowResult_t calibration_data[NR_FLOW_TESTS] = { 0 };

static const uint32_t target_fm_pulses[] = { FRIST_POUR_VOL_L * TURBINE_FM_PPL,
SECOND_POR_VOL_L * TURBINE_FM_PPL };
static uint32_t overflow_period_us = 0;

static FlowCalState_t flow_state = IDLE;

static TIM_HandleTypeDef *_htim = NULL;

static volatile uint32_t overflow_count = 0;
static volatile uint32_t pulse_count = 0;
static volatile uint32_t first_timestamp = 0;
static volatile uint32_t last_timestamp = 0;
static volatile uint8_t running = 0;

static uint8_t flow_nr = 0;

static uint32_t last_sol_switch = 0;
static uint32_t last_flow_check = 0;

static void PrimeSystem_Start(void);
static void PrimeSystem_Stop(void);
static void Start_Flow(void);
static void Stop_Flow(void);
static void Wait_for_Settle(void);
static void PurgeSystem_Stop(void);

void FlowMeter_Start(void);
void FlowMeter_Stop(void);
static void FlowMeter_Reset(void);
void FlowMeter_GetResults(FlowResult_t *result);
static uint32_t get_timestamp(void);

void Flow_Calibration(void) {
	if (state == FLOW_CAL) {

		switch (flow_state) {
		case IDLE:
			PrimeSystem_Start();
			break;
		case PRIMING:
			PrimeSystem_Stop();
			break;
		case READY:
			Start_Flow();
			break;
		case FLOW:
			Stop_Flow();
			break;
		case POST_FLOW_SETTLING:
			Wait_for_Settle();
			break;
		case PURGING:
			PurgeSystem_Stop();
			break;

		}
	}

}

static void PrimeSystem_Start(void) {

	FlowMeter_Start();
	HAL_GPIO_WritePin(A_SOL_EN_GPIO_Port, A_SOL_EN_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(D_SOL_EN_GPIO_Port, D_SOL_EN_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(W_SOL_EN_GPIO_Port, W_SOL_EN_Pin, GPIO_PIN_SET);
	flow_state = PRIMING;
}

static void PrimeSystem_Stop(void) {
	if (HAL_GetTick() - last_flow_check >= MIN_FLOW_CHECK_PERIOD_US) {
		last_flow_check = HAL_GetTick();

		if (pulse_count > PRIMING_VOL_L * TURBINE_FM_PPL) {
			HAL_GPIO_WritePin(D_SOL_EN_GPIO_Port, D_SOL_EN_Pin, GPIO_PIN_RESET);
			FlowMeter_Stop();
			flow_state = READY;
			last_sol_switch = HAL_GetTick();
			flow_nr = 0;
		}
	}
}

static void Start_Flow(void) {
	if (HAL_GetTick() - last_sol_switch >= FLOW_METER_SETTLING_TIME_MS
			&& flow_nr < NR_FLOW_TESTS) {
		FlowMeter_Start();
		HAL_GPIO_WritePin(D_SOL_EN_GPIO_Port, D_SOL_EN_Pin, GPIO_PIN_SET);
		flow_state = FLOW;
	}
}

static void Stop_Flow(void) {
	if ((pulse_count >= target_fm_pulses[flow_nr])
			|| (overflow_count * overflow_period_us
					>= ACTIVE_FLOW_TIMEOUT_MS * MS_TO_US)) {

		HAL_GPIO_WritePin(D_SOL_EN_GPIO_Port, D_SOL_EN_Pin, GPIO_PIN_RESET);
		last_sol_switch = HAL_GetTick();
		flow_state = POST_FLOW_SETTLING;
	}
}

static void Wait_for_Settle(void) {
	if (HAL_GetTick() - last_sol_switch >= FLOW_METER_SETTLING_TIME_MS) {
		FlowMeter_Stop();
		FlowMeter_GetResults(&calibration_data[flow_nr]);

		if (flow_nr < NR_FLOW_TESTS) {
			flow_state = READY;
		} else {
			HAL_GPIO_WritePin(W_SOL_EN_GPIO_Port, W_SOL_EN_Pin, GPIO_PIN_RESET);

			HAL_Delay(10);

			HAL_GPIO_WritePin(D_SOL_EN_GPIO_Port, D_SOL_EN_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(A_SOL_EN_GPIO_Port, A_SOL_EN_Pin, GPIO_PIN_SET);
			last_sol_switch = HAL_GetTick();
			flow_state = PURGING;

		}

	}
}

static void PurgeSystem_Stop(void) {
	if (HAL_GetTick() - last_sol_switch >= PURGE_PERIOD_MS) {
		HAL_GPIO_WritePin(D_SOL_EN_GPIO_Port, D_SOL_EN_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(A_SOL_EN_GPIO_Port, A_SOL_EN_Pin, GPIO_PIN_RESET);
		flow_state = IDLE;
		state = IDLE_COMMS;
	}
}

void FlowMeter_Init(TIM_HandleTypeDef *htim) {
	_htim = htim;

	uint32_t timer_clock = HAL_RCC_GetPCLK1Freq();
	uint32_t prescaler = _htim->Instance->PSC + 1;
	uint32_t period = _htim->Instance->ARR + 1;
	overflow_period_us = (period * 1000000UL) / (timer_clock / prescaler);
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

void FlowMeter_GetResults(FlowResult_t *result) {
	HAL_TIM_IC_Stop_IT(_htim, TIM_CHANNEL_3);
	__HAL_TIM_DISABLE_IT(_htim, TIM_IT_UPDATE);

	result->pulses = pulse_count;
	result->duration_us = (pulse_count < 2) ? 0 : (last_timestamp - first_timestamp);

	if (running) {
		__HAL_TIM_ENABLE_IT(_htim, TIM_IT_UPDATE);
		HAL_TIM_IC_Start_IT(_htim, TIM_CHANNEL_3);
	}
}

void FlowMeter_IC_Callback(TIM_HandleTypeDef *htim) {
	if (htim->Instance != _htim->Instance)
		return;
	if (htim->Channel != HAL_TIM_ACTIVE_CHANNEL_3)
		return;
	if (!running)
		return;

	uint32_t timestamp = get_timestamp();

	if (pulse_count == 0) {
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

static uint32_t get_timestamp(void) {
	uint32_t ovf;
	uint16_t cnt;

	do {
		ovf = overflow_count;
		cnt = __HAL_TIM_GET_COUNTER(_htim);
	} while (ovf != overflow_count); //retry if overflow isr triggered mid cnt read

	return (ovf << 16) | cnt;
}
