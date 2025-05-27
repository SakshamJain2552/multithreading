#include "reader.h"
#include "writer.h"
#include <cstdlib>
#include <chrono>
#include <iostream>

int main(int argc, char** argv) {
    bool printTiming = false;  // By default, don't print timing

    /* check command line arguments */
    if (argc < 3 || argc > 4) {
        std::cerr << "Usage: " << argv[0] << " infile outfile [-t]" << std::endl;
        return EXIT_FAILURE;
    }

    if (argc == 4 && std::string(argv[3]) == "-t") {
        printTiming = true;
    } else if (argc == 4) {
        std::cerr << "Invalid option. Use -t for timing." << std::endl;
        return EXIT_FAILURE;
    }

    auto totalStart = std::chrono::high_resolution_clock::now(); // Start total timer

    /* load the file and copy it to the destination */
    writer mywriter(argv[2]);
    reader myreader(argv[1], mywriter);

    auto readerStart = std::chrono::high_resolution_clock::now();
    myreader.run();
    auto readerEnd = std::chrono::high_resolution_clock::now();

    auto writerStart = std::chrono::high_resolution_clock::now();
    mywriter.run();
    auto writerEnd = std::chrono::high_resolution_clock::now();

    auto totalEnd = std::chrono::high_resolution_clock::now(); // End total timer

    if (printTiming) {
        std::cout << "Time taken by reader: " << std::chrono::duration<double>(readerEnd - readerStart).count() << " seconds." << std::endl;
        std::cout << "Time taken by writer: " << std::chrono::duration<double>(writerEnd - writerStart).count() << " seconds." << std::endl;
        std::cout << "Total execution time: " << std::chrono::duration<double>(totalEnd - totalStart).count() << " seconds." << std::endl;
    }

    return EXIT_SUCCESS;
}
