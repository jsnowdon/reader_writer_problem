/* 
 * CIS*4410 Assignment 1
 *
 * Author: Jonathan Snowdon
 *
 * Student#: 0709496
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "../include/a1.h"

#define FILENAME "./file.bin"
 pthread_mutex_t writerLock;
 pthread_mutex_t curReaderLock;

int main (int argc, char *argv[])
{
	int numOfReaders = 0, numOfWriters = 0, iterations = 0, rc;
	int i;

	/*Get the number of reader and writer threads from the user
	  along with the number of iterations*/
	getUserInput(&numOfReaders, &numOfWriters, &iterations);

	/*Initialize file*/
	initFile(numOfWriters);

	/*Create the reader threads*/
	pthread_t readers[numOfReaders];

	/*Malloc space for all the reader locks*/
	pthread_mutex_t *readerLocks = malloc(sizeof(pthread_mutex_t)*numOfReaders);

	/*Initialize all the reader locks*/
	for ( i = 0; i < numOfReaders; i++ ){
		pthread_mutex_init( &readerLocks[i], NULL);
	}

	/*Initialize the writer lock*/
	pthread_mutex_init(&writerLock, NULL);

	/*Create an array of structures for the readers*/
	thread_data reader_data_array[numOfReaders];

	for( i = 0; i < numOfReaders; i++ ){

		/*initialize struct*/
		reader_data_array[i].thread_id = i+1;
		reader_data_array[i].iterations = iterations;
		reader_data_array[i].numOfWriters = numOfWriters;
		reader_data_array[i].numOfReaders = numOfReaders;
		reader_data_array[i].listOfReaderLocks = NULL;
		reader_data_array[i].readerLock = readerLocks[i];

		/*create the pthread*/
		rc = pthread_create(&readers[i], NULL, readerThreads, &reader_data_array[i]);

		/*ensure no errors*/
		if (rc){
	    	printf("ERROR; return code from pthread_create() is %d\n", rc);
	    	exit(-1);
	  	}
	}

	/*Create the reader threads*/
	pthread_t writers[numOfWriters];

	thread_data writer_data_array[numOfWriters];

	for( i = 0; i < numOfWriters; i++ ){

		/*initialize struct*/
		writer_data_array[i].thread_id = i+1;
		writer_data_array[i].iterations = iterations;
		writer_data_array[i].numOfWriters = numOfWriters;
		writer_data_array[i].numOfReaders = numOfReaders;
		writer_data_array[i].readerLock = curReaderLock; //this is not needed
		writer_data_array[i].listOfReaderLocks = readerLocks;

		/*create the pthread*/
		rc = pthread_create(&writers[i], NULL, writerThreads, &writer_data_array[i]);

		/*ensure no errors*/
		if (rc){
	    	printf("ERROR; return code from pthread_create() is %d\n", rc);
	    	exit(-1);
	  	}
	}

	/* begin cleanup to avoid memory leaks */
	
	/* join all the reader the pthreads so no memory leaks */
	for( i = 0; i < numOfReaders; i++ ){
		pthread_join(readers[i], NULL);
	}

	/* join all the writer the pthreads so no memory leaks */
	for( i = 0; i < numOfWriters; i++ ){
		pthread_join(writers[i], NULL);
	}

	/* cleanup */
	pthread_mutex_destroy(readerLocks);
	/* cleanup */
	free(readerLocks);
	/* cleanup */
	pthread_exit(NULL);

 	return 0;
}

