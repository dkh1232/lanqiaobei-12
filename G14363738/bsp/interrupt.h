#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_
#include "main.h"
#include "stdbool.h"

struct keys{
	unsigned char key_judage;
	bool key_sta;
	bool single_flag;
	bool long_flag;
	unsigned int key_time;
};

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
#endif
