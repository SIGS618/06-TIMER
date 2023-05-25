//
// Created by Administrator on 2023/5/19.
//

#ifndef INC_06_TIMER_BUFFER_H
#define INC_06_TIMER_BUFFER_H

#include "stm32f4xx_hal.h"

#define MAX_BUF_SIZE 64 // max buffer size
#define MAX_DB_SIZE  10 // max data blocks size
#define MAX_RX_SIZE  6  // max receive buffer size
#define MAX_TX_SIZE  40 // max transmit buffer size

typedef struct {
  uint8_t *start;
  int size;
} DataBlocks_TypeDef;

typedef struct {
  // 所有数据块的缓存位置
  uint8_t data[MAX_BUF_SIZE];
  // 每一个数据块的起始位置和大小, 方便使用memcpy()函数
  DataBlocks_TypeDef blocks[MAX_DB_SIZE];
  int head, tail; // 数据块的 head & tail
  int cnt; // 当前总共接收了多少字节
} Uart_Buffer_TypeDef;

void Uart_Buffer_Init(Uart_Buffer_TypeDef* buf);

int Uart_Buffer_isEmpty(Uart_Buffer_TypeDef* buf);

int Uart_Buffer_isFull(Uart_Buffer_TypeDef* buf);

void Uart_Buffer_Push(Uart_Buffer_TypeDef* buf, uint8_t* data, int size);

DataBlocks_TypeDef* Uart_Buffer_Pop(Uart_Buffer_TypeDef* buf);

#endif //INC_06_TIMER_BUFFER_H
