
import numpy as np
import numpy
import sys

## Consider single RSU of Trange 243 metres

density = np.genfromtxt('Density.csv', delimiter=",")
density = np.array(density)

junction = np.genfromtxt('Junction.csv', delimiter=",")
junction = np.array(junction)
junction = junction[1:, :]

# distance = np.genfromtxt('Distance.csv', delimiter=",")
# distance = np.array(distance)
# distance = distance[1:,1:]

# clipped_distance = np.copy(distance)

# max_val = np.amax(clipped_distance)
# clipped_distance = np.clip(clipped_distance, 243, max_val)
# clipped_distance = clipped_distance - 243
# clipped_distance = np.clip(clipped_distance, 0, 1)

# coverage = 1 - clipped_distance

coverage = np.genfromtxt('coverage.csv', delimiter=",", dtype = float)
coverage = np.array(coverage)


def coverage_value (current_coverage):

  Total_sum = np.sum(current_coverage)

  return (Total_sum/current_coverage.shape[0])


def coverage_addition (current_coverage, coverage, RSU_list):

  answer = 0

  for i in range(0, coverage.shape[1]):
    answer += max(coverage[index][i] - current_coverage[i], 0)

    return answer

def next_solution (current_coverage, coverage, RSU_list):

  max_index = -1
  max_coverage_addition = 0

  curr_coverage_val = coverage_value(current_coverage, coverage, RSU_list)

  for i in range(RSU_list.shape[0]):

    if (RSU_list[i] == 1):
      continue;

    else:
      
      cov_addition = coverage_addition(current_coverage, coverage, RSU_list)

      if (cov_addition > max_coverage_addition):
        max_coverage_addition = cov_addition
        max_index = i

  return max_index


def updated_coverage_list (current_coverage, index, coverage):

  for i in range(0, coverage.shape[1]):
    current_coverage[i] = max(coverage[index][i], current_coverage[i])

    return current_coverage


max_budget = 200000

RSU_list = numpy.zeros((junction.shape[0]))
current_coverage = numpy.zeros((coverage.shape[1]))

current_budget = 0

while (current_budget <= max_budget):

  index = next_solution(current_coverage, coverage, RSU_list)

  if(index == -1):
    break

  cost = 3900 + 121.7 + junction[index][2]

  if ((current_budget + cost) > max_budget):
    break;

  else:

    RSU_list[index] = 1
    current_budget += cost
    current_coverage = updated_coverage_list(current_coverage, index, coverage)

  print(coverage_value(current_coverage, coverage, RSU_list))
  print(current_budget)

## Manually change from convBoundary

# print(RSU_list)

# boundary = np.array([0.00,0.00,4224.15,11360.21])

# def coordinates(pos, y_max):

#   cell = 10

#   x = pos[0] if pos[0]>=boundary[0] else boundary[0]
#   y = pos[1] if pos[0]>=boundary[1] else boundary[1]
      
#   x = int(x/cell)
#   y = int(y/cell)

#   return(x,y_max-y)

# d_max = np.amax(density)
# d_min = np.amin(density)

# new = (density - d_min)/(d_max - d_min)
# new = new*1000

# new = np.clip(new, 0, 255)

# kernel = np.ones((3,3), np.uint8)
# copy = cv2.dilate(new, kernel, iterations=1)

# for i in range(RSU_list.shape[0]):
  
#   if (RSU_list[i] == 1):
#     x,y = coordinates( np.array([junction[i][0], junction[i][1]]), density.shape[1] )   
#     copy = cv2.circle(copy, (x,y), 24, (255,0,0), 5)

# for i in range(RSU_list.shape[0]):
  
#   if (RSU_list[i] == 1):
#     x,y = coordinates( np.array([junction[i][0], junction[i][1]]), density.shape[1] )   
#     copy = cv2.circle(copy, (x,y), 7, (255,0,0), -1)

# plt.imshow(copy)