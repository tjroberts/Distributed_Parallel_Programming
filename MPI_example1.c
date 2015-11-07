//Tyler Robertson

#include "mpi.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define ROOT 0
#define DATA_SIZE 100000
#define STRING_SIZE 15

int main( int argc, char * argv[] ){

	FILE * inFile;
	FILE * outFile;

	int th_id;
	int num_th;
	double S_Time;
	double E_Time;
	int NT = 0;
	int NS = 0;

	char valueArray[DATA_SIZE][STRING_SIZE];  //for search input
	char * recvBuff;
	int answerBuffer[1];   //for tasks to reply for position they found string in
	char searchString[15];
	char line[STRING_SIZE];

	char * relative_path_to_the_input_file;
	char * relative_path_to_the_output_file;

	//had to change this because when running on test node, [0] was the program name
	relative_path_to_the_input_file = argv[1];
	relative_path_to_the_output_file= argv[2];

	MPI_Init( &argc, &argv );
	MPI_Comm_rank( MPI_COMM_WORLD, &th_id );
	MPI_Comm_size( MPI_COMM_WORLD, &num_th );

	int * outputBuff;
	outputBuff = malloc(4 * sizeof(int));     //for gather when getting all task results

	//int outputBuff[4];

	if ( th_id == ROOT ) {
		
		S_Time = MPI_Wtime();

		//init all elements to 0
		int i, j;
		for ( i = 0 ; i < DATA_SIZE ; i++) {
			for ( j = 0 ; j < STRING_SIZE ; j++ ) {
				valueArray[i][j] = 0;
			}
		}

		inFile = fopen(relative_path_to_the_input_file, "r");

		int counter = 0;
		while (fgets(line, STRING_SIZE, inFile)) {

			switch (counter) {
				case 0 :
					NT = atoi(line); //Number of tasks
					break;
				case 1 :
					NS = atoi(line); //Number of slices (always == NT)
					break;
				case 2 :
					strncpy(searchString, line, sizeof(line)); //search string
					break;
				default :
					strncpy(valueArray[counter - 3], line, sizeof(line)); //data
			}

			counter++;
		}

		//close input file for search data
		fclose(inFile);

	}


	//get section size of each array doing searching (rounded up because of uneven split)
	int sectionSize = ceil((double)DATA_SIZE/num_th);
	int elementSize = sectionSize * STRING_SIZE;

	//init receive buffer for slave machines
	recvBuff =(char *)malloc(sizeof(char) * elementSize);

	//root scatters array to all other machines
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Scatter(valueArray, elementSize, MPI_CHAR, recvBuff, elementSize, MPI_CHAR, ROOT, MPI_COMM_WORLD);

	//broadcast search string to all slaves
	MPI_Bcast(searchString, STRING_SIZE, MPI_CHAR, ROOT, MPI_COMM_WORLD);


	//now do the search
	bool found = false;
	int i = 0;
	int j = 0;
	int counter = 0;
	int linePosition = 0;
	char currentString[15];
	memset(currentString,0,sizeof(currentString));

	for ( i = 0 ; ((i < elementSize) && !found); i++) {

		currentString[counter] = recvBuff[i];

		if ( counter == (STRING_SIZE - 1) ) {

			if ( strcmp(searchString, currentString) == 0 ) {
				found = true;
				continue;
			}
			counter = 0;
			memset(currentString, 0, sizeof(char) * STRING_SIZE); //empty out currentString to load again
			linePosition++;
		}
		else
			counter++;
	}

	if ( found )
		answerBuffer[0] = linePosition + (th_id * sectionSize) + 4;  //add four because of inputs at beggining of file
	else
		answerBuffer[0] = -1;

	//gather all search results in ROOT task
	MPI_Gather(answerBuffer, 1, MPI_INT, outputBuff, 1, MPI_INT, ROOT, MPI_COMM_WORLD);

	//master prints results
	if ( th_id == ROOT ) {

		outFile = fopen(relative_path_to_the_output_file, "w");  //file for output

		E_Time = MPI_Wtime();  //End time of execution

		for ( i = 0 ; i < num_th ; i++ ) {
			if ( outputBuff[i] != -1 ) {
				fprintf(outFile, "Thread %d, found yes, slice %d, position %d\n", i, i, outputBuff[i]);   //print to console and file, directions were unclear
				printf("Thread %d, found yes, slice %d, position %d\n", i, i, outputBuff[i]);
			}
			else {
				fprintf(outFile, "Thread %d, found no, slice %d, position %d\n", i, i, outputBuff[i]);    //print to console and file, directions were unclear
				printf("Thread %d, found no, slice %d, position %d\n", i, i, outputBuff[i]);
			}
		}

		printf("%f\n", (E_Time - S_Time));    //print to console and file, directions were unclear
		fprintf(outFile, "%f\n", (E_Time - S_Time));

		fclose(outFile); //close output file
	}

	MPI_Finalize();
	return 0;
}
