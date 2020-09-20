# Greedy methods
import numpy as np

matrix = np.genfromtxt("matrix.csv")
weight = np.genfromtxt("weight_bias.txt", dtype=float)
weight = np.array(weight)

size = matrix.shape[0]
degree = np.sum(matrix, axis=1, dtype=float)

coverage = np.sum(matrix)//2
percent = 1
thresold = (1-percent)*coverage

selected_list_w = np.zeros(size)
included_edges_w = np.copy(matrix)
weight_w = 0

selected_list_d = np.zeros(size)
included_edges_d = np.copy(matrix)
weight_d = 0

selected_list_dw = np.zeros(size)
included_edges_dw = np.copy(matrix)
weight_dw = 0

# Should we skip degree zero nodes in weight
while (1):
    if ((np.sum(included_edges_w)//2) <= thresold):
        break

    min_index = -1
    min_weight = 1
    for i in range(size):
        if (weight[i] < min_weight and selected_list_w[i] == 0):
            min_index = i
            min_weight = weight[i]

    if (min_index == -1):
        print("Not Possible")
        break
    else:
        selected_list_w[min_index] = 1
        weight_w += weight[min_index]

    for j in range(size):
        included_edges_w[min_index][j] = 0
        included_edges_w[j][min_index] = 0

print("GREEDY BY WEIGHT")
print(selected_list_w)
print(weight_w)

while (1):
    if ((np.sum(included_edges_d)//2) <= thresold):
        break

    min_index = -1
    max_degree = 0
    degree_list = np.sum(included_edges_d, axis=1)

    for i in range(size):
        if (degree_list[i] > max_degree and selected_list_d[i] == 0):
            min_index = i
            max_degree = degree_list[i]

    if (min_index == -1):
        print("Not Possible")
        break
    else:
        selected_list_d[min_index] = 1
        weight_d += weight[min_index]

    for j in range(size):
        included_edges_d[min_index][j] = 0
        included_edges_d[j][min_index] = 0

print("GREEDY BY DEGREE")
print(selected_list_d)
print(weight_d)

while (1):
    if ((np.sum(included_edges_dw)//2) <= thresold):
        break

    min_index = -1
    max_ratio = 0
    degree_list = np.sum(included_edges_dw, axis=1)

    for i in range(size):
        if (degree_list[i]/weight[i] > max_ratio and selected_list_dw[i] == 0):
            max_ratio = degree_list[i]/weight[i]
            min_index = i

    if (min_index == -1):
        print("Not Possible")
        break
    else:
        selected_list_dw[min_index] = 1
        weight_dw += weight[min_index]

    for j in range(size):
        included_edges_dw[min_index][j] = 0
        included_edges_dw[j][min_index] = 0

print("GREEDY BY DEGREE AND WEIGHT")
print(selected_list_dw)
print(weight_dw)
