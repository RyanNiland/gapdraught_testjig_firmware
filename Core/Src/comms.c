
#include "comms.h"
#include "main.h"
#include "cont_test.h"
#include "sensors.h"
#include <string.h>
#include "cmsis_os.h"
#include <stdio.h>


uint8_t rxBuffer[UART_RX_BUFFER_SIZE];


CommandType ParseCommand(uint8_t *buf, uint16_t len);
void ProcessCommand(CommandType cmd);


void StartCommsTask(void *argument) {

	HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rxBuffer, sizeof(rxBuffer));

	RawMessage_t msg;

	for (;;) {

		if (osMessageQueueGet(rawMsgQueueHandle, &msg, 0U, portMAX_DELAY)
				== osOK) {
			CommandType cmd = ParseCommand(msg.data, msg.len);
			ProcessCommand(cmd);
		}
	}
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {

	if (huart->Instance == USART1) {

		RawMessage_t msg;
		msg.len = (Size < UART_RX_BUFFER_SIZE) ? Size : UART_RX_BUFFER_SIZE;
		memcpy(msg.data, rxBuffer, msg.len);

		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		osMessageQueuePut(rawMsgQueueHandle, &msg, 0U, 0U);

		HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rxBuffer, sizeof(rxBuffer));
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

	}
}

CommandType ParseCommand(uint8_t *buf, uint16_t len) {

	CommandType result = { CMD_UNKNOWN };

	char cmd[UART_RX_BUFFER_SIZE] = { 0 };
	uint16_t copylen = (len < sizeof(cmd) - 1) ? len : sizeof(cmd) - 1;
	memcpy(cmd, buf, copylen);

	for (int i = copylen - 1; i >= 0; i--) {
		if (cmd[i] == '\n' || cmd[i] == '\r')
			cmd[i] = '\0';
		else
			break;
	}

	// --- Test Sequences ---
	if (strcmp(cmd, "CMD:CONT_T_START") == 0)
		result = CMD_CONT_T_START;
	else if (strcmp(cmd, "CMD:PRESSURE_T_START") == 0)
		result = CMD_PRESSURE_T_START;
	else if (strcmp(cmd, "CMD:FLOW_CAL_START") == 0)
		result = CMD_FLOW_CAL_START;

	// --- Solenoids ---
	else if (strcmp(cmd, "CMD:OPEN_TEMP_SOL_W") == 0)
		result = CMD_OPEN_TEMP_SOL_W;
	else if (strcmp(cmd, "CMD:OPEN_TEMP_SOL_A") == 0)
		result = CMD_OPEN_TEMP_SOL_A;
	else if (strcmp(cmd, "CMD:OPEN_SOL_D") == 0)
		result = CMD_OPEN_SOL_D;
	else if (strcmp(cmd, "CMD:CLOSE_SOL_D") == 0)
		result = CMD_CLOSE_SOL_D;

	// --- Flow Meter Counter ---
	else if (strcmp(cmd, "CMD:RESTART_FM_COUNTER") == 0)
		result = CMD_RESTART_FM_COUNTER;
	else if (strcmp(cmd, "CMD:ENABLE_FM_COUNTER") == 0)
		result = CMD_ENABLE_FM_COUNTER;
	else if (strcmp(cmd, "CMD:DISABLE_FM_COUNTER") == 0)
		result = CMD_DISABLE_FM_COUNTER;
	else if (strcmp(cmd, "CMD:REPORT_FM_COUNTER") == 0)
		result = CMD_REPORT_FM_COUNTER;

	// --- Pressure ADC ---
	else if (strcmp(cmd, "CMD:ENABLE_PADC") == 0)
		result = CMD_ENABLE_PADC;
	else if (strcmp(cmd, "CMD:DISABLE_PADC") == 0)
		result = CMD_DISABLE_PADC;
	else if (strcmp(cmd, "CMD:REPORT_PRESSURE_VALUES") == 0)
		result = CMD_REPORT_PRESSURE_VALUES;

	// --- Relays ---
	else if (strcmp(cmd, "CMD:TOGGLE_IN_RELAYS") == 0)
		result = TOGGLE_IN_RELAYS;
	else if (strcmp(cmd, "CMD:TOGGLE_OUT_RELAYS") == 0)
		result = TOGGLE_OUT_RELAYS;
	else if (strcmp(cmd, "CMD:TOGGLE_VCC") == 0)
			result = TOGGLE_VCC;
	else if (strcmp(cmd, "CMD:READ_IN_PINS") == 0)
		result = READ_IN_PINS;
	else if (strcmp(cmd, "CMD:TOGGLE_OUT_POWER_PIN") == 0)
		result = TOGGLE_OUT_PWR_PIN;
	else if (strcmp(cmd, "CMD:TOGGLE_OUT_GND_PIN") == 0)
		result = TOGGLE_OUT_GND_PIN;
	else if (strcmp(cmd, "CMD:TOGGLE_OUT_A_PIN") == 0)
		result = TOGGLE_OUT_A_PIN;
	else if (strcmp(cmd, "CMD:TOGGLE_OUT_B_PIN") == 0)
		result = TOGGLE_OUT_B_PIN;

	// --- Debug LEDs ---
	else if (strcmp(cmd, "CMD:DEBUG_R_ON") == 0)
		result = DEBUG_R_ON;
	else if (strcmp(cmd, "CMD:DEBUG_R_OFF") == 0)
		result = DEBUG_R_OFF;
	else if (strcmp(cmd, "CMD:DEBUG_G_ON") == 0)
		result = DEBUG_G_ON;
	else if (strcmp(cmd, "CMD:DEBUG_G_OFF") == 0)
		result = DEBUG_G_OFF;
	else if (strcmp(cmd, "CMD:DEBUG_B_ON") == 0)
		result = DEBUG_B_ON;
	else if (strcmp(cmd, "CMD:DEBUG_B_OFF") == 0)
		result = DEBUG_B_OFF;

	return result;

}

