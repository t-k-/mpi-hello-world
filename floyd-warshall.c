#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define INF 999

int adj[][5] = {
	{0, 3, 8, INF, -4},
	{INF, 0, INF, 1, 7},
	{INF, 4, 0, INF, INF},
	{2, INF, -5, 0, INF},
	{INF, INF, INF, 6, 0}
};

void print(int rank, int mat[][5])
{
	int i, j;
	for (i = 0; i < 5; i++) {
		printf("node#%d: ", rank);
		for (j = 0; j < 5; j++) {
			printf("%d ", adj[i][j]);
		}
		printf("\n");
	}
}

int seq_main(int argc, char* argv[])
{
	int i, j, k;
	print(0, adj);
	for (k = 0; k < 5; k++)
		for (i = 0; i < 5; i++)
			for (j = 0; j < 5; j++)
				if (adj[i][j] > adj[i][k] + adj[k][j]) {
					adj[i][j] = adj[i][k] + adj[k][j];
				}
	print(0, adj);
	return 0;
}

int mpi_main(int argc, char* argv[])
{
	int i, j, k;
	int total_nodes, rank;
	const int MASTER = 0;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &total_nodes);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	printf("node#%d / %d\n", rank, total_nodes);

	for (k = 0; k < 5; k ++) {
		MPI_Bcast(adj, 5*5, MPI_INT, MASTER, MPI_COMM_WORLD);
		/* after Bcast, all nodes have the updated adj copy */

		/* do the floyd-warshall algorithm for assigned row */
		for (j = 0; j < 5; j++) {
			if (adj[rank][j] > adj[rank][k] + adj[k][j]) {
				adj[rank][j] = adj[rank][k] + adj[k][j];
			}
		}

		MPI_Gather(adj[rank], 5, MPI_INT, adj[rank], 5, MPI_INT,
				   MASTER, MPI_COMM_WORLD);
		/* after Gather, the master node has updated adj copy */
	}

	if (rank == 0)
		print(rank, adj);

	MPI_Finalize();
}

int main(int argc, char* argv[])
{
	mpi_main(argc, argv);
}
