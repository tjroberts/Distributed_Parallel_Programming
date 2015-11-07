//Tyler Robertson


#include <omp.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define DATA_SIZE 100000
#define STRING_SIZE 15

int main( int argc, char * argv[] ){

    FILE * inFile;
    FILE * outFile;

    int th_id = 0;
    int num_th = 0;
    
    int NT = 0;
    int NS = 0;
    int num_chunks = 0;

    char valueArray[DATA_SIZE][STRING_SIZE];  //for search input
    char searchString[15];
    char line[STRING_SIZE];
    int searchResults[NT];

    char * relative_path_to_the_input_file;
    char * relative_path_to_the_output_file;

    //had to change this because when running on test node, [0] was the program name
    relative_path_to_the_input_file = argv[1];
    relative_path_to_the_output_file= argv[2];
    NT = atoi(argv[3]);
    num_chunks = atoi(argv[4]);
    
    //initialize reslults buffer
    int i;
    for ( i = 0 ; i < NT ; i++ )
	searchResults[i] = -1;
	
    //init all elements to 0
    int j;
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
			    //NT = atoi(line); //Number of tasks
			    break;
		    case 1 :
			    //NS = atoi(line); //Number of slices (always == NT)
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
    
    //try to set the amount of threads to use
    omp_set_num_threads(NT);
    
    //BEGIN PART A
    #pragma omp parallel private(th_id), shared(num_th, searchResults, valueArray)
    {
        
        num_th = omp_get_num_threads(); //check to see what the actual number of threads is
        th_id = omp_get_thread_num();
	
	searchResults[th_id] = -1;
	
	#pragma omp for schedule(static)
	for ( j = 0 ; j < DATA_SIZE; j++ )
	{
	    if ( strcmp(searchString, valueArray[j] ) == 0 ) {
		searchResults[th_id] = j + 4;
	    }
	}
	    
    }

    
    outFile = fopen(relative_path_to_the_output_file, "w");  //file for output

    for ( i = 0 ; i < num_th ; i++ ) {
        if ( searchResults[i] != -1 ) {
            fprintf(outFile, "%d, %d, %d\n", i, i, searchResults[i]);   //print to console and file, directions were unclear
            printf("%d, %d, %d\n", i, i, searchResults[i]);
        }
    }
    
    
    //BEGIN PART B
    #pragma omp parallel private(th_id), shared(num_th, searchResults, valueArray)
    {
        
        num_th = omp_get_num_threads();
        th_id = omp_get_thread_num();
	
	searchResults[th_id] = -1;
	
	#pragma omp for schedule(dynamic, num_chunks)
	for ( j = 0 ; j < DATA_SIZE; j++ )
	{
	    if ( strcmp(searchString, valueArray[j] ) == 0 ) {
		searchResults[th_id] = j + 4;
	    }
	}
    }

    for ( i = 0 ; i < num_th ; i++ ) {
        if ( searchResults[i] != -1 ) {
            fprintf(outFile, "%d, %d\n", i, searchResults[i]);   //print to console and file, directions were unclear
            printf("%d, %d\n", i, searchResults[i]);
        }
    }

    fclose(outFile); //close output file

    return 0;
}
