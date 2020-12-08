#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <mqueue.h>

#include "common.h"


int main(int argc, char **argv)
{
    /* init mail queue */
    mqd_t mq;

    /* open the mail queue */
    mq = mq_open(QUEUE_NAME, O_WRONLY);
    CHECK((mqd_t)-1 != mq);

    printf("Send to server (enter \"exit\" to stop it):\n");

    Pozycja pose;

    do {
        printf("> ");
        fflush(stdout);

        memset(pose.pozycja, 0, MAX_SIZE);
        fgets(pose.pozycja, MAX_SIZE, stdin);

        /* send the message */
        CHECK(0 <= mq_send(mq, (const char*) &pose, sizeof(Pozycja), 0));

    } while (strncmp(pose.pozycja, MSG_STOP, strlen(MSG_STOP)));

    /* cleanup */
    CHECK((mqd_t)-1 != mq_close(mq));

    return 0;
}