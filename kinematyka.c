#include <math.h>
#include <stdio.h>

#define PI 3.14

int main(int argc, char **argv)
{
    double dane[4] = {0.7, 0.3, 1.4, 0.5};
	double poprzednia_pozycja[3] = {0, 0, 0};
	double czlon1, czlon2, dlugosc, alfa1, alfa2, alfa3, j1, j2, j3;
	double uchyb[3] = {0, 0, 0};
	double predkosc[3] = {0, 0, 0};

	czlon1 = 0.5;
	czlon2 = 0.5;
	dlugosc = sqrt((dane[0] * dane[0]) + (dane[1] * dane[1]));
	alfa1 = atan2(dane[1], dane[0]);
	alfa2 = acos(((czlon1 * czlon1) + (dlugosc * dlugosc) - (czlon2 * czlon2)) / (2 * czlon1 * dlugosc));
	alfa3 = acos(((czlon1 * czlon1) - (dlugosc * dlugosc) + (czlon2 * czlon2)) / (2 * czlon1 * czlon2));
	j1 = alfa1 + alfa2;
	j2 = alfa3 - PI;
	j3 = dane[2] - 1.4;
	uchyb[0] = (j1 - poprzednia_pozycja[0]);
	uchyb[1] = (j2 - poprzednia_pozycja[1]);
	uchyb[2] = (j3 - poprzednia_pozycja[2]);
	predkosc[0] = (uchyb[0] / dane[3]);
	predkosc[1] = (uchyb[1] / dane[3]);
	predkosc[2] = (uchyb[2] / dane[3]);
    printf("%.3f, %.3f, %.3f", predkosc[0], predkosc[1], predkosc[2]);
    return 0;
}
