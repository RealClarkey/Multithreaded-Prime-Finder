# Multithreaded Prime Finder (C, POSIX Threads)

This program reads a list of numbers from a file and uses multithreading to identify and count all the prime numbers. It was developed as part of a second-year module in *Numerical Methods and Concurrency* .

## Features
- Efficient prime number detection using `sqrt(n)` logic.
- Dynamically splits work equally across threads.
- Handles arbitrary file sizes and thread counts.
- Uses `malloc` for dynamic memory allocation.
- Outputs all prime numbers to `PrimeOutput.txt`.

## Assessment Criteria
- Prime detection algorithm
- Use of dynamic memory (`malloc`)
- Equal computational workload across threads
- Outputting results to file

## Requirements
- GCC or compatible C compiler
- POSIX-compliant system (Linux/macOS/WSL)

##  How to Compile and Run

```bash
gcc prime.c -o prime_finder -lpthread -lm
./prime_finder [thread_count] [filename]
