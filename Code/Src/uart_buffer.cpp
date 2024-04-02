//
// Created by Lai on 2024/4/2.
//

#include <cstring>
#include "uart_buffer.h"

bool UART_Buffer::isEmpty() {
    return buffer.empty();
}

void UART_Buffer::pushBytes(const uint8_t *data, uint16_t size) {
    __disable_irq();
    // 1. 声明并初始化新的数据块
    DataBlock_TypeDef new_db;
    new_db.start = new uint8_t[size];
    new_db.size = size;

    // 2. 拷贝数组
    memcpy(new_db.start, data, size);

    // 3. 将数据块添加到队列尾
    buffer.push(new_db);
    __enable_irq();
}

DataBlock_TypeDef *UART_Buffer::front() {
    return &buffer.front();
}

void UART_Buffer::pop() {
    delete[] buffer.front().start; // 释放内存
    buffer.pop();
}
