/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "led.h"
#include "lcd.h"
#include "interrupt.h"
#include "stdio.h"
#include "string.h"
#include "stdbool.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
extern struct keys key[4];
extern char rxbuffer[30];
extern unsigned char rxindex;
extern unsigned char rxdat;

unsigned char view=0;
unsigned char control = 0;
unsigned char led_flag=0;
unsigned char LED;
unsigned char CN_BR=0;
unsigned char VN_BR=0;
unsigned char ID_LE=8;
float cnbr_price=3.50;
float vnbr_price=2.00;
unsigned char ParkType[8][5];
unsigned char ParkCode[8][5];
unsigned char ParkTime[8][13];
unsigned char ParkFlag[8];

unsigned char RxParkType[5];
unsigned char RxParkCode[5];
unsigned char RxParkTime[13];
unsigned char EmptyIndex;
void key_pro(void);
void rx_pro(void);
void view_pro(void);
void control_PA7(void);
void led_control(void);
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM3_Init();
  MX_TIM17_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  LCD_Init();
  HAL_TIM_Base_Start_IT(&htim3);
  HAL_UART_Receive_IT(&huart1,&rxdat,1);
  HAL_TIM_PWM_Start(&htim17,TIM_CHANNEL_1);
  /* USER CODE END 2 */
	LCD_Clear(Black);
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);
	
	led_disp(0);
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	if(rxindex!=0)
	{
		unsigned char temp =rxindex;
		HAL_Delay(1);
		if(temp == rxindex)
			rx_pro();
	}
	key_pro();
	view_pro();
	control_PA7();
	led_control();
	led_disp(LED);
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV3;
  RCC_OscInitStruct.PLL.PLLN = 20;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void key_pro()
{
	if(key[0].single_flag==1)
	{
		key[0].single_flag=0;
		view+=1;
		if(view>1)
			view=0;
	}
	if(key[1].single_flag==1)
	{
		key[1].single_flag=0;
		if(view==1)
		{
			cnbr_price+=0.5f;
			vnbr_price+=0.5f;
		}
	}
	if(key[2].single_flag==1)
	{
		key[2].single_flag=0;
		if(view==1)
		{
			cnbr_price-=0.5f;
			vnbr_price-=0.5f;
		}
	}
	if(key[3].single_flag==1)
	{
		key[3].single_flag=0;
		control+=1;
		if(control>1)
			control=0;
}
	}

unsigned char find_emptycar(void)
{
	unsigned char i=0;
	for(i = 0;i<8;i++)
	{
		if(ParkFlag[i]==0)
		{
			break;
		}
	}
	return i;
}

unsigned char Find_ParkedCar(unsigned char *fParkType, unsigned char *fParkCode)
{
	unsigned char i = 0;
	for(i = 0; i < 8; i++)
	{
		if((strncmp(ParkType[i], fParkType, 4) == 0) && (strncmp(ParkCode[i], fParkCode, 4) == 0))
		{
			break;
		}
	}
	return i;
}

