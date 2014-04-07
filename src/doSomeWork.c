/*
 * Calculates prime numbers.
 */

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <limits.h>

#define COUNT 100000

int main(int argc, char **argv)
{
	int i = 0;
	// Numbers from 0 to COUNT
	int array_isPrim[COUNT];
	
	while (1) {

	for (i = 2; i < COUNT; ++i)
		array_isPrim[i] = 1;
	
	for (i = 2; i <= sqrt(COUNT); ++i) {
		if (array_isPrim[i]) {
			int j = 0;
			// All multiples are not prime numbers
			for (j = i*i; j < COUNT; j += i)
				array_isPrim[j] = 0;
		}
	}
	
	if (argc == 2 && (strcmp(argv[1], "-v") == 0)) {
		printf("Prime numbers from 2 to %d:\n", COUNT);
		for (i = 2; i < COUNT; ++i) {
			if (array_isPrim[i])
				printf("%d ", i);
		}
		printf("\n");
	}
	}
	
	return 0;
}
