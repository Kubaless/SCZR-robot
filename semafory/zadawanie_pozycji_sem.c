#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <unistd.h>
#include <math.h>
#include "../common.h"

#define EMPTYID 1
#define MUTEXID 0
#define FULLID 2


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

int main()
{
	Pozycja * pose;
	
	int message_Id;
	int semId;
	//pamiec
	message_Id = shmget(2137 , sizeof(Pozycja), 0666);
	pose = (Pozycja*)shmat(message_Id,NULL,0);
	//semafor
	semId = semget(2137, 3, 0600);
    double kat = 0.0;
    int i = 0;
    //pose->x = 0.7;
    //pose->y = 0.5;
    //pose->z = 0.0;
   //pose->t = 0.5;
    for (i; i < PROBKOWANIE; i++)
    {
        sem_down(semId,EMPTYID);
        sem_down(semId,MUTEXID);

        pose->x = 0.7;
        pose->y -= 0.001;
        pose->z = ((AMPLITUDA * sin(2*kat)) + PRZESUNIECIE);
        pose->t = 0.5;
        kat += ((2 * PI) / PROBKOWANIE);
        printf("y = %.3f    z = %.3f\n", pose->y, pose->z);

        //tu wysylanie

        sem_up(semId,MUTEXID);
        sem_up(semId,FULLID);

        usleep(pose->t*10000);
    }
return 0;
}