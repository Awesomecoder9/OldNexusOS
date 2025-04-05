#include "string.h"
#include "memory.h"

void strcpy(char* src, char* dest)
{
	memcpy(src, dest, strlen(src) + 1);
}

char* strcat(char* src, char* dest)
{
	char* nullChar = strchr(src, '\0');
	if (nullChar == NULL)
		return NULL;
	size_t totalLen = strlen(src) + strlen(dest);

	for (uint32_t i = 0; i < totalLen; i++)
	{
		nullChar[i] = dest[i];
	}


	*nullChar = '\0';
	return src;
}

char* strchr(char* str, char c)
{
	for (int i = 0; i < (strlen(str) + 1); i++)
	{
		if (str[i] == c)
			return str[i];
	}

	return NULL;
}

bool strcmp(char* str1, char* str2)
{
	int len1 = strlen(str1);
	int len2 = strlen(str2);
	if (len1 != len2)
		return false;

	return memcmp(str1, str2, len1);
}

char* strrev(char* str)
{
	int len = strlen(str);
	char temp;
	int k = len;
	for (int i = 0; i < len; i++)
	{
		temp = str[k];
		str[k] = str[i];
		str[i] = temp;
		k--;
		if (k == len / 2)
			break;
	}

	return str;
}

size_t strlen(char* str)
{
	size_t count = 0;
	while (str[count])
	{
		count++;
	}
	return count;
}
