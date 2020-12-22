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


/* Funkcja odpowiadajaca za zamkniecie semafora*/
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

/* Funkcja odpowiadajaca za otwarcie semafora*/
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
    /* Inicjacja zmiennych poczatkowych */
	Pozycja * pose;
	
	int message_Id;
	int semId;

    message_Id = shmget(2137 , sizeof(Pozycja), IPC_CREAT|0666);
    pose = (Pozycja*)shmat(message_Id,NULL,0);
    semId = semget(2137, 3, IPC_CREAT|IPC_EXCL|0600);
    semctl(semId, 0, SETVAL, (int)1); //mutex 
    semctl(semId, 1, SETVAL, (int)0); //empty czytanie z pustej
    semctl(semId, 2, SETVAL, (int)1); //pisanie do pelnej

	semId = semget(2137, 3, 0600);
    double kat = 0.0;
    int i = 0;
    pose->x = 0.7;
    pose->y = 0.5;
    pose->z = 0.0;
    pose->t = 0.5;
    for (i; i < PROBKOWANIE; i++)
    {
        /* Zamkniecie odpowiednich semaforow*/
        sem_down(semId,EMPTYID);
        sem_down(semId,MUTEXID);

        /* Zadawanie pozycji koncowki */
        pose->y -= 0.001;
        pose->z = ((AMPLITUDA * sin(2*kat)) + PRZESUNIECIE);
        kat += ((2 * PI) / PROBKOWANIE);
        printf("y = %.3f    z = %.3f\n", pose->y, pose->z);

        /* wyslanie pozycji koncowki w petli */

        sem_up(semId,MUTEXID);
        sem_up(semId,FULLID);

        usleep(pose->t*10000);
    }

    /* Czyszczenie */
    shmctl(message_Id,IPC_RMID,NULL);
    semctl(semId, 0, IPC_RMID);
    semctl(semId, 1, IPC_RMID);
    semctl(semId, 2, IPC_RMID);

    return 0;
}