void rx_pro()
{
	unsigned char numok;
	for(unsigned int j=10;j<22;j++)
	{
		if(rxbuffer[j]<='9'&&rxbuffer[j]>='0')
			numok=1;
		else
		{
			numok=0;
			break;
		}
	}
	if(rxindex==22&&(strncmp(rxbuffer,"VNBR",4)==0||strncmp(rxbuffer,"CNBR",4)==0)&&numok)
	{
		sscanf(rxbuffer, "%4s:%4s:%12s", RxParkType, RxParkCode, RxParkTime);
		unsigned char ParkedIndex = Find_ParkedCar(RxParkType, RxParkCode);
		if(ParkedIndex==8)
		{
			EmptyIndex=find_emptycar();
			if(EmptyIndex==8)
			{
				char temp[20];
				sprintf(temp,"Error\n");
				HAL_UART_Transmit(&huart1,(uint8_t *)temp,strlen(temp),50);
			}
			else{
				if(strncmp("CNBR", RxParkType,4) == 0)
					{
						CN_BR++;
						ID_LE--;
					}
				if(strncmp("VNBR", RxParkType,4) == 0)
					{
						VN_BR++;
						ID_LE--;
					}
				strcpy(ParkType[EmptyIndex],RxParkType);
				strcpy(ParkCode[EmptyIndex], RxParkCode);
				strcpy(ParkTime[EmptyIndex], RxParkTime);
				ParkFlag[EmptyIndex] = 1;
			}
		}
else
		{
			unsigned int dayin, dayout;
			unsigned int hourin, hourout;
			int MandSin, MandSout;
			dayin = (ParkTime[ParkedIndex][4] - '0') * 10 + (ParkTime[ParkedIndex][5] - '0');
			dayout = (RxParkTime[4] - '0') * 10 + (RxParkTime[5] - '0');
			unsigned char daylong = dayout - dayin;
			hourin = (ParkTime[ParkedIndex][6] - '0') * 10 + (ParkTime[ParkedIndex][7] - '0');
			hourout = (RxParkTime[6] - '0') * 10 + (RxParkTime[7] - '0');
			unsigned char hourlong = (hourout + daylong * 24) - hourin;
			MandSin = (ParkTime[ParkedIndex][8] - '0') * 1000 + (ParkTime[ParkedIndex][9] - '0') * 100 + (ParkTime[ParkedIndex][10] - '0') * 10 + ParkTime[ParkedIndex][11] - '0';
			MandSout = (RxParkTime[8] - '0') * 1000 + (RxParkTime[9] - '0') * 100 + (RxParkTime[10] - '0') * 10 + RxParkTime[11] - '0';
			if(MandSout - MandSin > 0)
				hourlong ++;
			if(strncmp("CNBR", RxParkType, 4) == 0)
			{
					char temp[30];
					sprintf(temp,"%4s:%4s:%02d:%.2f\n", ParkType[ParkedIndex], ParkCode[ParkedIndex], hourlong, hourlong * cnbr_price);
					HAL_UART_Transmit(&huart1,(uint8_t *)temp,strlen(temp),50);
					CN_BR--;
					ID_LE++;
			}
				
			if(strncmp("VNBR", RxParkType, 4) == 0)
			{
					char temp[30];
					sprintf(temp,"%4s:%4s:%02d:%.2f\n", ParkType[ParkedIndex], ParkCode[ParkedIndex], hourlong, hourlong * vnbr_price);
					HAL_UART_Transmit(&huart1,(uint8_t *)temp,strlen(temp),50);
					VN_BR--;
					ID_LE++;
			}
				
			strcpy(ParkType[ParkedIndex], "0");
			strcpy(ParkCode[ParkedIndex], "0");
			strcpy(ParkTime[ParkedIndex], "0");
			ParkFlag[ParkedIndex] = 0;
		}
	}
	else
	{
		char temp[20];
		sprintf(temp,"Error\n");
		HAL_UART_Transmit(&huart1,(uint8_t *)temp,strlen(temp),50);
	}
	rxindex=0;
	memset(rxbuffer,0,30);
}

void view_pro()
{
	if(view==0)
	{
		LCD_DisplayStringLine(Line1,(uint8_t *)"       Data    ");
		char text[30];
		sprintf(text,"   CNBR:%d     ",CN_BR);
		LCD_DisplayStringLine(Line3,(uint8_t *)text);
		sprintf(text,"   VNBR:%d     ",VN_BR);
		LCD_DisplayStringLine(Line5,(uint8_t *)text);
		sprintf(text,"   IDLE:%d     ",ID_LE);
		LCD_DisplayStringLine(Line7,(uint8_t *)text);		
	}
	if(view==1)
	{
	LCD_DisplayStringLine(Line1,(uint8_t *)"       Para       ");
	char text[30];
	sprintf(text,"   CNBR:%.2f      ",cnbr_price);
	LCD_DisplayStringLine(Line3,(uint8_t *)text);
	sprintf(text,"   VNBR:%.2f      ",vnbr_price);
	LCD_DisplayStringLine(Line5,(uint8_t *)text);
	LCD_DisplayStringLine(Line7,(uint8_t *)"                   ");		
	}
}
void control_PA7()
{
	if(control==0)
	{
		__HAL_TIM_SET_PRESCALER(&htim17,400-1);
		__HAL_TIM_SET_AUTORELOAD(&htim17,100-1);
		__HAL_TIM_SET_COMPARE(&htim17,TIM_CHANNEL_1,20);
		HAL_TIM_GenerateEvent(&htim17,TIM_EVENTSOURCE_UPDATE);
		led_flag=1;
	}
	else
	{
		__HAL_TIM_SET_COMPARE(&htim17,TIM_CHANNEL_1,0);
		HAL_TIM_GenerateEvent(&htim17,TIM_EVENTSOURCE_UPDATE);
		led_flag=0;
	}
}
void led_control()
{
	if(ID_LE)
	{
		
		LED=LED & 0xfe|0x01;
	}
	else
	{
		LED=LED &0xfe;
	}
	if(led_flag==1)
	{
		LED=LED&0xcf|0x30;
	}
	else
	{
		LED=LED&0xcf;
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
