import sys
import device as dv
#import mockDevice as dv
import time
import numpy as np

def int_to_deg(integer):
	return int(integer*180./65535)

def deg_to_int(degree):
	return int(degree*65535./180)

def int_to_rad(integer):
	return integer*np.pi/65535

print "Starting scan to", sys.argv[1]

#on scale of 0-180 = 0 - 65535

sample_yaw= 15;
sample_pitch = 15;

min_angle = 1000
max_angle = 64000

yaws = np.linspace(min_angle, max_angle, sample_yaw)
pitches = np.linspace(min_angle, max_angle, sample_pitch)
print yaws
print pitches
board = dv.Device()

output = []
for yaw in yaws:
	yawRad = int_to_rad(yaw)
	for pitch in pitches:
		board.set_positions(yaw, pitch)
		time.sleep(0.25)
		pitchRad = int_to_rad(pitch)
		print "scanning", yawRad, pitchRad
		for i in range(10):
			positions = board.ping()
		print positions
		if len(positions) == 1:
			pass
		elif positions[1] < 8250:
			pass
		else:
			output.append((yawRad, pitchRad, positions[1]))

f = open(sys.argv[1], "w+")
for j in output:
	out = [str(x) for x in j]
	f.write(','.join(out))
	f.write('\n')
f.close()
print output



