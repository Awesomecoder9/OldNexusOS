#include "stdio.h"
#include "memory.h"
#include "stdmath.h"
#include "string.h"
#include "x86.h"

void putc(char c)
{
	x86_WriteCharTeletype(c, 0);
}

void puts(const char* str)
{
	while (*str)
	{
		putc(*str);
		str++;
	}
}


void printf(const char* fmt, ...)
{
	int* argp = (int*)(&fmt + 1);
	PrintfState state = PRINT_STATE_START;
	PrintfLength length = PRINT_LENGTH_START;
	while (*fmt)
	{
		switch (state)
		{
		case PRINT_STATE_START:
			if (*fmt == '%')
			{
				state = PRINT_STATE_LENGTH;
			}
			else
			{
				putc(*fmt);
			}
			break;
		case PRINT_STATE_LENGTH:
			if (*fmt == 'h')
			{
				state = PRINT_STATE_SHORT;
				length = PRINT_LENGTH_SHORT;
			}
			else if (*fmt == 'l')
			{
				state = PRINT_STATE_LONG;
				length = PRINT_LENGTH_LONG;
			}
			else
			{
				goto PRINT_STATE_DONE_;
			}
			break;
		case PRINT_STATE_SHORT:
			if (*fmt == 'h')
			{
				state = PRINT_STATE_DONE;
				length = PRINT_LENGTH_SHORT_SHORT;
			}
			else
			{
				goto PRINT_STATE_DONE_;
			}
			break;
		case PRINT_STATE_LONG:
			if (*fmt == 'l')
			{
				state = PRINT_STATE_DONE;
				length = PRINT_LENGTH_LONG_LONG;
			}
			else
			{
				goto PRINT_STATE_DONE_;
			}
			break;
		case PRINT_STATE_DONE:
		PRINT_STATE_DONE_:
			switch (*fmt)
			{
			case 'c':
				putc(*(char*)argp);
				argp++;
				break;
			case 's':
				if (length == PRINT_LENGTH_LONG || length == PRINT_LENGTH_LONG_LONG)
				{
					puts(*(const char**)argp);
					argp += 2;
				}
				else
				{
					puts(*(const char**)argp);
					argp++;
				}

				break;
			case 'd':
			case 'i':
				printf_number(argp, length, 10, true);
				break;
			case 'u':
				printf_number(argp, length, 10, false);
				break;
			case 'o':
				printf_number(argp, length, 8, false);
				break;
			case 'x':
			case 'X':
			case 'p':
				printf_number(argp, length, 16, false);
				break;
			case '%':
				putc('%');
				break;
			default:
				break;
			}
			break;
		}
	}
}


void printf_number(int* number, PrintfLength length, int radix, bool sign)
{
	puts(num_to_char_ptr(*number, radix));
}

char* num_to_char_ptr(int32_t num, int radix)
{

	bool sign = num < 0 ? true : false;
	if (num < 10 && !sign)
	{
		char* str = kmalloc(2);
		str[0] = (char)('0' + num);
		str[1] = '\0';
		return str;
	}
	else if (num < 10 && sign)
	{
		char* str = kmalloc(3);
		str[0] = '-';
		num = -num;
		str[1] = (char)('0' + num);
		str[2] = '\0';
		return str;
	}

	char buffer[32] = {0};
	if (sign)
	{
		buffer[0] = '-';
		num = -num;
	}
	int currentNum = num;
	int i = 1;
	while (currentNum)
	{
		buffer[i++] = (char)('0' + currentNum % radix);
		currentNum /= radix;
	}
	char* str = kmalloc(32);
	memset(str, 0, 32);
	buffer[31] = '\0';
	strrev(buffer);
	strcpy(buffer, str);
	return str;
}
