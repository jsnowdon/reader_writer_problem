/* 
 * CIS*4410 Assignment 1
 *
 * Author: Jonathan Snowdon
 *
 * Student#: 0709496
 *
 */

 typedef struct{

   int  thread_id;
   int iterations;
   int numOfWriters;
   int numOfReaders;
   pthread_mutex_t *listOfReaderLocks;
   pthread_mutex_t readerLock;

} thread_data;

void *readerThreads(void *threadid);
void *writerThreads(void *threadData);
void getUserInput(int *numOfReaders, int *numOfWriters, int *iterations);
void initFile(int numOfWriters);