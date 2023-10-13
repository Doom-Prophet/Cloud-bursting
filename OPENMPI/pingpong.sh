#!/bin/bash
#SBATCH --job-name=pingpong_test
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=1
#SBATCH --time=4
#SBATCH --output=pingpong_output.txt
#SBATCH --partition=cpu_mini

mpirun ./pingpong