void *readerThreads(void *threadData)
{

	thread_data *my_data;
	my_data = (thread_data *) threadData;

	int threadID, iterations, bufferSize, i, j;
	FILE *fp;
	int temp;
	char intString[sizeof(int)];

	/*Get data from struct*/
	threadID = my_data->thread_id;
	iterations = my_data->iterations;
	bufferSize = my_data->numOfWriters;
	curReaderLock = my_data->readerLock;

	for( i = 0; i < iterations; i++){

		/*lock our own reader lock*/
		pthread_mutex_lock (&curReaderLock);

		/*create buffer*/
		char *buffer = (char *)calloc(bufferSize,sizeof(int));

		fp = fopen(FILENAME, "rb+");

		/*get all integers from file*/
		for( j = 0; j < bufferSize; j++){
			/*move to correct integer*/
			fseek(fp,sizeof(int)*j,SEEK_SET);
			/*read integer into temp*/
    		fread(&temp, sizeof(int), 1, fp);
    		/*convert integer to string*/
    		sprintf(intString, "%d", temp);

    		/*add new interger string to buffer*/
    		strncat(buffer,intString,sizeof(int));

    		/*rewind the filepointer*/
    		rewind(fp);
    	}

    	printf("I am thread #%d, iteration #%d and the file contains: %s\n", threadID, i+1, buffer);

    	/*cleanup*/
    	fclose(fp);
    	free(buffer);

    	/*unlock our own reader lock*/
    	pthread_mutex_unlock (&curReaderLock);

    	/*sleep for a bit*/
		sleep(rand()%2);
	}

	/* cleanup */
	pthread_exit((void *)threadData);
}

void *writerThreads(void *threadData)
{
	thread_data *my_data;
	my_data = (thread_data *) threadData;

	int threadID, iterations, fileSize, numOfReaders, i, j, k;
	FILE *fp;
	int temp;
	pthread_mutex_t *listOfReaderLocks;

	/*Get data from struct*/
	threadID = my_data->thread_id;
	iterations = my_data->iterations;
	fileSize = my_data->numOfWriters;
	numOfReaders = my_data->numOfReaders;
	listOfReaderLocks = my_data->listOfReaderLocks;


	for( i = 0; i < iterations; i++){

		/*lock all the reader locks*/
		for( k = 0; k < numOfReaders; k++ ){
			pthread_mutex_lock (&listOfReaderLocks[k]);
		}

		/*lock the global writer lock*/
		pthread_mutex_lock (&writerLock);

		fp = fopen(FILENAME, "rb+");

		/*get all integers from file*/
		for( j = 0; j < fileSize; j++){

			/*read integer into temp*/
			fseek(fp,sizeof(int)*j,SEEK_SET);
    		fread(&temp, sizeof(int), 1, fp);
    		
    		/*rewind the filepointer*/
    		rewind(fp);

    		/*get the writers integer from the file*/
    		if(threadID == j+1){
    			/*increment the number*/
    			temp = temp + 1;
    			/*write the new integer to the file*/
    			fseek(fp,sizeof(int)*j,SEEK_SET);
				fwrite(&temp,sizeof(int),1, fp);

				/*rewind the filepointer*/
    			rewind(fp);

				/*make sure no errors occured*/
				if(ferror(fp)){
					printf("Error writing to file\n");
					exit(-1);
				}
    		}
    	}

    	/*cleanup*/
    	fclose(fp);

    	/*unlock the global writer lock*/
    	pthread_mutex_unlock (&writerLock);

    	/*unlock all the reader locks*/
		for( k = 0; k < numOfReaders; k++ ){
			pthread_mutex_unlock (&listOfReaderLocks[k]);
		}

		/*sleep for a bit*/
		sleep(rand()%2);
	}

	/* cleanup */
	pthread_exit((void *)threadData);
}

void getUserInput(int *numOfReaders, int *numOfWriters, int *iterations)
{
	/*Get user input*/
	printf("Please enter the number of Reader Threads:\n");
	scanf("%d", numOfReaders);

	printf("Please enter the number of Writer Threads:\n");
	scanf("%d", numOfWriters);

	printf("Please enter the number of iterations:\n");
	scanf("%d", iterations);

	printf("---------------------------------------\n");
}

void initFile(int numOfWriters)
{
	int i, placeHolder = 0;
	/*open the file for writting*/
	FILE *fp = fopen(FILENAME, "wb");

	/*check that the file opened correctly*/
	if(!fp){
		printf("Error opening file\n");
		exit(-1);
	}

	/*print a 0 to the file for every writer*/
	for ( i = 0; i < numOfWriters; i++){
		/*write the zero to the binary file*/
		fwrite(&placeHolder,sizeof(int),1, fp);
		/*make sure no errors occured*/
		if(ferror(fp)){
			printf("Error writing to file\n");
			exit(-1);
		}
	}

	fclose(fp);
}
