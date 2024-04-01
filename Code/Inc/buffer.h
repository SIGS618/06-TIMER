//
// Created by Lai on 2024/4/2.
//

#ifndef INC_06_TIMER_BUFFER_H
#define INC_06_TIMER_BUFFER_H

#include <queue>
#include "stm32f4xx_hal.h"

// 数据块类型定义
typedef struct {
    uint8_t *start; // 数据块起始地址(uint8_t 数组首地址)
    int size;       // 数据块长度
} DataBlock_TypeDef;

// 使用「类」封装 buffer, 不仅仅适用于 uart
class Buffer {
private:
    std::queue<DataBlock_TypeDef> data_blocks;
public:
    bool isEmpty();

    void pushData(const uint8_t *data, int size);

    DataBlock_TypeDef *popDataBlock();

    void delFront();
};

#endif //INC_06_TIMER_BUFFER_H
