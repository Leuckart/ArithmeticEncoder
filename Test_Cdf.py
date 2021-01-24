import sys
import time
import math

import AE
import numpy as np
import scipy.stats

np.set_printoptions(suppress=True)


def main():
    data_size = 525000

    x_mean = 25
    x_sigma = 0.5

    Datas = np.random.normal(loc=x_mean, scale=x_sigma, size=(data_size))
    Datas = list(map(lambda i: int(round(i)), Datas))

    Min_V = min(Datas)
    Max_V = max(Datas)

    Means = x_mean * np.ones(data_size)
    Scales = x_sigma * np.ones(data_size)

    range_size = Max_V - Min_V + 1
    factor = (1 << 16) - range_size

    cdf = scipy.stats.norm(x_mean - Min_V, x_sigma).cdf(np.arange(range_size + 1) - 0.5)
    for i in range(range_size + 1):
        cdf[i] = cdf[i] * factor + i + 1
    cdf = cdf.astype(np.int)
    print(cdf)

    Cdf = np.tile(cdf, [data_size, 1])

    T1 = time.time()
    Cdf_0 = list(map(lambda x, y: int(y[x - Min_V]), Datas, Cdf))
    Cdf_1 = list(map(lambda x, y: int(y[x - Min_V]), Datas, Cdf[:, 1:]))
    AE.encode_cdf(Cdf_0, Cdf_1, "./compressed.bin")
    TE1 = time.time() - T1

    AE.init_decoder("./compressed.bin", Min_V, Max_V)
    T2 = time.time()
    Recons = []
    for i in range(data_size):
        Recons.append(AE.decode_cdf(cdf.tolist()))
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
