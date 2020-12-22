#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <mqueue.h>
#include <math.h>
#include <unistd.h>

#include "common.h"


int main(int argc, char **argv)
{
    /* inicjacja kolejki */
    mqd_t mq;

    /* otwarcie kolejki */
    mq = mq_open(QUEUE_NAME, O_WRONLY);
    CHECK((mqd_t)-1 != mq);

    printf("Starting producer process\n");

    Pozycja pose;

    /*Inicjacja zmiennych*/
    double kat = 0.0;
    int i = 0;
    pose.x = 0.7;
    pose.y = 0.5;
    pose.z = 0.0;
    pose.t = 0.5;
    /*Wysylanie pozycji koncowki jako sinusoidy */
    for (i; i < PROBKOWANIE; i++)
    {
        pose.y -= 0.001;
        pose.z = ((AMPLITUDA * sin(2*kat)) + PRZESUNIECIE);
        kat += ((2 * PI) / PROBKOWANIE);
        printf("y = %.3f    z = %.3f\n", pose.y, pose.z);
        CHECK(0 <= mq_send(mq, (const char*) &pose, sizeof(struct Pozycja), 10));
        usleep(pose.t*10000);

    }

    /* zamkniecie kolejki */
    CHECK((mqd_t)-1 != mq_close(mq));

    return 0;
}