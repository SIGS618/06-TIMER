//
// Created by Administrator on 2023/5/25.
//

#include "uart.h"

Buffer TX_BUF, RX_BUF;

void UART_Transmit_IT(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size) {
    if (huart->gState == HAL_UART_STATE_READY)
        HAL_UART_Transmit_IT(huart, pData, Size);
    else // huart->gState == HAL_UART_STATE_BUSY
        TX_BUF.pushData(pData, Size);
}