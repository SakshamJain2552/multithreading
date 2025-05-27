#include <pthread.h>
#include <fstream>
#include <iostream>
#include <string>
#include <queue>
#include <chrono>
#include "writer.h"
#ifndef READER
#define READER

struct QueueItem {
    std::string line;
    int order;
};

extern double totalLockTimeReader;
extern double totalReadTime;
extern int readCount;
extern bool timingEnabled;
extern int NUM_READERS;
extern  std::queue<QueueItem> sharedQueue;
extern pthread_mutex_t queueMutex;
extern pthread_cond_t dataAvailable;
extern pthread_cond_t spaceAvailable;
extern bool isReadingComplete;
extern int readOrder;
extern std::ifstream inputFile;
extern std::ofstream outputFile;


class reader {
   public:
    static void* runner(void* arg);
private:
    static std::ifstream in;
};
#endif
