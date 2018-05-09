#include <mpi.h>
#include <stdio.h>

#define ROOT 0

int main(int argc, char const *argv[]) {

  MPI_Init(NULL, NULL);

  int number_of_processes, current_process_rank;
  MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);
  MPI_Comm_rank(MPI_COMM_WORLD, &current_process_rank);

  int next_to_src = (ROOT + 1) % number_of_processes;
  int prev_of_src = (ROOT + number_of_processes - 1) % number_of_processes;
  int next = (current_process_rank + 1) % number_of_processes;
  int prev = (current_process_rank - 1 + number_of_processes) % number_of_processes;

  int data;
  if (current_process_rank == ROOT) {
    data = 10;
    MPI_Send(&data, 1, MPI_INT, next_to_src, 0, MPI_COMM_WORLD);
    printf("Process ID: %d - Data: %d\n", current_process_rank, data);
  } else if (current_process_rank == prev_of_src) {
    MPI_Recv(&data, 1, MPI_INT, prev, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printf("Process ID: %d - Data: %d\n", current_process_rank, data);
  } else {
    MPI_Recv(&data, 1, MPI_INT, prev, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Send(&data, 1, MPI_INT, next, 0, MPI_COMM_WORLD);
    printf("Process ID: %d - Data: %d\n", current_process_rank, data);
  }

  MPI_Finalize();

  return 0;
}
