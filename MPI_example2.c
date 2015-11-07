//Tyler Robertson

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define ROOT 0

void my_barrier() {

	int num_th;
	int th_id;

	//determine rank and size
	MPI_Comm_rank( MPI_COMM_WORLD, &th_id );
	MPI_Comm_size( MPI_COMM_WORLD, &num_th );

	int recvBuff[1];
	int sendBuff[1];

	if ( th_id == ROOT ) {

		sendBuff[0] = th_id;   //message with root rank

		int i = 0;
		for ( i = 1 ; i < num_th; i++ )
			MPI_Recv(recvBuff, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); //blocks here

		for ( i = 1 ; i < num_th; i++ )
			MPI_Send(sendBuff, 1, MPI_INT, i, 0, MPI_COMM_WORLD);

	} //for all slave processes
	else {

		sendBuff[0] = th_id;

		//send message to master
		MPI_Send(sendBuff, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

		//now wait for reply
		MPI_Recv(recvBuff, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); //blocks here
	}
}

int main( int argc, char * argv[] ){

	int th_id;
	int num_th;
	FILE * outFile;

	char * relative_path_to_the_input_file;
	char * relative_path_to_the_output_file;


	//had to change this because when running on test node, [0] was the program name
	//need dummy value for first in since this program has no input
	relative_path_to_the_input_file = argv[1];
	relative_path_to_the_output_file = argv[2];

	MPI_Init( &argc, &argv );
	MPI_Comm_rank( MPI_COMM_WORLD, &th_id );
	MPI_Comm_size( MPI_COMM_WORLD, &num_th );

	double S_Time = MPI_Wtime();

	//program here
	my_barrier();

	double E_Time = MPI_Wtime();

	if ( th_id == ROOT ) {

		outFile = fopen(relative_path_to_the_output_file, "w");
		fprintf(outFile,"%f",(E_Time - S_Time));
		printf("%f\n", (E_Time - S_Time));         //print to console to be safe, directions were confusing
		fclose(outFile);
	}

	MPI_Finalize();
	return 0;
}
