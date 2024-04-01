//
// Created by Lai on 2024/4/2.
//

#include <cstring>
#include "buffer.h"

bool Buffer::isEmpty() {
    return data_blocks.empty();
}

void Buffer::pushData(const uint8_t *data, int size) {
    __disable_irq();
    // 1. 声明并初始化新的数据块
    DataBlock_TypeDef new_db;
    new_db.start = new uint8_t[size];
    new_db.size = size;

    // 2. 拷贝数组
    memcpy(new_db.start, data, size);

    // 3. 将数据块添加到队列尾
    data_blocks.push(new_db);
    __enable_irq();
}

DataBlock_TypeDef *Buffer::popDataBlock() {
    return &data_blocks.front();
}

void Buffer::delFront() {
    delete data_blocks.front().start; // 释放内存
    data_blocks.pop();
}


