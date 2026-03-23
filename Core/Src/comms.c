#include "comms.h"
#include "main.h"

typedef struct{
	uint8_t data[UART_RX_BUFFER_SIZE];
	uint_t len;
} RawMessage_t;

uint8_t rxBuffer[UART_RX_BUFFER_SIZE];


void StartCommsTask(void *argument) {

	HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rxBuffer, sizeof(rxBuffer));

	RawMessage_t msg;

	for (;;) {

		if (osMessageQUeueGet(rawMsgQueueHandle, &msg, NULL, portMAX_DELAY)
				== osOK) {
			CommandType cmd = ParseCommand(rxBuffer, Size);
			ProcessCommand(cmd);
		}
	}
}

void HAL_UARTEx_RxEventCallback(UART_HANDLE_TypeDef *huart, uint16_t Size) {

	if (huart->Instance == USART1) {

		RawMessage_t msg;
		msg.len = (Size < UART_RX_BUFFER_SIZE) ? Size : UART_RX_BUFFER_SIZE;
		memcpy(msg.data, rxBuffer, msg.len);

		BaseType_t xHigherPriorityTastWoken = pdFalse;
		osMessageQueuePutFromISR(rawMsgQueueHandle, &msg, 0,
				&xHigherPriorityTaskWoken);

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
		result.cmd = CMD_CONT_T_START;
	else if (strcmp(cmd, "CMD:PRESSURE_T_START") == 0)
		result.cmd = CMD_PRESSURE_T_START;
	else if (strcmp(cmd, "CMD:FLOW_CAL_START") == 0)
		result.cmd = CMD_FLOW_CAL_START;

	// --- Solenoids ---
	else if (strcmp(cmd, "CMD:OPEN_TEMP_SOL_W") == 0)
		result.cmd = CMD_OPEN_TEMP_SOL_W;
	else if (strcmp(cmd, "CMD:OPEN_TEMP_SOL_A") == 0)
		result.cmd = CMD_OPEN_TEMP_SOL_A;
	else if (strcmp(cmd, "CMD:OPEN_SOL_D") == 0)
		result.cmd = CMD_OPEN_SOL_D;
	else if (strcmp(cmd, "CMD:CLOSE_SOL_D") == 0)
		result.cmd = CMD_CLOSE_SOL_D;

	// --- Flow Meter Counter ---
	else if (strcmp(cmd, "CMD:RESTART_FM_COUNTER") == 0)
		result.cmd = CMD_RESTART_FM_COUNTER;
	else if (strcmp(cmd, "CMD:ENABLE_FM_COUNTER") == 0)
		result.cmd = CMD_ENABLE_FM_COUNTER;
	else if (strcmp(cmd, "CMD:DISABLE_FM_COUNTER") == 0)
		result.cmd = CMD_DISABLE_FM_COUNTER;
	else if (strcmp(cmd, "CMD:REPORT_FM_COUNTER") == 0)
		result.cmd = CMD_REPORT_FM_COUNTER;

	// --- Pressure ADC ---
	else if (strcmp(cmd, "CMD:ENABLE_PADC") == 0)
		result.cmd = CMD_ENABLE_PADC;
	else if (strcmp(cmd, "CMD:DISABLE_PADC") == 0)
		result.cmd = CMD_DISABLE_PADC;
	else if (strcmp(cmd, "CMD:REPORT_PRESSURE_VALUES") == 0)
		result.cmd = CMD_REPORT_PRESSURE_VALUES;

	// --- Relays ---
	else if (strcmp(cmd, "CMD:TOGGLE_IN_RELAYS") == 0)
		result.cmd = TOGGLE_IN_RELAYS;
	else if (strcmp(cmd, "CMD:TOGGLE_OUT_RELAYS") == 0)
		result.cmd = TOGGLE_OUT_RELAYS;
	else if (strcmp(cmd, "CMD:TOGGLE_VCC") == 0)
			result.cmd = TOGGLE_VCC;

	// --- Debug LEDs ---
	else if (strcmp(cmd, "CMD:DEBUG_R_ON") == 0)
		result.cmd = DEBUG_R_ON;
	else if (strcmp(cmd, "CMD:DEBUG_R_OFF") == 0)
		result.cmd = DEBUG_R_OFF;
	else if (strcmp(cmd, "CMD:DEBUG_G_ON") == 0)
		result.cmd = DEBUG_G_ON;
	else if (strcmp(cmd, "CMD:DEBUG_G_OFF") == 0)
		result.cmd = DEBUG_G_OFF;
	else if (strcmp(cmd, "CMD:DEBUG_B_ON") == 0)
		result.cmd = DEBUG_B_ON;
	else if (strcmp(cmd, "CMD:DEBUG_B_OFF") == 0)
		result.cmd = DEBUG_B_OFF;

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
	char txBuf[UART_TX_BUFFER_SIZE];

	switch(cmd)
	{
	case CMD_UNKNOWN:
		UartRespond("[ERROR] UNKNOWN COMMAND");
		break;
	case CMD_CONT_T_START:
		break;
	case CMD_PRESSURE_T_START:
		break;
	case CMD_FLOW_CAL_START:
		break;
	case CMD_OPEN_TEMP_SOL_W:
		UartRespond("[DEBUG] OPENING WATER SOL FOR 2s");
		HAL_GPIO_WritePin(W_SOL_EN_GPIO_Port, W_SOL_EN_Pin, GPIO_PIN_SET);
		osDelay(pdMS_TO_TICKS(2000));
		HAL_GPIO_WritePin(W_SOL_EN_GPIO_Port, W_SOL_EN_Pin, GPIO_PIN_RESET);
		break;
	case CMD_OPEN_TEMP_SOL_A:
		UartRespond("[DEBUG] OPENING AIR SOL FOR 2s");
		HAL_GPIO_WritePin(A_SOL_EN_GPIO_Port, A_SOL_EN_Pin, GPIO_PIN_SET);
		osDelay(pdMS_TO_TICKS(2000));
		HAL_GPIO_WritePin(A_SOL_EN_GPIO_Port, A_SOL_EN_Pin, GPIO_PIN_RESET);
		break;
	case CMD_OPEN_SOL_D:
		UartRespond("[DEBUG] OPENING DRAIN SOL");
		HAL_GPIO_WritePin(A_SOL_EN_GPIO_Port, A_SOL_EN_Pin, GPIO_PIN_SET);
		break;
	case CMD_CLOSE_SOL_D:
		UartRespond("[DEBUG] CLOSING DRAIN SOL");
		HAL_GPIO_WritePin(A_SOL_EN_GPIO_Port, A_SOL_EN_Pin, GPIO_PIN_RESET);
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
		break;
	case TOGGLE_IN_RELAYS:
		UartRespond("[DEBUG] TOGGLING GAP IN RELAY");
		HAL_GPIO_TogglePin(CONT_I_COMMS_RLY_GPIO_Port, CONT_I_COMMS_RLY_Pin);
		break;
	case TOGGLE_OUT_RELAYS:
		UartRespond("[DEBUG] TOGGLING GAP OUT RELAYS");
		HAL_GPIO_TogglePin(CONT_O_PWR_RLY_GPIO_Port, CONT_O_PWR_RLY_Pin);
		HAL_GPIO_TogglePin(CONT_O_GND_RLY_GPIO_Port, CONT_O_GND_RLY_Pin);
		HAL_GPIO_TogglePin(CONT_O_A_RLY_GPIO_Port, CONT_O_A_RLY_Pin);
		HAL_GPIO_TogglePin(CONT_O_B_RLY_GPIO_Port, CONT_O_B_RLY_Pin);
		break;
	case TOGGLE_VCC:
		UartRespond("[DEBUG] TOGGLING VCC");
		HAL_GPIO_TogglePin(VCC_GPIO_Port, VCC_Pin);
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
