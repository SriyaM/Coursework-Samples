"""
Project Objective:

I set out to implement principal component analysis (PCA) with condensed nearest neighbors (CNN) to predict whether a patient had benign or malignant breast cancer. The data set I used outlines a number of tumor characteristics derived from patient scans including radius, texture, perimeter, area, smoothness, compactness, etc. 

What I Accomplished:

I was able to implement PCA into our CNN program by:
1. Converting our dictionary of data into an array for calculations. 
2. Standardizing the data so that the mean of each column was zero.
3. Calculating the covariance matrix.
4. Using NumPy to calculate the eigenvectors and eigenvalues of the cov matrix. 
5. Ordering the principal components based on the eigenvalues and discarding the 5 least important ones.
6. Projecting our data along the axes of the principal components. 
7. Taking this new data and feeding it through our CNN  algorithm.

Results:

Our original CNN program had an accuracy of about 0.86 - 0.933. I tested out the new PCA data with the CNN program. The accuracy-component trade off seemed to be ideal when 4 principal components were chosen of the 10 which were calculated (a pretty significant cut). The accuracy of the CNN program with 4 components was about 0.76 - 0.866. For a huge amount of data reduction, the accuracy did not take a big hit.

Issues:

While I was starting to implement the PCA program, it was difficult to understand each step of the data manipulation without having knowledge of the significance of covariance matrices, eigenvectors, etc. Once I was able to decode the process itself, I mainly ran into issues with doing these complex calculations with various data structures. I initially tried keeping our original dictionary format, then tried nested lists, and finally, I ended up using NumPy arrays. After putting the data in arrays, it was pretty smooth sailing and I was able to complete the PCA implementation.

"""

import csv
import random
import collections
import seaborn as sns
import numpy as np
from numpy.linalg import eig

with open('FinalData1.csv', newline = '') as csvfile:
  reader = csv.DictReader(csvfile)
  data = list(reader) #opening data file and putting it in a dictionary

#data 2 will be used for the pca analysis - formatted as a numpy array for easier calculations
data2 = []
with open('FinalData1.csv') as csvfile:    
	csvReader = csv.reader(csvfile)    
	for row in csvReader:        
		data2.append(row)

#deleting the rows and columns that dont have numbers for calculations
data2 = data2[1:]
data2 = np.array(data2)
data3 = data2
data2 = np.delete(data2,11,axis=1)
data2 = data2.astype(np.float)

# allows us to not have to resize in the future
length = len(data)
radius = [None]*length
texture = [None]*length
perimeter = [None]*length
area = [None]*length
smoothness = [None]*length
compactness = [None]*length
concavity = [None]*length
concave = [None]*length
symmetry = [None]*length
fractal = [None]*length

#adding labels
for (i,row) in enumerate(data):
  radius[i] = row['radius_mean']
  texture[i] = row['texture_mean']
  perimeter[i] = row['perimeter_mean']
  area[i] = row['area_mean']
  smoothness[i] = row['smoothness_mean']
  compactness[i] = row['compactness_mean']
  concavity[i] = row['concavity_mean']
  concave[i] = row['concave points_mean']
  symmetry[i] = row['symmetry_mean']
  fractal[i] = row['fractal_dimension_mean']

# initializing the max and min for each charactersitic
data[0]['Id'] = int(data[0]['Id'])
data[0]['radius_mean'] = float(data[0]['radius_mean'])
max_radius = min_radius = data[0]['radius_mean'] 
data[0]['texture_mean'] = float(data[0]['texture_mean'])
max_texture = min_texture = data[0]['texture_mean']
data[0]['perimeter_mean'] = float(data[0]['perimeter_mean'])
max_perimeter = min_perimeter = data[0]['perimeter_mean']
data[0]['area_mean'] = float(data[0]['area_mean'])
max_area = min_area = data[0]['area_mean']
data[0]['smoothness_mean'] = float(data[0]['smoothness_mean'])
max_smoothness = min_smoothness = data[0]['smoothness_mean'] 
data[0]['compactness_mean'] = float(data[0]['compactness_mean'])
max_compactness = min_compactness = data[0]['compactness_mean']
data[0]['concavity_mean'] = float(data[0]['concavity_mean'])
max_concavity = min_concavity = data[0]['concavity_mean']
data[0]['concave points_mean'] = float(data[0]['concave points_mean'])
max_concave = min_concave = data[0]['concave points_mean']
data[0]['symmetry_mean'] = float(data[0]['symmetry_mean'])
max_symmetry = min_symmetry = data[0]['symmetry_mean']
data[0]['fractal_dimension_mean'] = float(data[0]['fractal_dimension_mean'])
max_fractal = min_fractal = data[0]['fractal_dimension_mean']

