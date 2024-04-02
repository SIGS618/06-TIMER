//
// Created by Lai on 2024/4/2.
//

#ifndef INC_06_TIMER_COMMON_TYPES_H
#define INC_06_TIMER_COMMON_TYPES_H

#include "stm32f4xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

// 数据块类型定义
typedef struct {
    uint8_t *start; // 数据块起始地址
    int size;       // 数据块长度
} DataBlock_TypeDef;

#ifdef __cplusplus
}
#endif


#endif //INC_06_TIMER_COMMON_TYPES_H
