#pragma once
#ifndef MEMORY_H
#define MEMORY_H

#include "stdint.h"

void memset(void* dest, uint8_t fill, uint32_t size);
void memcpy(void* src, void* dest, uint32_t size);
bool memcmp(void* ptr1, void* ptr2, size_t size);
void memmove(void* src, void* dest, uint32_t size);
void* kmalloc(size_t size);

#endif