osStatus_t UartRespond(const char *msg)
{

	osStatus_t status = osMutexAcquire(uartTxMutexHandle, 100);
	if (status == osOK)
	{
		HAL_UART_Transmit(&huart1, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
		osMutexRelease(uartTxMutexHandle);
	}
	return status;
}

void ProcessCommand(CommandType cmd)
{
	char txBuffer[UART_TX_BUFFER_SIZE];
	GPIO_PinState pin_level;

	switch(cmd)
	{
	case CMD_UNKNOWN:
		UartRespond("[ERROR] UNKNOWN COMMAND\n");
		break;
	case CMD_CONT_T_START:
		break;
	case CMD_PRESSURE_T_START:
		UartRespond("[DEBUG] STARTING PRESSURE TEST\n");
		osThreadFlagsSet(PressureTaskHandle, 0x0001);
		break;
	case CMD_FLOW_CAL_START:
		break;
	case CMD_OPEN_TEMP_SOL_W:
		UartRespond("[DEBUG] OPENING WATER SOL FOR 2s\n");
		HAL_GPIO_WritePin(W_SOL_EN_GPIO_Port, W_SOL_EN_Pin, GPIO_PIN_SET);
		osDelay(pdMS_TO_TICKS(2000));
		HAL_GPIO_WritePin(W_SOL_EN_GPIO_Port, W_SOL_EN_Pin, GPIO_PIN_RESET);
		break;
	case CMD_OPEN_TEMP_SOL_A:
		UartRespond("[DEBUG] OPENING AIR SOL FOR 2s\n");
		HAL_GPIO_WritePin(A_SOL_EN_GPIO_Port, A_SOL_EN_Pin, GPIO_PIN_SET);
		osDelay(pdMS_TO_TICKS(2000));
		HAL_GPIO_WritePin(A_SOL_EN_GPIO_Port, A_SOL_EN_Pin, GPIO_PIN_RESET);
		break;
	case CMD_OPEN_SOL_D:
		UartRespond("[DEBUG] OPENING DRAIN SOL\n");
		HAL_GPIO_WritePin(D_SOL_EN_GPIO_Port, D_SOL_EN_Pin, GPIO_PIN_SET);
		break;
	case CMD_CLOSE_SOL_D:
		UartRespond("[DEBUG] CLOSING DRAIN SOL\n");
		HAL_GPIO_WritePin(D_SOL_EN_GPIO_Port, D_SOL_EN_Pin, GPIO_PIN_RESET);
		break;
	case CMD_RESTART_FM_COUNTER:
		break;
	case CMD_ENABLE_FM_COUNTER:
		break;
	case CMD_DISABLE_FM_COUNTER:
		break;
	case CMD_REPORT_FM_COUNTER:
		break;
	case CMD_ENABLE_PADC:
		break;
	case CMD_DISABLE_PADC:
		break;
	case CMD_REPORT_PRESSURE_VALUES:

#if PRESSURE_DEBUG_MODE
		PressureResult_t result;
		if (osMessageQueueGet(pressureResultQueueHandle, &result, NULL, 0) == osOK){
			for(int i = 0; i < TOTAL_SAMPLES; i++){
				snprintf(txBuffer, sizeof(txBuffer), "[RESULT] SAMPLE_%d:%lu\n",
						i, (unsigned long) result.samples[i]);
				UartRespond(txBuffer);
			}
		}
#endif

		break;
	case TOGGLE_IN_RELAYS:
		UartRespond("[DEBUG] TOGGLING GAP IN RELAY\n");
		HAL_GPIO_TogglePin(CONT_I_COMMS_RLY_GPIO_Port, CONT_I_COMMS_RLY_Pin);
		break;
	case TOGGLE_OUT_RELAYS:
		UartRespond("[DEBUG] TOGGLING GAP OUT RELAYS\n");
		HAL_GPIO_TogglePin(gap_wire[POWER].out_relay_port, gap_wire[POWER].out_relay_pin);
		HAL_GPIO_TogglePin(gap_wire[GND].out_relay_port, gap_wire[GND].out_relay_pin);
		HAL_GPIO_TogglePin(gap_wire[A].out_relay_port, gap_wire[A].out_relay_pin);
		HAL_GPIO_TogglePin(gap_wire[B].out_relay_port, gap_wire[B].out_relay_pin);
		break;
	case TOGGLE_VCC:
		UartRespond("[DEBUG] TOGGLING VCC\n");
		HAL_GPIO_TogglePin(VCC_GPIO_Port, VCC_Pin);
		break;
	case READ_IN_PINS:
		for(int i = 0; i < GAP_WIRE_NUMBER; i++){
			pin_level = HAL_GPIO_ReadPin(gap_wire[i].in_port, gap_wire[i].in_pin);
			snprintf(txBuffer, sizeof(txBuffer), "PIN:%s STATE:%s\n",
					gap_wire[i].name,
					pin_level == GPIO_PIN_SET ? "HIGH" : "LOW");
			UartRespond(txBuffer);
			}
			break;
	case TOGGLE_OUT_PWR_PIN:
		HAL_GPIO_TogglePin(gap_wire[POWER].out_port, gap_wire[POWER].out_pin);
		pin_level = HAL_GPIO_ReadPin(gap_wire[POWER].out_port,
				gap_wire[POWER].out_pin);
		snprintf(txBuffer, sizeof(txBuffer),
				"[DEBUG] TOGGLING POWER OUT PIN -> %s\n",
				pin_level == GPIO_PIN_SET ? "HIGH" : "LOW");
		UartRespond(txBuffer);
		break;
	case TOGGLE_OUT_GND_PIN:
		HAL_GPIO_TogglePin(gap_wire[GND].out_port, gap_wire[GND].out_pin);
		pin_level = HAL_GPIO_ReadPin(gap_wire[GND].out_port,
				gap_wire[GND].out_pin);
		snprintf(txBuffer, sizeof(txBuffer),
				"[DEBUG] TOGGLING GND OUT PIN -> %s\n",
				pin_level == GPIO_PIN_SET ? "HIGH" : "LOW");
		UartRespond(txBuffer);
		break;
	case TOGGLE_OUT_A_PIN:
		HAL_GPIO_TogglePin(gap_wire[A].out_port, gap_wire[A].out_pin);
		pin_level = HAL_GPIO_ReadPin(gap_wire[A].out_port,
				gap_wire[A].out_pin);
		snprintf(txBuffer, sizeof(txBuffer),
				"[DEBUG] TOGGLING A OUT PIN -> %s\n",
				pin_level == GPIO_PIN_SET ? "HIGH" : "LOW");
		UartRespond(txBuffer);
		break;
	case TOGGLE_OUT_B_PIN:
		HAL_GPIO_TogglePin(gap_wire[B].out_port, gap_wire[B].out_pin);
		pin_level = HAL_GPIO_ReadPin(gap_wire[B].out_port,
				gap_wire[B].out_pin);
		snprintf(txBuffer, sizeof(txBuffer),
				"[DEBUG] TOGGLING B OUT PIN -> %s\n",
				pin_level == GPIO_PIN_SET ? "HIGH" : "LOW");
		UartRespond(txBuffer);
		break;
	case DEBUG_R_ON:
		UartRespond("[DEBUG] DEBUG-R-ON COMMAND RECEIVED");
		HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_SET);
		break;
	case DEBUG_R_OFF:
		UartRespond("[DEBUG] DEBUG-R-OFF COMMAND RECEIVED");
		HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_RESET);
		break;
	case DEBUG_G_ON:
		UartRespond("[DEBUG] DEBUG-G-ON COMMAND RECEIVED");
		HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_SET);
		break;
	case DEBUG_G_OFF:
		UartRespond("[DEBUG] DEBUG-G-OFF COMMAND RECEIVED");
		HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_RESET);
		break;
	case DEBUG_B_ON:
		UartRespond("[DEBUG] DEBUG-B-ON COMMAND RECEIVED");
		HAL_GPIO_WritePin(LED_B_GPIO_Port, LED_B_Pin, GPIO_PIN_SET);
		break;
	case DEBUG_B_OFF:
		UartRespond("[DEBUG] DEBUG-B-OFF COMMAND RECEIVED");
		HAL_GPIO_WritePin(LED_B_GPIO_Port, LED_B_Pin, GPIO_PIN_RESET);
		break;
	default:
		UartRespond("[ERROR] BROKEN UART COMMAND HANDLING");
		break;
	}
}
