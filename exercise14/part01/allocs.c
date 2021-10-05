#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <time.h>

int main(int argc, char *argv[])
{
	size_t n = 1024;
	char *ptr_m;
	char *ptr_c;
	char *ptr_a;
	clock_t s, e;
	int t = atoi(argv[1]);
	char *desc[] = { "malloc", "calloc", "alloca" };
	printf("Testing %s\n", desc[t - 1]);

	while (1) {
		s = clock();
		switch (t) {
		case 1:
			ptr_m = (char *)malloc(n);
			ptr_m[n - 1] = 1;
			break;
		case 2:
			ptr_c = (char *)calloc(1, n);
			ptr_c[n - 1] = 1;
			break;
		case 3:
			ptr_a = (char *)alloca(n);
			ptr_a[n - 1] = 1;
			break;
		default:
			break;
		}
		e = clock();
		if (t == 1)
			free(ptr_m);
		if (t == 2)
			free(ptr_c);
		n <<= 1;
		printf("Passed %llu, n = %zu\n", e - s, n);
	}
	return 0;
}