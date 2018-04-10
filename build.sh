#!/bin/sh
module load openmpi-1.10-x86_64
mpicc -o a.out floyd-warshall.c
mpirun -np 5 ./a.out