#iterating thruogh the data and finding the max and mean for each characteristic
for datum in data[1:]:
  datum['Id'] = int(datum['Id'])
  datum['radius_mean'] = float(datum['radius_mean'])
  if datum['radius_mean'] > max_radius: 
    max_radius = datum['radius_mean']
  elif datum['radius_mean'] < min_radius:
    min_radius = datum['radius_mean']

  datum['texture_mean'] = float(datum['texture_mean'])
  if datum['texture_mean'] > max_texture:
    max_texture = datum['texture_mean']
  elif datum['texture_mean'] < min_texture:
    min_texture = datum['texture_mean']

  datum['perimeter_mean'] = float(datum['perimeter_mean'])
  if datum['perimeter_mean'] > max_perimeter:
    max_perimeter = datum['perimeter_mean']
  elif datum['perimeter_mean'] < min_perimeter:
    min_perimeter = datum['perimeter_mean']

  datum['area_mean'] = float(datum['area_mean'])
  if datum['area_mean'] > max_area:
    max_area = datum['area_mean']
  elif datum['area_mean'] < min_area:
    min_area = datum['area_mean']

  datum['smoothness_mean'] = float(datum['smoothness_mean'])
  if datum['smoothness_mean'] > max_smoothness:
    max_smoothness = datum['smoothness_mean']
  elif datum['smoothness_mean'] < min_smoothness:
    min_smoothness = datum['smoothness_mean']

  datum['compactness_mean'] = float(datum['compactness_mean'])
  if datum['compactness_mean'] > max_compactness:
    max_compactness = datum['compactness_mean']
  elif datum['compactness_mean'] < min_compactness:
    min_compactness = datum['compactness_mean']

  datum['concavity_mean'] = float(datum['concavity_mean'])
  if datum['concavity_mean'] > max_concavity:
    max_concavity = datum['concavity_mean']
  elif datum['concavity_mean'] < min_concavity:
    min_concavity = datum['concavity_mean']

  datum['concave points_mean'] = float(datum['concave points_mean'])
  if datum['compactness_mean'] > max_concave:
    max_concave = datum['concave points_mean']
  elif datum['concave points_mean'] < min_concave:
    min_concave = datum['concave points_mean']
    
  datum['symmetry_mean'] = float(datum['symmetry_mean'])
  if datum['symmetry_mean'] > max_symmetry:
    max_symmetry = datum['symmetry_mean']
  elif datum['symmetry_mean'] < min_symmetry:
    min_symmetry = datum['symmetry_mean']

  datum['fractal_dimension_mean'] = float(datum['fractal_dimension_mean'])
  if datum['fractal_dimension_mean'] > max_fractal:
    max_fractal = datum['fractal_dimension_mean']
  elif datum['fractal_dimension_mean'] < min_fractal:
    min_fractal = datum['fractal_dimension_mean']

#calculating the range for each variable
radius_range = max_radius - min_radius
texture_range = max_texture - min_texture
perimeter_range = max_perimeter - min_perimeter
area_range = max_area - min_area
smoothness_range = max_smoothness - min_smoothness
compactness_range = max_compactness - min_compactness
concavity_range = max_concavity - min_concavity
concave_range = max_concave - min_concave
symmetry_range = max_symmetry - min_symmetry
fractal_range = max_fractal - min_fractal

#normalizing data
for datum in data:
  datum['radius_mean'] = (datum['radius_mean']-min_radius)/radius_range
  datum['texture_mean'] = (datum['texture_mean']-min_texture)/texture_range
  datum['perimeter_mean'] = (datum['perimeter_mean']-min_perimeter)/perimeter_range
  datum['area_mean'] = (datum['area_mean']-min_area)/area_range
  datum['smoothness_mean'] = (datum['smoothness_mean']-min_smoothness)/smoothness_range
  datum['compactness_mean'] = (datum['compactness_mean']-min_compactness)/compactness_range
  datum['concavity_mean'] = (datum['concavity_mean']-min_concavity)/concavity_range
  datum['concave points_mean'] = (datum['concave points_mean']-min_concave)/concave_range
  datum['symmetry_mean'] = (datum['symmetry_mean']-min_symmetry)/symmetry_range
  datum['fractal_dimension_mean'] = (datum['fractal_dimension_mean']-min_fractal)/fractal_range

