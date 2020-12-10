#include <math.h>
#include <stdio.h>

#define PI 3.14
#define PROBKOWANIE 1000
#define PRZESUNIECIE 1.4
#define AMPLITUDA 0.3

int main(int argc, char **argv)
{
	double kat = 0.0;
	int i = 0;
	double dane[4] = {0.7, 0, 0, 0.5};
	for (i; i < PROBKOWANIE; i++)
	{
		dane[1] += 0.001;
		dane[2] = ((AMPLITUDA * sin(kat)) + PRZESUNIECIE);
		kat += ((2 * PI) / PROBKOWANIE);
		printf("y = %.3f    z = %.3f\n", dane[1], dane[2]);
	}
	getchar();
	return 0;
}
