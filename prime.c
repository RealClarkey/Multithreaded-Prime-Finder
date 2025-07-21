//
// Created by Leigh Clarke 2245151 on 03/12/2024.
//
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> //pthread library POSIX thread (gcc prime.c -lpthread -lm)
#include <math.h>
#include <stdbool.h>

//Creating a structure for relevant variables.
struct structVars{
    int start;
    int end;
    int primeNumbers;
    int *numbersInFile;
    int *primes;
};

void *PrimeFinder(void *args) {
    struct structVars *threadStruct = (struct structVars *)args;

    int count = 0;

    for (int i = threadStruct->start; i <= threadStruct->end; i++) {
        int num = threadStruct->numbersInFile[i];

        //Logic for finding a prime number.
        if (num >= 2) { // Check if the number is greater than or equal to 2.
            bool isPrime = true;
            for (int j = 2; j <= sqrt(num); j++) {
                if (num % j == 0) { // Check divisibility up to sqrt(num), mark non-prime and break if divisible.
                    isPrime = false;
                    break;
                }
            }
            if (isPrime) { // increment prime numbers found count if prime number found (isPrime = true).
                count++;
            }
        }
    }
    // Allocating memory for the array that stores prime numbers.
    threadStruct->primes = (int *)malloc(count * sizeof(int));
    threadStruct->primeNumbers = count;

    count = 0;

    for (int i = threadStruct->start; i <= threadStruct->end; i++) {
        int num = threadStruct->numbersInFile[i];

        if (num >= 2) {
            bool isPrime = true;
            for (int j = 2; j <= sqrt(num); j++) {
                if (num % j == 0) {
                    isPrime = false;
                    break;
                }
            }
            if (isPrime) {
                threadStruct->primes[count++] = num;
            }
        }
    }

    pthread_exit(NULL);
    return NULL;
}


void main(int argc, char** argv){
    const char* filename = argv[2];
    FILE *fptr;
    FILE *fptw;
    int numbersInFile = 0; // variable for counting numbers in a file.
    int number;

    // Error handling for passing arguments in terminal. ./a.out [thread count] [ file name ].
    if(argc < 3){
        printf("Error: Both Thread Count and File name are required. Please try again.\n");
        exit(-1);
    }

    // Opening the file.
    fptr = fopen(filename, "r");

    // Error handling for opening file.
    if(fptr == NULL){
        printf("Error: Unable to open the file '%s'. Please check if the file exists or if you have the necessary permissions.\n", filename);
        exit(-1); // Exit program.
    } else {
        printf("Success: File opened.\n");
    }

    // Counting the number of lines in the file and adding to numbersInFile.
    while(fscanf(fptr, "%d", &number)==1){
        numbersInFile++;
    }

    // Using rewind function to go to start of the file.
    rewind(fptr);

    // Using malloc to allocate memory to store data from file.
    int * numbers = malloc(sizeof(int) * numbersInFile);

    // Store numbers from file in the list
    for(int i = 0; i < numbersInFile; i++){
        fscanf(fptr, "%d", numbers+ i);
    }
    //close file
    fclose(fptr);

    // Thread count input from command line argument.
    int threadCounts = atoi(argv[1]);

    // Error handling for thread count. Check thread count is positive
    if(threadCounts < 1){
        printf("Error: thread count needs to be positive. Please try again.\n");
        exit(-1);
    }

    int threadWorkload[threadCounts]; // Workload assigned to each thread
    int remainder = numbersInFile % threadCounts; // Find the remainder for adjustment

    int baseWorkload = numbersInFile/threadCounts; //**
    // Splitting the workload according to amount of threads(threadCounts).
    // Later adding the remaining data to threadWorkloads.
    for (int i = 0; i < threadCounts; i++){
        //threadWorkload[i] = numbersInFile / threadCounts;
        threadWorkload[i] = baseWorkload; //** Delete other line with //** as the line above does same thing.
    }
    //Distribute the remaining data across the threads.
    for (int j = 0; j < remainder; j++){
        threadWorkload[j] = threadWorkload[j] + 1;
    }

    // Work out the start and end indices of each thread's workload.
    int startList[threadCounts];
    int endList[threadCounts];

    for (int k = 0; k < threadCounts; k++){
        if(k == 0){
            startList[k] = 0;
            endList[k] = startList[k] + threadWorkload[k] - 1;
        }else{
            startList[k] = endList[k-1] + 1;
            endList[k] = startList[k] + threadWorkload[k] - 1;
        }
    }

    struct structVars results[threadCounts];// Array to store per-thread results.

    // creating the threads using pthread's function.
    pthread_t threadIDs[threadCounts];

    for(int i = 0; i < threadCounts; i++){
        // Set the threads start and end limits.
        results[i].start = startList[i];
        results[i].end = endList[i];
        results[i].numbersInFile = numbers; //Assign arrays to each thread.

        // Create a new thread with the PrimeFinder function and thread-specific data.
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_create(&threadIDs[i], &attr, PrimeFinder, &results[i]);
    }

    // After each thread is finished, accumulates the count of prime numbers from each thread.
    int totalPrimes = 0;
    for(int i = 0; i < threadCounts; i++){
        pthread_join(threadIDs[i], NULL);
        totalPrimes += results[i].primeNumbers;
    }

    // Allocating all the data to the allPrimes array.
    int * allPrimes = (int *)malloc(totalPrimes * sizeof(int));
    int currentIndex = 0;

    // Copy prime numbers from each thread's result to the consolidated 'allPrimes' array.
    for (int i = 0; i < threadCounts; i++){
        for(int j = 0; j < results[i].primeNumbers; j++){
            allPrimes[currentIndex] = results[i].primes[j];
            currentIndex++;
        }
        free(results[i].primes); //free up the thread specific used memory.
    }


    // Create file and write data to new file.
    // Open file for writing
    fptw = fopen("PrimeOutput.txt", "w");

    // Error handling to see if the file has been written.
    if(fptw == NULL){
        printf("Error: File has not been created.\n");
    }else{
        printf("Success: The file has been created.\n");
        printf("File has been created with a total of %d prime numbers. ", totalPrimes);
    }

    //Write total amount of prime numbers and the prime numbers to file.
    fprintf(fptw, "Total number of prime numbers = %d\n", totalPrimes);
    for(int i = 0; i < totalPrimes; i++){ //Writing the prime numbers to file.
        fprintf(fptw, "%d\n", allPrimes[i]);
    }

    // Close the file
    fclose(fptw);

}