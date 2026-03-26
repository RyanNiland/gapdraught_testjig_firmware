
#ifndef __FLOW_CAL_H
#define __FLOW_CAL_H

#include "stm32g0xx_hal.h"

void FlowMeter_Init(TIM_HandleTypeDef *htim);

void FlowMeter_IC_Callback(TIM_HandleTypeDef *htim);
void FlowMeter_OVF_Callback(TIM_HandleTypeDef *htim);

#endif /* __FLOW_CAL_H */
