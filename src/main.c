#include <mpi.h>
#include <stdio.h>

#define ROOT 0

void broadcast_ring(void * data, int count, MPI_Datatype datatype, int root, MPI_Comm communicator) {

  int number_of_processes, current_process_rank;
  MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);
  MPI_Comm_rank(MPI_COMM_WORLD, &current_process_rank);

  int next_to_src = (ROOT + 1) % number_of_processes;
  int prev_of_src = (ROOT + number_of_processes - 1) % number_of_processes;
  int next = (current_process_rank + 1) % number_of_processes;
  int prev = (current_process_rank - 1 + number_of_processes) % number_of_processes;

  if (current_process_rank == ROOT) {
    MPI_Send(data, 1, MPI_INT, next_to_src, 0, MPI_COMM_WORLD);
  } else if (current_process_rank == prev_of_src) {
    MPI_Recv(data, 1, MPI_INT, prev, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  } else {
    MPI_Recv(data, 1, MPI_INT, prev, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Send(data, 1, MPI_INT, next, 0, MPI_COMM_WORLD);
  }
}

int main(int argc, char const *argv[]) {

  MPI_Init(NULL, NULL);

  int number_of_processes, current_process_rank;
  MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);
  MPI_Comm_rank(MPI_COMM_WORLD, &current_process_rank);

  int next_to_src = (ROOT + 1) % number_of_processes;
  int prev_of_src = (ROOT + number_of_processes - 1) % number_of_processes;
  int next = (current_process_rank + 1) % number_of_processes;
  int prev = (current_process_rank - 1 + number_of_processes) % number_of_processes;

  int data[4] = {10, 20, 30, 40};
  int result;
  int tmp;
  if (current_process_rank == ROOT) {
    for (int i = 1; i < 4; i++) {
      MPI_Send(&data[(ROOT - i + number_of_processes) % number_of_processes], 1, MPI_INT, next_to_src, 0, MPI_COMM_WORLD);
    }
    result = data[0];
  } else {

    MPI_Recv(&tmp, 1, MPI_INT, prev, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    for (int i = 0; i < (ROOT - 1 - current_process_rank + number_of_processes) % number_of_processes; i++) {
      MPI_Send(&tmp, 1, MPI_INT, next, 0, MPI_COMM_WORLD);
      MPI_Recv(&tmp, 1, MPI_INT, prev, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    result = tmp;
  }

  printf("Hello from process %d out of %d, my data is %d\n", current_process_rank, number_of_processes, result);


  // int data;
  // if (current_process_rank == ROOT) {
  //   data = 100;
  // }
  //
  // broadcast_ring(&data, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
  // printf("Hi am process %d out of %d and the data is %d.\n", current_process_rank, number_of_processes, data);

  MPI_Finalize();

  return 0;
}
