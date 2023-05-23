//
// Created by Administrator on 2023/5/19.
//

#include "buffer.h"

void Uart_Buffer_Init(Uart_Buffer_TypeDef* buf)
{
  // buffer初始化
  buf->head = buf->tail = 0; // 均指向第一个数据块
  buf->cnt  = 0;
  buf->blocks[buf->tail].start = buf->data + buf->cnt;
}

int Uart_Buffer_isEmpty(Uart_Buffer_TypeDef* buf)
{
  return (buf->head == buf->tail);
}

int Uart_Buffer_isFull(Uart_Buffer_TypeDef* buf)
{
  return ((buf->tail+1)%MAX_CB_SIZE == buf->head);
}