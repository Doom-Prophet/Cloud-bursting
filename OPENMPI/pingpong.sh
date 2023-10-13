#!/bin/bash
#SBATCH --job-name=pingpong_test
#SBATCH --nodes=1
#SBATCH --ntasks=2
#SBATCH --time=4
#SBATCH --output=pingpong_output.txt
#SBATCH --partition=cpu_mini

mpirun -np 2 ./pingpong

