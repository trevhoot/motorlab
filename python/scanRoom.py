import sys
#import device as dv
import mockDevice as dv
import time
import numpy as np

print "Starting scan to", sys.argv[1]

#on scale of 0-360

sample_yaw= 5;
sample_pitch = 5;

min_angle = 0
max_angle = 180

yaws = np.linspace(min_angle, max_angle, sample_yaw)
pitches = np.linspace(min_angle, max_angle, sample_pitch)

board = dv.Device()

output = []
for yaw in yaws:
  for pitch in pitches:
    print "scanning", yaw, pitch
    board.set_positions(yaw, pitch)
    positions = board.ping()
    output.append((yaw, pitch, positions[0]))

f = open(sys.argv[1], "w+")
for j in output:
  out = [str(x) for x in j]
  f.write(','.join(out))
  f.write('\n')
f.close()
print output



