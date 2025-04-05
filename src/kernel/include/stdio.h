#include "stdint.h"
typedef enum
{
	PRINT_STATE_START,
	PRINT_STATE_LENGTH,
	PRINT_STATE_DONE,
	PRINT_STATE_SHORT,
	PRINT_STATE_LONG,
} PrintfState;

typedef enum
{
	PRINT_LENGTH_START,
	PRINT_LENGTH_SHORT,
	PRINT_LENGTH_SHORT_SHORT,
	PRINT_LENGTH_LONG,
	PRINT_LENGTH_LONG_LONG,
} PrintfLength;


void putc(char c);
void puts(const char* str);
void printf(const char* fmt, ...);
void printf_number(int* number, PrintfLength length, int radix, bool sign);
char* num_to_char_ptr(int32_t num, int radix);