k = 5 #number of nearest neighbors

#finding the distance between point in a multidimensional space
def distance(point_a, point_b):
  out = 0
  out += (point_a['radius_mean']-point_b['radius_mean'])**2
  out += (point_a['texture_mean']-point_b['texture_mean'])**2
  out += (point_a['perimeter_mean']-point_b['perimeter_mean'])**2
  out += (point_a['area_mean']-point_b['area_mean'])**2
  out += (point_a['smoothness_mean']-point_b['smoothness_mean'])**2
  out += (point_a['compactness_mean']-point_b['compactness_mean'])**2
  out += (point_a['concavity_mean']-point_b['concavity_mean'])**2
  out += (point_a['concave points_mean']-point_b['concave points_mean'])**2
  out += (point_a['symmetry_mean']-point_b['symmetry_mean'])**2
  out += (point_a['fractal_dimension_mean']-point_b['fractal_dimension_mean'])**2
  return out**0.5 

#finds the distance for pca arrays (not dictionaries)
def distance_pca(point_a, point_b):
  out = 0
  for i in range(len(point_a)):
    out += (point_a[i]-point_b[i])**2
  return out**0.5 

#simple prediction
def nn(test_datum, train_data):
  distances = [[distance(test_datum, neighbor), neighbor['diagnosis']] for neighbor in train_data] #arrays with distance and matching classification
  prediction = min(distances)[1] #prediction comes from the minimum of the distance
  return prediction == test_datum['diagnosis']

#finds the diagnosis of the pca data point (unlabelled during calculations)
def find_diag(datum):
  for i,j in enumerate(data2):
    for k,l in enumerate(j):
      if l==datum[1]:
        row = i
  diag = data3[row][11]
  return diag

#nearest neighbor classification for pca
def pca_nn(test_datum, train_data):
  test_diag = find_diag(test_datum)
  distances = [[distance_pca(test_datum, neighbor), find_diag(neighbor)] for neighbor in train_data]
  prediction = min(distances)[1]
  return prediction == test_diag

#knn algorithm
def knn(test_datum, train_data, k):
  distances = [[distance(test_datum, neighbor), neighbor['diagnosis']] for neighbor in train_data]
  distances.sort() #orders the distances
  radius = distances[k-1][0]
  index = k
  while distances[index][0] == radius:
    index += 1
  neighbors = distances[:index]
  labels = collections.defaultdict(lambda: 0)
  total = 0
  for neighbor in neighbors:
    neighbor[0] += 1.0/(neighbor[0]+1e-323)
    total += neighbor[0]
  for neighbor in neighbors:
    labels[neighbor[1]] = neighbor[0]/total
  max_count = -float("inf")
  prediction = None
  for label,count in labels.items():
    if count > max_count:
      max_count = count #count update
      prediction = label #updates the prediction
  return datum['diagnosis'] == prediction

#returns the distance of the kth neighbhor
def kth_nn(test_datum, train_data, k):
  distances = [distance(test_datum, neighbor) for neighbor in train_data]
  distances.sort()
  return distances[k-1]

#condensed near neighbor
def cnn_outliers(train_data, k, r):
  points = []
  for ind, datum in enumerate(train_data):
    distances = [[distance(datum,neighbor), neighbor['diagnosis']] for d, neighbor in enumerate(train_data) if d!= ind]
    distances.sort()
    nearest = distances[:k] #finding k nearest
    diff_species = 0
    for near in nearest:
      if near[1] != datum['diagnosis']:
        diff_species += 1
    if r == 1: #different radius values
      if diff_species != k:
        points.append(datum)
    elif r == k/2:
      if diff_species < k/2:
        points.append(datum)
    else:
      points.append(datum) #only appending values in the r for training
  return(points)

#same as cnn_outliers but works with the pca arrays
def cnn_outliers_pca(train_data, k, r):
  points = []
  for ind, datum in enumerate(train_data):
    distances = [[distance_pca(datum,neighbor), find_diag(neighbor)] for d, neighbor in enumerate(train_data) if d!= ind]
    distances.sort()
    nearest = distances[:k]
    diff_species = 0
    for near in nearest:
      if near[1] != find_diag(datum):
        diff_species += 1
    if r == 1:
      if diff_species != k:
        points.append(datum)
    elif r == k/2:
      if diff_species < k/2:
        points.append(datum)
    else:
      points.append(datum)
  return(points)

