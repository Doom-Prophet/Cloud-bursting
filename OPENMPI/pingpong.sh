#!/bin/bash
#SBATCH --job-name=pingpong_test
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=1
#SBATCH --time=00:10:00
#SBATCH --output=pingpong_output.txt

mpirun ./pingpong
