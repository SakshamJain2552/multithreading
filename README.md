# 🌀 OSP File Copier

A console-based C++20 personal project that demonstrates single- and multi-threaded file copying with synchronization, timing instrumentation, and clean design.

---

## 🔥 Features

- **Task 1: Single-Threaded Copier**  
  Read a text file one line at a time and write it to a new file, ensuring byte-perfect replication.

- **Task 2: Multi-Threaded Copier**  
  Launch a pool of reader threads to enqueue lines from the source and writer threads to dequeue and write them, parameterized by thread count.

- **Task 3: Synchronization with Mutexes**  
  Protect shared resources (file streams, queue) using `pthread_mutex_t` to eliminate race conditions.

- **Task 4: No Busy Waiting**  
  Use `pthread_cond_t` condition variables (or `sleep()` for partial credit) to block threads until work is available.

- **Timing Instrumentation**  
  Insert optional `-t` flag to measure CPU time (`clock()`) for reading, locking, queue operations, and writing.

---

## 🛠️ Build & Run

### Prerequisites

- C++20-capable compiler (e.g. `g++` ≥ 11 via `scl enable devtoolset-11 bash`)  
- POSIX threads library (`-lpthread`)  
- Unix-like shell

### Build

```bash
# Build both programs
make

# Or build individually
make copier   # builds single-threaded ./copier
make mtcopier # builds multi-threaded ./mtcopier
````

```bash
# Manual build
g++ -std=c++20 -Wall -Werror -O2 -lpthread \
  copier.cpp mtcopier.cpp Reader.cpp Writer.cpp Queue.cpp \
  -o copier -o mtcopier
```

### Run

```bash
# Single-threaded (Task 1)
./copier infile.txt outfile.txt [-t]

# Multi-threaded (Tasks 2–4)
./mtcopier <numthreads> infile.txt outfile.txt [-t]
```

* `infile.txt` – source file to copy
* `outfile.txt` – destination file
* `numthreads` – number of reader+writer threads (e.g. 10–10000)
* `-t` – optional flag to enable timing output

---

## 🧠 C++ Concepts Practiced

* **Abstract Data Types & Classes**
  Separate classes for file reading, writing, and shared queue.

* **Pointers & Dynamic Memory Management**
  Manual allocation/deallocation for queue nodes; RAII principles.

* **File I/O**
  Line-by-line parsing with `<fstream>` and error handling.

* **POSIX Threading**
  `pthread_create`, `pthread_join`; thread pools for readers/writers.

* **Synchronization Primitives**
  `pthread_mutex_t` for mutual exclusion; `pthread_cond_t` for condition signaling.

* **Timing with `<ctime>`**
  `clock()` to capture CPU cycles and convert to seconds.

* **Modern C++20 Features & STL**
  `auto`, range-based `for`, scoped enums, `std::string`, `std::vector`, `std::queue`.

* **Build Automation**
  `Makefile` targets for modular compilation and cleaning.
