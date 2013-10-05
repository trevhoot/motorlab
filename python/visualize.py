from mayavi import mlab
import csv
import sys
import numpy as np

print sys.argv[1] 
csvFile = open(sys.argv[1])
csvRead = csv.reader(csvFile, delimiter=',')
Yaw, Pitch, R = zip(*csvRead)
Yaw = np.array([float(j) for j in Yaw])
Pitch = np.array([float(j) for j in Pitch])
R = np.array([float(j) for j in R])
X = -R*np.cos(Yaw)*np.cos(Pitch)
Y = -R*np.sin(Yaw)*np.cos(Pitch)
Z = R*np.sin(Pitch)

print "Found", len(X), "points"
mlab.figure()
mlab.points3d(X, Y, Z)
mlab.show()
