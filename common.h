#ifndef COMMON_H_
#define COMMON_H_

#define QUEUE_NAME  "/test_queue"
#define MAX_SIZE    1024
#define MSG_STOP    "exit"

#define PI 3.14
#define PROBKOWANIE 1000
#define PRZESUNIECIE 0.3535
#define AMPLITUDA 0.15

#define CHECK(x) \
    do { \
        if (!(x)) { \
            fprintf(stderr, "%s:%d: ", __func__, __LINE__); \
            perror(#x); \
            exit(-1); \
        } \
    } while (0) \

    /* init struct Pozycja */
typedef struct Pozycja
{
  long czasDostarczenia;
  double x;
  double y;
  double z;
  double t;
} Pozycja;


#endif /* #ifndef COMMON_H_ */