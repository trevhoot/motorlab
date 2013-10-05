
import usb.core
import time
class Device:

    def __init__(self):
        self.SET_VALS = 0
        self.PING = 1
        self.GET_VAL = 2
        self.dev = usb.core.find(idVendor = 0x6666, idProduct = 0x0003)
        if self.dev is None:
            raise ValueError('no USB device found matching idVendor = 0x6666 and idProduct = 0x0003')
        self.dev.set_configuration()
        self.pan = 0;
        self.tilt = 0;

    def close(self):
        self.dev = None

    def set_positions(self, val1, val2):
        try:
            self.dev.ctrl_transfer(0x40, self.SET_VALS, int(val1), int(val2))
        except usb.core.USBError:
            print "Could not send SET_VALS vendor request."
    def set_pan(self, val):
      self.pan = val
      self.set_positions(self.pan, self.tilt)
    def set_tilt(self, val):
      self.tilt = val
      self.set_positions(self.pan, self.tilt)
    def read_val(self):
       try:
           ret = self.dev.ctrl_transfer(0xC0, self.GET_VAL, 0, 0, 4)
       except usb.core.USBError:
           print "Could not send GET_VAL vendor request."
       else:
           return int(ret[0]) + int(ret[1])*256;
    def time_to_dist(self, time):
      #ms_elapsed = time * 31.25 * 10**-6
      return time

    def ping(self):
        vals = []
        try:
            self.dev.ctrl_transfer(0x40, self.PING)
            startTime = time.time()
            #wait for 20 ms
            #TODO don't block here
            while (time.time() - startTime < (1/1000.0)*20):
              value = self.read_val()
              if not value in vals:
                vals.append(self.time_to_dist(value))
        except usb.core.USBError:
            print "Could not send PING vendor request."
        return vals
