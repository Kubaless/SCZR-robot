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
    /* init mail queue */
    mqd_t mq;

    /* open the mail queue */
    mq = mq_open(QUEUE_NAME, O_WRONLY);
    CHECK((mqd_t)-1 != mq);

    printf("Starting producer process\n");

    Pozycja pose;

    double kat = 0.0;
    int i = 0;
    pose.x = 0.7;
    pose.y = 0.5;
    pose.z = 0.0;
    pose.t = 0.5;
    for (i; i < PROBKOWANIE; i++)
    {
        pose.y -= 0.001;
        pose.z = ((AMPLITUDA * sin(kat)) + PRZESUNIECIE);
        kat += ((2 * PI) / PROBKOWANIE);
        printf("y = %.3f    z = %.3f\n", pose.y, pose.z);
        CHECK(0 <= mq_send(mq, (const char*) &pose, sizeof(struct Pozycja), 10));
        usleep(pose.t*10000);

    }

/*    do {
        printf("> ");
        fflush(stdout);

        memset(pose.pozycja, 0, MAX_SIZE);
        fgets(pose.pozycja, MAX_SIZE, stdin);
        pose.x = 2;

        /* send the message 
        CHECK(0 <= mq_send(mq, (const char*) &pose, sizeof(struct Pozycja), 0));

    } while (strncmp(pose.pozycja, MSG_STOP, strlen(MSG_STOP)));
*/
    /* cleanup */
    CHECK((mqd_t)-1 != mq_close(mq));

    return 0;
}