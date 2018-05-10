#include <mpi.h>
#include <stdio.h>

#define ROOT 0

int current_process_rank, number_of_processes,
  next_to_src, prev_of_src, next, prev, result, tmp;

void broadcast_ring(int * data, int count, MPI_Datatype datatype, MPI_Comm communicator) {

  if (current_process_rank == ROOT) {
    MPI_Send(data, count, MPI_INT, next_to_src, 0, MPI_COMM_WORLD);
  } else if (current_process_rank == prev_of_src) {
    MPI_Recv(data, count, MPI_INT, prev, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  } else {
    MPI_Recv(data, count, MPI_INT, prev, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Send(data, count, MPI_INT, next, 0, MPI_COMM_WORLD);
  }

}

void scatter_ring(int * data, int count, MPI_Datatype datatype, MPI_Comm communicator) {

  if (current_process_rank == ROOT) {
    for (int i = 1; i < 4; i++) {
      MPI_Send(&data[(ROOT - i + number_of_processes) % number_of_processes], count, MPI_INT, next_to_src, 0, MPI_COMM_WORLD);
    }
    result = data[current_process_rank];
  } else {

    MPI_Recv(&tmp, count, MPI_INT, prev, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    for (int i = 0; i < (ROOT - 1 - current_process_rank + number_of_processes) % number_of_processes; i++) {
      MPI_Send(&tmp, count, MPI_INT, next, 0, MPI_COMM_WORLD);
      MPI_Recv(&tmp, count, MPI_INT, prev, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    result = tmp;
  }

}

void displayVector(int * vector, int sizeOfVector, int processID) {
  printf("ProcessID: %d - ", processID);
  for (int i = 0; i < sizeOfVector; i++) {
    (i == sizeOfVector - 1) ? printf("%d\n", vector[i]) : printf("%d ", vector[i]);
  }
}

int main(int argc, char const *argv[]) {

  MPI_Init(NULL, NULL);

  MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);
  MPI_Comm_rank(MPI_COMM_WORLD, &current_process_rank);
  next_to_src = (ROOT + 1) % number_of_processes;
  prev_of_src = (ROOT + number_of_processes - 1) % number_of_processes;
  next = (current_process_rank + 1) % number_of_processes;
  prev = (current_process_rank - 1 + number_of_processes) % number_of_processes;

  // Create a vector
  int vector[4] = {0};
  int matrix[4] = {0};
  if (current_process_rank == ROOT) {
    vector[0] = 10;
    vector[1] = 20;
    vector[2] = 30;
    vector[3] = 40;
    matrix[0] = 100;
    matrix[1] = 200;
    matrix[2] = 300;
    matrix[3] = 400;
  }

  broadcast_ring(vector, 4, MPI_INT, MPI_COMM_WORLD);

  scatter_ring(matrix, 1, MPI_INT, MPI_COMM_WORLD);

  displayVector(vector, 4, current_process_rank);
  printf("Hi am process %d out of %d and the data is %d.\n", current_process_rank, number_of_processes, result);

  MPI_Finalize();

  return 0;
}
