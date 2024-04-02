//
// Created by Lai on 2024/4/2.
//
#include "uart_buffer.h"
#include "uart_buffer_interface.h"

extern "C" {

void *UART_Buffer_Create() {
    return new UART_Buffer();
}

int UART_Buffer_isEmpty(void *instance) {
    auto *buffer = static_cast<UART_Buffer *>(instance);
    if (buffer->isEmpty())
        return 1;
    else
        return 0;
}

void UART_Buffer_pushBytes(void *instance, const uint8_t *data, uint16_t size) {
    auto *buffer = static_cast<UART_Buffer *>(instance);
    buffer->pushBytes(data, size);
}

DataBlock_TypeDef *UART_Buffer_front(void *instance) {
    auto *buffer = static_cast<UART_Buffer *>(instance);
    return buffer->front();
}

void UART_Buffer_pop(void *instance) {
    auto *buffer = static_cast<UART_Buffer *>(instance);
    buffer->pop();
}

void UART_Buffer_Destroy(void *instance) {
    auto *buffer = static_cast<UART_Buffer *>(instance);
    delete buffer;
}

}
