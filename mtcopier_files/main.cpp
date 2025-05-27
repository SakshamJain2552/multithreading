#include <chrono>
#include <iostream>
#include <fstream>
#include <queue>
#include <vector>
#include <pthread.h>
#include <cstdlib>
#include "reader.h"
#include "writer.h"

// Global variables to store timing and count information for readers and writers
double totalLockTimeReader = 0.0;
double totalReadTime = 0.0;
int readCount = 0;
int writeCount = 0;

// Global variables to store timing
double totalLockTimeWriter = 0.0;
double totalWriteTime = 0.0;
double totalWriteOrderLockTime = 0.0;

// Number of reader and writer threads
int NUM_READERS;
int NUM_WRITERS;

// Mutex for controlling the order of writing
pthread_mutex_t writeOrderMutex = PTHREAD_MUTEX_INITIALIZER;

// Shared queue between readers and writers
std::queue<QueueItem> sharedQueue;

// Mutex for controlling access to the shared queue
pthread_mutex_t queueMutex = PTHREAD_MUTEX_INITIALIZER;

// Condition variables for notifying when data is available in the queue or when space is available
pthread_cond_t dataAvailable = PTHREAD_COND_INITIALIZER;
pthread_cond_t spaceAvailable = PTHREAD_COND_INITIALIZER;

// Mutex for controlling access to the output file
pthread_mutex_t outputFileMutex = PTHREAD_MUTEX_INITIALIZER;

// Flags and counters for reading and writing order
bool isReadingComplete = false;
int readOrder = 0;
int writeOrder = 0;

// Input and output file streams
std::ifstream inputFile;
std::ofstream outputFile;

// Global variable to check if timing is enabled
bool timingEnabled = false;

// Cleanup function to destroy mutexes, condition variables, and close files
void cleanup() {
    pthread_mutex_destroy(&queueMutex);
    pthread_cond_destroy(&dataAvailable);
    pthread_cond_destroy(&spaceAvailable);
    pthread_mutex_destroy(&outputFileMutex);

    if (inputFile.is_open()) {
        inputFile.close();
    }

    if (outputFile.is_open()) {
        outputFile.close();
    }
}

int main(int argc, char* argv[]) {
    // Check for correct number of command-line arguments
    if (argc < 4 || argc > 5) {
        std::cerr << "Usage: " << argv[0] << " numthreads infile outfile [-t]" << std::endl;
        return EXIT_FAILURE;
    }

    // Check if timing flag is provided
    if (argc == 5 && std::string(argv[4]) == "-t") {
        timingEnabled = true;
    } else if (argc == 5) {
        std::cerr << "Invalid option. Use -t for timing." << std::endl;
        return EXIT_FAILURE;
    }

    // Parse the number of threads from command-line arguments
    int numThreads;
    try {
        numThreads = std::stoi(argv[1]);
        NUM_READERS = numThreads;
        NUM_WRITERS = numThreads;
    }
    catch (const std::exception &e) {
        std::cerr << "Error: Invalid number of threads provided." << std::endl;
        return EXIT_FAILURE;
    }

    // Validate the number of threads
    if (numThreads < 25 || numThreads > 100 ) {
        std::cerr << "Error: Number of threads must be a value between 25-100." << std::endl;
        return EXIT_FAILURE;
    }

    // Open the input file and measure the time taken
    auto fileOpenStart = std::chrono::high_resolution_clock::now();
    const char* inputFileName = argv[2];
    inputFile.open(inputFileName);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Unable to open input file: " << inputFileName << std::endl;
        return EXIT_FAILURE;
    }
    auto fileOpenEnd = std::chrono::high_resolution_clock::now();

    // Open the output file
    const char* outputFileName = argv[3];
    outputFile.open(outputFileName);
    if (!outputFile.is_open()) {
        std::cerr << "Error: Unable to open output file: " << outputFileName << std::endl;
        inputFile.close(); // Close the input file before exiting
        return EXIT_FAILURE;
    }

    // Register cleanup function to be called on exit
    atexit(cleanup);

    // Start timing the entire program execution
    auto start = std::chrono::high_resolution_clock::now(); // Start time

    // Create reader and writer threads
    std::vector<pthread_t> readers(numThreads);
    std::vector<pthread_t> writers(numThreads);

    auto threadCreationStart = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < numThreads; i++) {
        pthread_create(&readers[i], nullptr, reader::runner, &inputFile);
    }

    for (int i = 0; i < numThreads; i++) {
        pthread_create(&writers[i], nullptr, writer::runner, &outputFile);
    }
    auto threadCreationEnd = std::chrono::high_resolution_clock::now();

    auto threadJoinStart = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < numThreads; i++) {
        pthread_join(readers[i], nullptr);
    }

    for (int i = 0; i < numThreads; i++) {
        pthread_join(writers[i], nullptr);
    }
    auto threadJoinEnd = std::chrono::high_resolution_clock::now();

    // End timing the entire program execution
    auto end = std::chrono::high_resolution_clock::now(); // End time

    // If timing is enabled, print out the timing information
    if (timingEnabled) {
        std::cout << "File opening took: " << std::chrono::duration<double>(fileOpenEnd - fileOpenStart).count() << " seconds." << std::endl;
        std::cout << "Thread creation took: " << std::chrono::duration<double>(threadCreationEnd - threadCreationStart).count() << " seconds." << std::endl;
        std::cout << "Thread joining took: " << std::chrono::duration<double>(threadJoinEnd - threadJoinStart).count() << " seconds." << std::endl;
        std::cout << "Total program execution took: " << std::chrono::duration<double>(end - start).count() << " seconds." << std::endl;

        std::cout << "Average time to gain lock (reader): " << totalLockTimeReader / readCount << " seconds." << std::endl;
        std::cout << "Average time to read from file: " << totalReadTime / readCount << " seconds." << std::endl;

        std::cout << "Average time to gain lock (writer): " << totalLockTimeWriter / writeCount << " seconds." << std::endl;
        std::cout << "Average time to write to file: " << totalWriteTime / writeCount << " seconds." << std::endl;
        std::cout << "Average time to acquire the write order mutex: " << totalWriteOrderLockTime / writeCount << " seconds." << std::endl;
    }

    return EXIT_SUCCESS;
}