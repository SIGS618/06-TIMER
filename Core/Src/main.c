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
#include "uart.h"
#include "LED.h"
#include "buffer.h"

#include "stdlib.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define LED_R_CHANNEL TIM_CHANNEL_1
#define LED_G_CHANNEL TIM_CHANNEL_2
#define LED_B_CHANNEL TIM_CHANNEL_3
#define RGB_FACTOR (htim5.Init.Period / 255)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t uart_rx_data[MAX_RX_SIZE];
//uint8_t uart_tx_data[MAX_TX_SIZE];

// 用于串口输出状态
uint8_t str_pattern[MAX_RX_SIZE];
uint8_t str_interval[MAX_RX_SIZE];

int pattern = 3;
uint8_t cnt = 0;     // 模式1
uint8_t idx_seq;     // 模式1、2中, RGB序列下标
uint8_t R_value = 0; // 模式3
int flag_inc = 1;    // 标志位: 0-RGB值递减, 1-RGB值递增
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void parse_cmd(void); // 解析命令
void set_pattern(int mode); // 设置运行模式
void set_speed(const uint8_t *digits, int len); // 设置速度
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim == &htim6) { // 判断中断来源
    switch (pattern) { // 根据不同模式设置对应LED
      case 1:
        __HAL_TIM_SET_COMPARE(&htim5, LED_R_CHANNEL, rgb_seq_p1[idx_seq].R * RGB_FACTOR);
        __HAL_TIM_SET_COMPARE(&htim5, LED_G_CHANNEL, rgb_seq_p1[idx_seq].G * RGB_FACTOR);
        __HAL_TIM_SET_COMPARE(&htim5, LED_B_CHANNEL, rgb_seq_p1[idx_seq].B * RGB_FACTOR);
        idx_seq = (idx_seq + 1) % len_seq_p1;
        break;
      case 2:
        __HAL_TIM_SET_COMPARE(&htim5, LED_R_CHANNEL, rgb_seq_p2[idx_seq].R * RGB_FACTOR);
        __HAL_TIM_SET_COMPARE(&htim5, LED_G_CHANNEL, rgb_seq_p2[idx_seq].G * RGB_FACTOR);
        __HAL_TIM_SET_COMPARE(&htim5, LED_B_CHANNEL, rgb_seq_p2[idx_seq].B * RGB_FACTOR);
        idx_seq = (idx_seq + 1) % len_seq_p2;
        break;
      case 3:
        if(flag_inc) R_value++;
        else R_value--;

        if(R_value == 0 || R_value == 255)
          flag_inc ^= 1;

        __HAL_TIM_SET_COMPARE(&htim5, LED_R_CHANNEL, R_value * RGB_FACTOR);
      default:
        break;
    }
  }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart == &huart1) {
    if (!UART_Buffer_isEmpty(&TX_BUF)) { // 发送缓冲区非空
      DataBlocks_TypeDef *dblock = UART_Buffer_Pop(&TX_BUF);
      HAL_UART_Transmit_IT(&huart1, dblock->start, dblock->size);
    }
  }
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
  if (huart == &huart1) {
    // 如果buffer已满, 不再写入新的数据
    if(UART_Buffer_isFull(&RX_BUF)) return;

    // 入队
    UART_Buffer_Push(&RX_BUF, uart_rx_data, Size);

    // 再次开启中断
    HAL_UARTEx_ReceiveToIdle_IT(&huart1, uart_rx_data, MAX_RX_SIZE);
  }
}

void parse_cmd(void) { // 解析命令
  // 出队
  DataBlocks_TypeDef *dblock = UART_Buffer_Pop(&RX_BUF);

  uint8_t *cmd = (uint8_t *) malloc(dblock->size);
  memcpy(cmd, dblock->start, dblock->size);

  switch (cmd[0]) {
    case 'p':
      set_pattern(cmd[1] - '0');
      memcpy(str_pattern, cmd+1, dblock->size-1);
      break;
    case 's':
      set_speed(cmd+1, dblock->size-1);
      memcpy(str_interval, cmd+1, dblock->size-1);
      break;
    default:
      break;
  }
  free(cmd);

  // 回写当前工作状态: 模式 + 速度, 发送数据模板: pattern:%d interval: %dms
  // 计算LED变换间隔, 单位ms
  /* 踩坑点1: uart_tx_data[]数组需要被定义为全局变量, 否则sprintf()函数无法正确运行 */
  UART_Transmit_IT(&huart1, (uint8_t *)"pattern: ", 9, &TX_BUF);
  UART_Transmit_IT(&huart1, str_pattern, sizeof(str_pattern), &TX_BUF);
  UART_Transmit_IT(&huart1, (uint8_t *)", interval: ", 12, &TX_BUF);
  UART_Transmit_IT(&huart1, str_interval, sizeof(str_interval), &TX_BUF);
  UART_Transmit_IT(&huart1, (uint8_t *)"ms", 2, &TX_BUF);
}

void set_pattern(int mode)
{
  switch (mode) {
    case 1:
      pattern = 1;
      __disable_irq();
      idx_seq = 0;
      __enable_irq();
      break;
    case 2:
      pattern = 2;
      __disable_irq();
      idx_seq = 0;
      __enable_irq();
      break;
    case 3:
      pattern = 3;
      __HAL_TIM_SET_COMPARE(&htim5, LED_R_CHANNEL, 0);
      __HAL_TIM_SET_COMPARE(&htim5, LED_G_CHANNEL, 0);
      __HAL_TIM_SET_COMPARE(&htim5, LED_B_CHANNEL, 0);
      __disable_irq();
      cnt = 0;
      flag_inc = 1;
      __enable_irq();
      break;
    default:
      break;
  }
}

void set_speed(const uint8_t *digits, int len)
{ // digits为需要设置的间隔(单位为ms)的字符串, len为该字符串长度
  int interval = 0;
  for (int i = 0; i < len; i++) {
    interval *= 10;
    interval += digits[i]-'0';
  }
  // 重新设置tim6的period, 即自动重载值autoreload value
  __HAL_TIM_SET_AUTORELOAD(&htim6, interval * 10 - 1);
}
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
  MX_TIM5_Init();
  MX_TIM6_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  // 初始化接收/发送buffer
  UART_Buffer_Init(&TX_BUF);
  UART_Buffer_Init(&RX_BUF);
  // 开启串口空闲中断, stm32会在接收Size字节或空闲时产生中断
  HAL_UARTEx_ReceiveToIdle_IT(&huart1, uart_rx_data, MAX_RX_SIZE);
  HAL_TIM_Base_Start(&htim5); // 通用定时器, 输出PWM控制LED
  HAL_TIM_Base_Start_IT(&htim6); // 基本定时器, 产生中断, 在终端中调节PWM
  HAL_TIM_PWM_Start(&htim5, LED_R_CHANNEL);
  HAL_TIM_PWM_Start(&htim5, LED_G_CHANNEL);
  HAL_TIM_PWM_Start(&htim5, LED_B_CHANNEL);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    if (!UART_Buffer_isEmpty(&RX_BUF)) // 等待命令到来
      parse_cmd(); // 非阻塞式
    /* USER CODE END WHILE */

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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 15;
  RCC_OscInitStruct.PLL.PLLN = 216;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

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
