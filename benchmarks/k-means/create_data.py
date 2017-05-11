#!/usr/bin/python

import numpy as np
def npBlobs(n_samples=5, centers=[[1,1]], cluster_std=0.5):
    if len(centers)<1:
        print('provide center')
        return None
    d = len(centers[0])
    cn = len(centers)
    cov = np.identity(d)*cluster_std
    pn = int(np.ceil(n_samples/float(cn)))
    res = np.zeros((pn*cn,d))
    labels = np.zeros(pn*cn)
    for i in xrange(cn):
        c = centers[i]
        res[(i*pn):((i+1)*pn),:] = np.random.multivariate_normal(c, cov, (pn, ))
        labels[(i*pn):((i+1)*pn)] = i
    s=np.arange(n_samples)
    return res[s],labels[s]

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
    X, _ = npBlobs(n_samples=N, centers=centers, cluster_std=std)

    with open(f_name,'wb') as f:
        for i in range(N):
            row = ','.join(['%.5f' % num for num in X[i]])
            if i==N-1:
                f.write(row)
            else:
                f.write(row+'\n')

if __name__ == "__main__":
    main()