def pca(train_data_pca):
  #normalizing the data so the mean of each column is 0
  means = np.mean(train_data_pca, axis=0) #finding the mean of columns
  centered = train_data_pca - means #centering the mean by subtracting it
  #finding the covariance matrix
  for j in range(len(means)):
    sum = 0
    covs = []
    for k in range(len(means)):
      sum = 0
      for i in range(len(means)):
        sum += ((centered[i][j] - means[j]) * (centered[i][k] - centered[k]))
      covariance  = sum / len(means)
      covs.append(covariance) #appending to the covariance matrix
  values, vectors = eig(covs)   #calculating the eigen vectors and values
  idx = np.argsort(values) #sorting the eigen values
  values = values[idx] #indx of sorted values
  vectors = vectors[:,idx] #sorting the eigen vectors based on the values
  obj = [5,6,7,8,9,10] #the components to delete (least important)
  principal = np.delete(vectors,obj,axis=1) #deleting those components
  projection = principal.T.dot(centered.T) #projecting the data onto new axes based on eigen vectors
  projection = projection.astype(np.float)
  return((projection.T))

test_indeces = random.sample(list(range(0,50)), 10)+random.sample(list(range(51,100)),10)+random.sample(list(range(101,150)),10)
test_data = [data[i] for i in test_indeces]
train_data = [data[i] for i in range(length) if i not in test_indeces]

data2 = pca(data2) #creating train and testing data for pca
test_data_pca = [data2[i] for i in test_indeces]
test_data_pca = np.array(test_data_pca)
train_data_pca = [data2[i] for i in range(length) if i not in test_indeces]
train_data_pca = np.array(train_data_pca)

distances = [(kth_nn(datum, train_data[:index]+train_data[index+1:], k), datum) for index,datum in enumerate(train_data)]
distances.sort(key = lambda x: x[0])
#print(distances[0][0], distances[-1][0])
sns.barplot(x=list(range(len(distances))), y=[distance[0] for distance in distances])
train_data = [datum[1] for datum in distances if datum[0] <= 0.2]

#calculating the border ratios for prototypes
border_ratios = [None]*len(train_data)
for index,x in enumerate(train_data):
  neighbors = [(distance(x,y),y) for y in train_data if y['diagnosis'] != x['diagnosis']]
  y = min(neighbors, key = lambda t: t[0])
  neighbors = [(distance(x_prime, y[1]), x_prime) for x_prime in train_data if x_prime['diagnosis'] == x['diagnosis']]
  x_prime = min(neighbors, key = lambda t: t[0])
  if x == x_prime[1]:
    border_ratio = 1
  else:
    border_ratio = x_prime[0] / y[0]
  border_ratios[index] = (border_ratio, x)

#appending border ratios based on prototypes
border_ratios.sort(reverse = True, key = lambda t: t[0])
non_prototypes = [datum[1] for datum in border_ratios[1:]]
prototypes = [border_ratios[0][1]]
sentinel = True
while sentinel:
  sentinel = False
  for x in train_data:
    neighbors = [(distance(x,prototype), prototype['diagnosis']) for prototype in prototypes]
    nearest = min(neighbors, key = lambda p: p[0])
    if x['diagnosis'] != nearest[1]:
      prototypes.append(x)
      non_prototypes.remove(x)
      sentinel = True
      break

#initializing values (reocrding accuracy)
cnn_with_outliers_half_accurate = 0
cnn_without_outlier_accurate = 0
cnn_pca_accurate = 0

#prepping training data
cnn_without_outlier_data = cnn_outliers(train_data, k, 0)
cnn_with_outliers_1_data = cnn_outliers(train_data, k, 1)
cnn_with_outliers_half_data = cnn_outliers(train_data, k, k/2)

#testing
for datum in test_data:
  cnn_with_outliers_half_accurate += nn(datum, cnn_with_outliers_half_data)

for datum in test_data_pca:
  cnn_pca_accurate += pca_nn(datum,train_data_pca)

print("CNN w/o PCA (r=k/2) :", cnn_with_outliers_half_accurate, cnn_with_outliers_half_accurate/len(test_data))

print("CNN with PCA (4/10 components) :", cnn_pca_accurate, cnn_pca_accurate/len(test_data))
