#include "stdmath.h"

int64_t pow(int64_t num, int64_t power)
{
	int64_t result = 1;
	for (int64_t i = 1; i < power; i++)
	{
		result *= num;
	}
	return result;
}