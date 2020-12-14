#include <stdio.h>
#include <stdint.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include "../common.h"
#define NUM_COMMANDS 7

// semafor --
void sem_down(int semid, int semnum)
{
    struct sembuf buf;
    buf.sem_num = semnum;
    buf.sem_op = -1;
    buf.sem_flg = 0;
    if (semop(semid, &buf, 1) == -1)
    {
        printf("SEM DOWN ERROR");
        exit(-1);
    }
}

// semafor ++
void sem_up(int semid, int semnum)
{
    struct sembuf buf;
    buf.sem_num = semnum;
    buf.sem_op = 1;
    buf.sem_flg = 0;
    if (semop(semid, &buf, 1) == -1)
    {
        printf("SEM UP ERROR");
        exit(-1);
    }
}

FILE * otworzPotokGnuplota() 
{
    FILE * g_potok = popen( "gnuplot -persistent", "w" ); /* otwarcie potoku do zapisu */
    return g_potok;
}

#define EMPTYID 1
#define MUTEXID 0
#define FULLID 2

int main(int argc,char * argv)
{
	/* creating starting parameters */
    // !!! przy zmianie wyswietlania nalezy zmienic odpowiadajaca ilosc komend NUM_COMANDS dla plaszczyzn NUM_COMANDS = 5!!!

    //char * commandsForGnuplot[] = {"set title \"Wizualizacja robota w osi yz\"","set xrange [-2:2]","set yrange [-2:2]","set zrange [0:2]", "plot 'data.temp' using 2:3:5:6 with vectors nohead lw 2"};
    char * commandsForGnuplot[] = {"set title \"Wizualizacja robota 3D\"","set xrange [-1:1]","set yrange [-1:1]","set zrange [0:1]","set xyplane 0","set grid" , "splot 'data.temp' using 1:2:3:4:5:6 with vectors nohead lw 2"};
    //char * commandsForGnuplot[] = {"set title \"Wizualizacja robota w osi xz\"","set xrange [-2:2]","set yrange [-2:2]","set zrange [0:2]", "plot 'data.temp' using 1:3:4:6 with vectors nohead lw 2"};
    //char * commandsForGnuplot[] = {"set title \"Wizualizacja robota w osi xy\"","set xrange [-2:2]","set yrange [-2:2]","set zrange [0:2]", "plot 'data.temp' using 1:2:4:5 with vectors nohead lw 2"};
    //char * commandsForGnuplot[] = {"set term wxt title 'Gazebo'","set title \"Wizualizacja robota 3D\"","set xrange [-2:2]","set yrange [-2:2]","set zrange [0:1]","set xyplane 0","set grid" , "splot 'data.temp' using 1:2:3:4:5:6 with vectors nohead lw 2"};

    double poprzednia_pozycja[3] = {0, 0, 0};
    double czlon1, czlon2, dlugosc, alfa1, alfa2, alfa3, j1, j2, j3;
    double uchyb[3] = {0, 0, 0};
    double predkosc[3] = {0, 0, 0};
    double joint0[3] = {0, 0, 0};
    double joint1[3] = {0, 0, 0};
    double joint2[3] = {0, 0, 0};
    double joint3[3] = {0, 0, 0};
    double joint4[3] = {0, 0, 0};
    
    czlon1 = 0.5;
    czlon2 = 0.5;

    FILE * gnuplotPipe = otworzPotokGnuplota();
	Pozycja * pose;
	int message_Id;
	int semId;
	//pamiec
	message_Id = shmget(2137 , sizeof(Pozycja), 0666);
	pose = (Pozycja*)shmat(message_Id,NULL,0);
	//semafor
	semId = semget(2137, 3, 0600);
	int i =0;
	int must_stop = 0;

	do {
        sem_down(semId,FULLID);
		sem_down(semId,MUTEXID);

        if (pose->y <= -0.49999)
        {
            must_stop = 1;
        }
        else
        {
            FILE * temp = fopen("data.temp", "w");
            printf("Otrzymano: %.3f , %.3f, %.3f , %.3f \n", pose->x, pose->y, pose->z, pose->t );
            //fprintf(temp, "%f %f %f \n", pose->x, pose->y, pose->z); //Write the data to a temporary file
            dlugosc = sqrt((pose->x * pose->x) + (pose->y * pose->y));
            alfa1 = atan2(pose->y, pose->x);
            alfa2 = acos(((czlon1 * czlon1) + (dlugosc * dlugosc) - (czlon2 * czlon2)) / (2 * czlon1 * dlugosc));
            alfa3 = acos(((czlon1 * czlon1) - (dlugosc * dlugosc) + (czlon2 * czlon2)) / (2 * czlon1 * czlon2));
            j1 = alfa1 + alfa2;
            j2 = alfa3 - PI;
            j3 = pose->z - PRZESUNIECIE;
            uchyb[0] = (j1 - poprzednia_pozycja[0]);
            uchyb[1] = (j2 - poprzednia_pozycja[1]);
            uchyb[2] = (j3 - poprzednia_pozycja[2]);
            predkosc[0] = (uchyb[0] / pose->t);
            predkosc[1] = (uchyb[1] / pose->t);
            predkosc[2] = (uchyb[2] / pose->t);
            printf("alfy: 1 %.3f, 2 %.3f, 3 %.3f \n", alfa1, alfa2, alfa3);
            printf("predkosci: %.3f, %.3f, %.3f \n", predkosc[0], predkosc[1], predkosc[2]);
            printf("uchyby: %.3f, %.3f, %.3f \n", uchyb[0], uchyb[1], uchyb[2]);
            printf("parametry: j1 %.3f, j2 %.3f, j3 %.3f \n", j1, j2, j3);
            poprzednia_pozycja[0] = j1;
            poprzednia_pozycja[1] = j2;
            poprzednia_pozycja[2] = j3;
            joint1[0]=czlon1*cos(j1);
            joint1[1]=czlon1*sin(j1);
            joint2[0]=czlon1*cos(j1)+czlon2*cos(j1+j2);
            joint2[1]=czlon1*sin(j1)+czlon2*sin(j1+j2);
            joint2[2]=czlon2/sqrt(2);
            joint3[0]=czlon1*cos(j1)+czlon2*cos(j1+j2);
            joint3[1]=czlon1*sin(j1)+czlon2*sin(j1+j2);
            joint3[2]=joint2[2]+j3-0.15;
            joint4[0]=0;
            joint4[1]=0;
            joint4[2]=0.3;
            printf("Koncowka: %.3f, %.3f, %.3f \n\n", joint3[0],joint3[1],joint3[2]+0.15);
            //joint3[0]=pose->x;
            //joint3[1]=pose->y;
            //joint3[2]=pose->z;
            //joint4[0]=pose->x;
            //joint4[1]=0;
            //joint4[2]=0.4;

            fprintf(temp, "%f %f %f %f %f %f\n%f %f %f %f %f %f\n%f %f %f %f %f %f\n", joint0[0], joint0[1],joint0[2], joint1[0], joint1[1], joint1[2], joint1[0], joint1[1], joint1[2], joint2[0]-joint1[0], joint2[1]-joint1[1], joint2[2]-joint1[2], joint3[0], joint3[1], joint3[2], joint4[0], joint4[1], joint4[2]); //Write data to data file
            fclose(temp);

            for (int i=0; i < NUM_COMMANDS; i++)
            {
                fprintf(gnuplotPipe, "%s \n", commandsForGnuplot[i]); //Send commands to gnuplot one by one.
            }

            sem_up(semId,MUTEXID);
			sem_up(semId,EMPTYID);

        }
    } while (!must_stop);

return 0;

}