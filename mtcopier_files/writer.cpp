#include "writer.h"

// Function for the writer thread
void* writer::runner(void* arg) {

    // Cast the passed argument to an ofstream pointer
    std::ofstream* outputFile = static_cast<std::ofstream*>(arg);

    // Continuously write to the output file until there's no more data to write
    while (true) {
        QueueItem item;

        // Start timing the duration to acquire the queue mutex
        auto lockStart = std::chrono::high_resolution_clock::now();

        // Lock the mutex to ensure exclusive access to the shared queue
        pthread_mutex_lock(&queueMutex);

        auto lockEnd = std::chrono::high_resolution_clock::now();

        // Wait until there's data in the queue and it's this thread's turn to write
        while (sharedQueue.empty() || sharedQueue.front().order != writeOrder) {

            // If reading is complete and there's no more data to write, exit the loop
            if (isReadingComplete) {
                pthread_mutex_unlock(&queueMutex);
                return nullptr;
            }

            // Wait for data to become available in the queue
            pthread_cond_wait(&dataAvailable, &queueMutex);
        }

        // Retrieve the next item from the queue
        item = sharedQueue.front();
        sharedQueue.pop();

        // Signal that space is available in the queue for more data
        pthread_cond_signal(&spaceAvailable);
        pthread_mutex_unlock(&queueMutex);

        // Start timing the duration to acquire the write order mutex
        auto writeOrderLockStart = std::chrono::high_resolution_clock::now();
        pthread_mutex_lock(&writeOrderMutex);
        auto writeOrderLockEnd = std::chrono::high_resolution_clock::now();

        // Start timing the duration to write to the file
        auto writeStart = std::chrono::high_resolution_clock::now(); 
        bool isLastLine = isReadingComplete && sharedQueue.empty();

        // Ensure that items are written in the correct order
        while (true) {
            if (item.order == writeOrder) {

                // Lock the mutex to ensure exclusive access to the output file
                pthread_mutex_lock(&outputFileMutex);
                *outputFile << item.line;

                // If it's not the last line, add a newline character
                if (!isLastLine) {
                    *outputFile << std::endl;
                }

                pthread_mutex_unlock(&outputFileMutex);

                // Increment the write order for the next item
                writeOrder++;
                pthread_mutex_unlock(&writeOrderMutex);
                break;
            }
            pthread_mutex_unlock(&writeOrderMutex);
        }
        auto writeEnd = std::chrono::high_resolution_clock::now(); 

        // If timing is enabled, update the global timing and count variables
        if (timingEnabled) {
            totalLockTimeWriter += std::chrono::duration<double>(lockEnd - lockStart).count();
            totalWriteTime += std::chrono::duration<double>(writeEnd - writeStart).count();
            totalWriteOrderLockTime += std::chrono::duration<double>(writeOrderLockEnd - writeOrderLockStart).count();
            writeCount++;
        }
    }
    return nullptr;
}

