#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <mqueue.h>

#include "common.h"

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

    /* create the message queue */
    mq = mq_open(QUEUE_NAME, O_CREAT | O_RDONLY, 0644, &attr);
    CHECK((mqd_t)-1 != mq);
    Pozycja pose;

    do {
        ssize_t bytes_read;

        /* receive the message */
        bytes_read = mq_receive(mq, (char *) &pose, sizeof(Pozycja), NULL);
        CHECK(bytes_read >= 0);

        if (! strncmp(pose.pozycja, MSG_STOP, strlen(MSG_STOP)))
        {
            must_stop = 1;
        }
        else
        {
            printf("Received: %s\n", pose.pozycja);
        }
    } while (!must_stop);

    /* cleanup */
    CHECK((mqd_t)-1 != mq_close(mq));
    CHECK((mqd_t)-1 != mq_unlink(QUEUE_NAME));

    return 0;
}