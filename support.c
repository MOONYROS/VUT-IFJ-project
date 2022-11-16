#include <stdio.h>
#include <stdlib.h>

#include "support.h"

void* safe_malloc(size_t size)
{
	char* ptr = malloc(size);
	if (ptr == NULL)
	{
		printf("FATAL ERROR: memory allocation failed");
		exit(-1);
	}
	else
	{
		for (int i=0; i < size; i++)
			*(ptr + i) = 0;
	}
	return ptr;
}