from mayavi import mlab
import csv
import sys
import numpy as np

def time_to_dist(time):
    dist = float(time)/300.
    return dist

print sys.argv[1] 
csvFile = open(sys.argv[1])
csvRead = csv.reader(csvFile, delimiter=',')
Yaw, Pitch, R = zip(*csvRead)

'''
Yaw = list(Yaw)
Pitch = list(Pitch)
R = list(R)

'''
Yaw = np.array([float(j) for j in Yaw])
Pitch = np.array([float(j) for j in Pitch])
R = np.array([float(time_to_dist(j)) for j in R])
X = -R*np.cos(Yaw)*np.cos(Pitch)
Y = -R*np.sin(Yaw)*np.cos(Pitch)
Z = R*np.sin(Pitch)
'''
X = []
Y = []
Z = []
Value = []
for i in range(len(Yaw)):
	yaw = float(Yaw[i])
	pitch = float(Pitch[i])
	r = 10*float(time_to_dist(R[i]))
	X.append(-r*np.cos(yaw)*np.cos(pitch))
	Y.append(-r*np.sin(yaw)*np.cos(pitch))
	Z.append(r*np.sin(pitch))
	Value.append(0.5)
'''
print 'data:'
print X
print Y
print Z
value = [0.6] * len(X)
value[1] = 0.5
print "Found", len(X), "points"
#mlab.figure()
mlab.points3d(X, Y, Z, value)
mlab.show()
