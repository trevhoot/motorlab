import time
class Device:
    def __init__(self):
        self.pan = 0;
        self.tilt = 0;

    def close(self):
        self.dev = None

    def set_positions(self, val1, val2):
      pass
    def set_pan(self, val):
      self.pan = val
      self.set_positions(self.pan, self.tilt)
    def set_tilt(self, val):
      self.tilt = val
      self.set_positions(self.pan, self.tilt)
    def read_val(self):
      ret = [1, 2, 3]
      return int(ret[0]) + int(ret[1])*256 + int(ret[2])*256*256;

    def ping(self):
        vals = []
        startTime = time.time()
        #wait for 20 ms
        #TODO don't block here
        while (time.time() - startTime < (1/1000.0)*20):
          value = self.read_val()
          if not value in vals:
            vals.append(value)
        return vals
