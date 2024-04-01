//
// Created by Administrator on 2023/5/25.
//

#ifndef INC_06_TIMER_UART_H
#define INC_06_TIMER_UART_H

#include "stm32f4xx_hal.h"
#include "buffer.h"

extern Buffer TX_BUF, RX_BUF;

// TODO: 完善带 buffer 的串口数据接受
void UART_Receive_IT(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size);
// TODO: 调整为 DMA 模式
void UART_Transmit_IT(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size);

#endif //INC_06_TIMER_UART_H
