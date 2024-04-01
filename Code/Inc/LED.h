//
// Created by Administrator on 2023/5/18.
//

#ifndef INC_06_TIMER_LED_H
#define INC_06_TIMER_LED_H

#include "stm32f4xx_hal.h"

typedef struct {
    uint8_t R;
    uint8_t G;
    uint8_t B;
} RGB;

extern RGB rgb_seq_p1[];
extern int len_seq_p1;

extern RGB rgb_seq_p2[];
extern int len_seq_p2;

#endif //INC_06_TIMER_LED_H
