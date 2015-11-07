//Tyler Robertson
#define DATA_SIZE 100000
#define STRING_SIZE 15

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

pthread_mutex_t consoleMutex;
pthread_mutex_t debugMutex;


struct threadArgs {
	
	int threadId;
	char searchString[STRING_SIZE];
	int startIndex;
	int endIndex;
	char searchArray[DATA_SIZE][STRING_SIZE];
	
};

void *threadWork(void *args) {	
	
	struct threadArgs * newArgs;
	newArgs = (struct threadArgs *)args;
	
	bool found = false;
	int position = -1;
	
	int i;
	for ( i = newArgs->startIndex ; (i < newArgs->endIndex) && !found ; i++ ) {
		
		if ( strcmp(newArgs->searchArray[i], newArgs->searchString) == 0) {
			found = true;
			position = i;
		}
	}
	
	pthread_mutex_lock(&consoleMutex); //Lock for console output
	
	if ( found )
		printf("thread %d,\t found yes,\t slice %d,\t position %d\n", newArgs->threadId, newArgs->threadId, position + 4);
	else
		printf("thread %d,\t found no,\t slice %d,\t position %d\n", newArgs->threadId, newArgs->threadId, position);
	
	pthread_mutex_unlock(&consoleMutex); //Unlock for console output
	
	pthread_exit(NULL);
}

int main () {
	
	FILE *fp;
	char valueArray[DATA_SIZE][STRING_SIZE];  //for search input
	char searchString[STRING_SIZE];
	int NT = 0;
	int NS = 0;
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
	fp = fopen("fartico_aniketsh_input_partA.txt", "r");
	
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
    
    for ( i = 0 ; i < NS ; i++ ) {
		
		//set up arguments for thread
		memcpy(args[i].searchArray, valueArray, (DATA_SIZE * STRING_SIZE) * sizeof(char));
		strcpy(args[i].searchString, searchString);
		args[i].threadId = i + 1;
		args[i].startIndex = startIndex;
		args[i].endIndex = endIndex;
		
		status = pthread_create(&threads[i], NULL, threadWork, (void*)&args[i]);
		
		//calculate search area for next thread
		startIndex = startIndex + sectionSize;
		endIndex = endIndex + sectionSize;
	}
	
	//make main wait for threads to finish
	for ( i = 0 ; i < NT ; i++)
		pthread_join(threads[i], NULL);
	
	return 0;
}
