//Tyler Robertson
#define DATA_SIZE 100000
#define STRING_SIZE 15

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

//GLOBALS
int NT = 0;
int NS = 0;
int SV = 1;

pthread_mutex_t consoleMutex;
pthread_mutex_t debugMutex;
pthread_mutex_t SVmutex;

struct threadArgs {
	
	int threadId;
	char searchString[STRING_SIZE];
	int dataSize;
	char searchArray[DATA_SIZE][STRING_SIZE];
	
};

void *threadWork(void *args) {
	
	int slice = 0;
	bool found = false;
	int position = -1;
	int startIndex, endIndex = 0;
	
	struct threadArgs * newArgs;
	newArgs = (struct threadArgs *)args;
	
	while ( true ) {
		
		pthread_mutex_lock(&SVmutex); //LOCK
		
		if ( SV == NS + 1 ) {
			
			pthread_mutex_unlock(&SVmutex);
			pthread_exit(NULL);
		}
		
		slice = SV; //get the current slice to search
		SV++;		//increment slice variable
		
		startIndex = computeStartIndex(slice, DATA_SIZE, NS);
		endIndex = computeEndIndex(slice, DATA_SIZE, NS);
		
		pthread_mutex_unlock(&SVmutex); //UNLOCK
	
		int i;
		for ( i = startIndex ; (i < endIndex) && !found ; i++ ) {
			
			if ( strcmp(newArgs->searchArray[i], newArgs->searchString) == 0) {
				found = true;
				position = i;
			}
		}
		
		pthread_mutex_lock(&consoleMutex); //Lock for console output
		
		if ( found ) {
			printf("thread %d,\t found yes,\t slice %d,\t position %d\n", newArgs->threadId, slice, position + 4);
			found = false;
		}
		else
			printf("thread %d,\t found no,\t slice %d,\t position %d\n", newArgs->threadId, slice, position);
		
		pthread_mutex_unlock(&consoleMutex); //Unlock for console output
		
		//sleep(1);    //SOMETIMES THREAD 1 IS SO FAST IT FINISHES SEARCH, LETS OTHER THREADS WORK
	
	}
}

inline int computeStartIndex(int slice, int size, int numSlices) {
	
	int sliceSize = size / numSlices;
	return sliceSize * (slice - 1);
}

inline int computeEndIndex(int slice, int size, int numSlices) {
	
	int sliceSize = size / numSlices;
	return (sliceSize * slice) - 1;
}

int main () {
	
	FILE *fp;
	char valueArray[DATA_SIZE][STRING_SIZE];  //for search input
	char searchString[STRING_SIZE];

	int sectionSize = 0;
	int startIndex, endIndex;
	
	char line[STRING_SIZE];
	int counter = 0;
	
	//init all elements to 0
	int i, j;
	for ( i = 0 ; i < DATA_SIZE ; i++)
		for ( j = 0 ; j < STRING_SIZE ; j++ )
			valueArray[i][j] = 0;
	
	//open file for search input
	fp = fopen("fartico_aniketsh_input_partB.txt", "r");

	
	while (fgets(line, STRING_SIZE, fp)) {
		
		switch (counter) {
			case 0 :
				NT = atoi(line); //Number of threads
				break;
			case 1 :
				NS = atoi(line); //Number of slices
				break;
			case 2 :
				strncpy(searchString, line, sizeof(line));
				break;
			default :
				strncpy(valueArray[counter - 3], line, sizeof(line));
		}
		counter++;
    }
		
    //close input file for search data
    fclose(fp);
    
    pthread_t threads[NT];
    sectionSize = DATA_SIZE / NS;
    startIndex = 0;
    endIndex = sectionSize - 1;
    int status;
    
    struct threadArgs args[NT];
    
    //SET UP ARGUMENTS FOR THREADS, SO IT DOENST SLOW DOWN THREAD CREATION IN FOR LOOP BELOW
    for ( i = 0 ; i < NT ; i++ ) {
		memcpy(args[i].searchArray, valueArray, (DATA_SIZE * STRING_SIZE) * sizeof(char));
		strcpy(args[i].searchString, searchString);
		args[i].threadId = i + 1;
		args[i].dataSize = DATA_SIZE;
	}
    
    for ( i = 0 ; i < NT ; i++ )
		status = pthread_create(&threads[i], NULL, threadWork, (void*)&args[i]);
	
	//make main wait for threads to finish
	for ( i = 0 ; i < NT ; i++)
		pthread_join(threads[i], NULL);
	
	return 0;
}

