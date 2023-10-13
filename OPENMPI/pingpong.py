from mpi4py import MPI
import numpy as np
import time

def pingpong(size):
    rank = MPI.COMM_WORLD.Get_rank()
    partner = 1 - rank
    data = np.zeros(size, dtype=np.float64)

    if rank == 0:
        print(f'Ping-pong benchmark for size: {size} doubles')

    iterations = 10
    MPI.COMM_WORLD.Barrier()

    start_time = time.time()
    for i in range(iterations):
        if rank == i % 2:
            MPI.COMM_WORLD.Send(data, dest=partner, tag=0)
        else:
            MPI.COMM_WORLD.Recv(data, source=partner, tag=0)
    end_time = time.time()

    elapsed_time = end_time - start_time
    if rank == 0:
        bandwidth = (2 * size * iterations * 8) / (elapsed_time * 1e9)
        print(f'Bandwidth: {bandwidth:.2f} GB/s')

if __name__ == "__main__":
    sizes = [1, 10, 100]
    for size in sizes:
        pingpong(size)
