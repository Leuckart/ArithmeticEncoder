import sys
import time
import math

import AE
import numpy as np
import scipy.stats

np.set_printoptions(suppress=True)


def main():
    data_size = 525000
    x_mean = 22.5
    x_sigma = 0.5

    Datas = np.random.normal(loc=x_mean, scale=x_sigma, size=(data_size))
    Datas = list(map(lambda i: int(round(i)), Datas))

    Min_V = min(Datas)
    Max_V = max(Datas)

    Means = list(x_mean * np.ones(data_size))
    Scales = list(x_sigma * np.ones(data_size))

    T1 = time.time()
    AE.encode(Datas, Means, Scales, "./compressed.bin")
    TE1 = time.time() - T1

    AE.init_decoder("./compressed.bin", Min_V, Max_V)
    T2 = time.time()
    Recons = []
    for i in range(data_size):
        Recons.append(AE.decode(Means[i], Scales[i]))
    TE2 = time.time() - T2

    error_number = 0
    for i in range(data_size):
        if Datas[i] != Recons[i]:
            error_number += 1
    print("Error Number: ", error_number)

    print("Encoding Time: ", TE1)
    print("Decoding Time: ", TE2)
    print("Total Time: ", TE1 + TE2)


if __name__ == "__main__":
    main()
