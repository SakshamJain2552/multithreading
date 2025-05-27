#include "reader.h"

void* reader::runner(void* arg) {

    // Cast the passed argument to an ifstream pointer
    std::ifstream* inputFile = static_cast<std::ifstream*>(arg);

    // Continuously read from the input file until the end
    while (true) {
        QueueItem item;

        // Start timing the duration to acquire the queue mutex
        auto lockStart = std::chrono::high_resolution_clock::now();
        // Lock the mutex to ensure exclusive access to the shared queue
        pthread_mutex_lock(&queueMutex);
        auto lockEnd = std::chrono::high_resolution_clock::now();

        // Start timing the duration to read from the file
        auto readStart = std::chrono::high_resolution_clock::now();

        // Check if the file is still good for reading
        if (inputFile->good()) {

            // Read a line from the file and store it in the queue item
            std::getline(*inputFile, item.line);

            // Assign an order number to the item and increment the read order
            item.order = readOrder++;

            // Push the item to the shared queue
            sharedQueue.push(item);

            // Signal that data is available in the queue
            pthread_cond_signal(&dataAvailable);
        } else {

            // If the end of the file is reached, set the reading complete flag
            isReadingComplete = true;

            // Broadcast to all waiting threads that reading is complete
            pthread_cond_broadcast(&dataAvailable);

            // Unlock the mutex and exit the loop
            pthread_mutex_unlock(&queueMutex);
            break;
        }
        auto readEnd = std::chrono::high_resolution_clock::now();

        // Unlock the mutex after reading
        pthread_mutex_unlock(&queueMutex);

    // If timing is enabled, update the global timing and count variables
    if (timingEnabled) {
        totalLockTimeReader += std::chrono::duration<double>(lockEnd - lockStart).count();
        totalReadTime += std::chrono::duration<double>(readEnd - readStart).count();
        readCount++;
    }
    }
    return nullptr;
}