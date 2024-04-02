//
// Created by Lai on 2024/4/2.
//

#ifndef INC_06_TIMER_UART_BUFFER_H
#define INC_06_TIMER_UART_BUFFER_H

#include <queue>
#include "stm32f4xx_hal.h"
#include "common_types.h"

typedef std::queue<DataBlock_TypeDef> qDataBlocks;

class UART_Buffer {
private:
    qDataBlocks buffer;
public:
    bool isEmpty();

    void pushBytes(const uint8_t *data, uint16_t size);

    DataBlock_TypeDef *front();

    void pop();
};

#endif //INC_06_TIMER_UART_BUFFER_H
