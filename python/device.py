
import usb.core

class Device:

    def __init__(self):
        self.SET_VALS = 0
        self.PING = 1
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

    def ping(self):
        try:
            self.dev.ctrl_transfer(0x40, self.PING)
        except usb.core.USBError:
            print "Could not send PING vendor request."

