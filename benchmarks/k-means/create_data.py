#!/usr/bin/python

import numpy as np
import matplotlib.pyplot as plt

from sklearn.cluster import MiniBatchKMeans, KMeans
from sklearn.metrics.pairwise import pairwise_distances_argmin
from sklearn.datasets.samples_generator import make_blobs


def main():
    import sys
    if len(sys.argv ) not in set([3,4]) :
        raise ValueError('one/two argument pls: python create_data N file_name [std]')
    N = int(sys.argv[1]) 
    f_name = sys.argv[2] 
    if len(sys.argv) == 3:
        std = 0.4
    else:
        std = float(sys.argv[3]) 
    np.random.seed(0)

    centers = [[-1,1,0],
              [-1,-1,0],
              [1,1,0],
              [1,-1,0],
              [0,0,3],
              [-1,1,6],
              [-1,-1,6],
              [1,1,6],
              [1,-1,6]]
    print('Centers: ',centers)
    X, labels_true = make_blobs(n_samples=N, centers=centers, cluster_std=std)

    with open(f_name,'wb') as f:
        for i in range(N):
            row = ','.join(['%.5f' % num for num in X[i]])
            if i==N-1:
                f.write(row)
            else:
                f.write(row+'\n')

if __name__ == "__main__":
    main()

