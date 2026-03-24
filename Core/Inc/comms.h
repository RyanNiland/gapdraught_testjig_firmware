// Inc/comms.h
#ifndef __COMMS_H
#define __COMMS_H

#include <stdint.h>
#include "cmsis_os.h"

#define UART_RX_BUFFER_SIZE  64
#define UART_TX_BUFFER_SIZE  128


typedef enum {
	CMD_UNKNOWN,
	CMD_CONT_T_START,
	CMD_PRESSURE_T_START,
	CMD_FLOW_CAL_START,

	CMD_OPEN_TEMP_SOL_W,
	CMD_OPEN_TEMP_SOL_A,
	CMD_OPEN_SOL_D,
	CMD_CLOSE_SOL_D,

	CMD_RESTART_FM_COUNTER,
	CMD_ENABLE_FM_COUNTER,
	CMD_DISABLE_FM_COUNTER,
	CMD_REPORT_FM_COUNTER,

	CMD_ENABLE_PADC,
	CMD_DISABLE_PADC,
	CMD_REPORT_PRESSURE_VALUES,

	TOGGLE_IN_RELAYS,
	TOGGLE_OUT_RELAYS,
	TOGGLE_VCC,
	READ_IN_PINS,
	TOGGLE_OUT_PWR_PIN,
	TOGGLE_OUT_GND_PIN,
	TOGGLE_OUT_A_PIN,
	TOGGLE_OUT_B_PIN,

	DEBUG_R_ON,
	DEBUG_R_OFF,
	DEBUG_G_ON,
	DEBUG_G_OFF,
	DEBUG_B_ON,
	DEBUG_B_OFF
} CommandType;

typedef struct{
	uint8_t data[UART_RX_BUFFER_SIZE];
	uint16_t len;
} RawMessage_t;

void StartCommsTask(void *argument);
osStatus_t UartRespond(const char *msg);


#endif /* __COMMS_H */
