
#include <pthread.h>
#include <deque>
#include <fstream>
#include <iostream>
#include <string>
#ifndef WRITER
#define WRITER
#include "reader.h"

extern double totalLockTimeWriter;
extern double totalWriteTime;
extern double totalWriteOrderLockTime;
extern int writeCount;
extern int NUM_WRITERS;
extern  pthread_mutex_t writeOrderMutex;
extern pthread_mutex_t outputFileMutex;
extern int writeOrder;


class writer {
    public:
    static void* runner(void* arg);
   private:
    static std::ofstream out;
};
#endif
