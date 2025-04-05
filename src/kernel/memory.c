#include "memory.h"

void memset(void* dest, uint8_t fill, uint32_t size)
{
	for (uint32_t i = 0; i < size; i++)
	{
		((char*)dest)[i] = fill;
	}
}

void memcpy(void* src, void* dest, uint32_t size)
{
	for (uint32_t i = 0; i < size; i++)
	{
		((char*)dest)[i] = ((char*)src)[i];
	}
}

bool memcmp(void* ptr1, void* ptr2, size_t size)
{
	for (uint32_t i = 0; i < size; i++)
	{
		if (((char*)ptr1)[i] != ((char*)ptr2)[i])
			return false;
	}

	return true;
}

void memmove(void* src, void* dest, uint32_t size)
{
	memcpy(src, dest, size);
	memset(src, 0, size);
}
