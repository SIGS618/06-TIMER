//
// Created by Lai on 2024/4/2.
//

#ifndef INC_06_TIMER_UART_BUFFER_INTERFACE_H
#define INC_06_TIMER_UART_BUFFER_INTERFACE_H

#include "common_types.h"

#ifdef __cplusplus
extern "C" {
#endif

// 创建UART_Buffer类实例
void *UART_Buffer_Create();

// 检查缓冲区是否为空
int UART_Buffer_isEmpty(void *instance);

// 向缓冲区中推送字节
void UART_Buffer_pushBytes(void *instance, const uint8_t *data, uint16_t size);

// 获取缓冲区首部的数据块
DataBlock_TypeDef *UART_Buffer_front(void *instance);

// 从缓冲区中弹出首部的数据块
void UART_Buffer_pop(void *instance);

// 删除UART_Buffer类实例
void UART_Buffer_Destroy(void *instance);

#ifdef __cplusplus
}
#endif

#endif //INC_06_TIMER_UART_BUFFER_INTERFACE_H
