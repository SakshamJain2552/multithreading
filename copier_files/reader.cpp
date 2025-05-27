#include "reader.h"
#include <functional>

reader::reader(const std::string& name, writer& mywriter) : thewriter(mywriter) {
    in.open(name);
}

void reader::run() {
    std::string line;
    while (true) {
        std::getline(in, line);
        thewriter.append(line);
        if (in.eof()) {
            break;  // Exit the loop if EOF is reached
        }
    }
}
