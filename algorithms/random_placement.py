import numpy as np
import numpy
import sys
import random

junction = np.genfromtxt('Junction.csv', delimiter=",")
junction = np.array(junction)
junction = junction[1:, :]

Coverage = np.genfromtxt('Coverage.csv', dtype = float)
Coverage = np.array(Coverage)

print(Coverage)

iterations = 50
number = 30

final_list = []
coverage_val = numpy.zeros((iterations, number))

for itr in range(iterations):
    
    res = np.random.randint(low=0, high=junction.shape[0]-1, size=number)
    final_list.append(res)

print(final_list)

for i in range(iterations):
    
    matrix = np.zeros((Coverage.shape[0], Coverage.shape[1]))

    for j in range(number):

        matrix[:,final_list[i][j]] = Coverage[:,final_list[i][j]]
        coverage_val[i][j] = np.sum(np.max(matrix, axis = 1))/Coverage.shape[0]

answer = np.max(coverage_val, axis = 0)
print(answer)