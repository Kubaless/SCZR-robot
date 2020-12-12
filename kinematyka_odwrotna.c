#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <mqueue.h>
#include <math.h>
#include <unistd.h>

#include "common.h"

FILE * otworzPotokGnuplota() {
    FILE * g_potok = popen( "gnuplot -persistent", "w" ); /* otwarcie potoku do zapisu */
    return g_potok;
}



int main(int argc, char **argv)
{
    /* delete messages with the same name */
    if(mq_unlink(QUEUE_NAME) == 0)
        fprintf(stdout, "Message queue %s removed from system.\n", QUEUE_NAME);

    mqd_t mq;
    struct mq_attr attr;
    int must_stop = 0;

    /* initialize the queue attributes */
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(Pozycja);
    attr.mq_curmsgs = 0;

    /* creating starting parameters */
    char * commandsForGnuplot[] = {"set title \"pozycja koncowki\"", "splot 'data.temp'"};
    double poprzednia_pozycja[3] = {0, 0, 0};
    double czlon1, czlon2, dlugosc, alfa1, alfa2, alfa3, j1, j2, j3;
    double uchyb[3] = {0, 0, 0};
    double predkosc[3] = {0, 0, 0};
    czlon1 = 0.5;
    czlon2 = 0.5;

    /* create the message queue */
    mq = mq_open(QUEUE_NAME, O_CREAT | O_RDONLY, 0644, &attr);
    CHECK((mqd_t)-1 != mq);

    FILE * gnuplotPipe = otworzPotokGnuplota();
    FILE * temp = fopen("data.temp", "w");
    Pozycja pose;
    Params par;

    do {
        ssize_t bytes_read;

        /* receive the message */
        bytes_read = mq_receive(mq, (char *) &pose, sizeof(struct Pozycja), NULL);
        CHECK(bytes_read >= 0);

        if (pose.y <= -0.49999)
        {
            must_stop = 1;
        }
        else
        {
            printf("Otrzymano: %.3f , %.3f, %.3f , %.3f ", pose.x, pose.y, pose.z, pose.t );
            fprintf(temp, "%f %f %f \n", pose.x, pose.y, pose.z); //Write the data to a temporary file
            dlugosc = sqrt((pose.x * pose.x) + (pose.y * pose.y));
            alfa1 = atan2(pose.y, pose.x);
            alfa2 = acos(((czlon1 * czlon1) + (dlugosc * dlugosc) - (czlon2 * czlon2)) / (2 * czlon1 * dlugosc));
            alfa3 = acos(((czlon1 * czlon1) - (dlugosc * dlugosc) + (czlon2 * czlon2)) / (2 * czlon1 * czlon2));
            j1 = alfa1 + alfa2;
            j2 = alfa3 - PI;
            j3 = pose.z - 1.4;
            uchyb[0] = (j1 - poprzednia_pozycja[0]);
            uchyb[1] = (j2 - poprzednia_pozycja[1]);
            uchyb[2] = (j3 - poprzednia_pozycja[2]);
            predkosc[0] = (uchyb[0] / pose.t);
            predkosc[1] = (uchyb[1] / pose.t);
            predkosc[2] = (uchyb[2] / pose.t);
            printf("alfy: 1 %.3f, 2 %.3f, 3 %.3f ", alfa1, alfa2, alfa3);
            printf("predkosci: %.3f, %.3f, %.3f ", predkosc[0], predkosc[1], predkosc[2]);
            printf("uchyby: %.3f, %.3f, %.3f ", uchyb[0], uchyb[1], uchyb[2]);
            printf("parametry: j1 %.3f, j2 %.3f, j3 %.3f \n", j1, j2, j3);
            poprzednia_pozycja[0] = pose.x;
            poprzednia_pozycja[1] = pose.y;
            poprzednia_pozycja[2] = pose.z;
            fprintf(gnuplotPipe, "%s \n", commandsForGnuplot[0]); 
            fprintf(gnuplotPipe, "%s \n", commandsForGnuplot[1]); //Send commands to gnuplot one by one.

        }
    } while (!must_stop);

    /* cleanup */
    CHECK((mqd_t)-1 != mq_close(mq));
    CHECK((mqd_t)-1 != mq_unlink(QUEUE_NAME));
    return 0;
}