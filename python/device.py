
import usb.core
import time
class Device:

	def __init__(self):
		self.SET_SPEED = 0
		self.GET_TICKS = 1
		self.GET_EMF = 2
		self.GET_CURRENT = 3
		self.SET_CONSTANTS = 4
		self.dev = usb.core.find(idVendor = 0x6666, idProduct = 0x0003)
		if self.dev is None:
			raise ValueError('no USB device found matching idVendor = 0x6666 and idProduct = 0x0003')
		self.dev.set_configuration()
		self.speed = 0
		self.invert = 0
		self.k = 0
		self.B = 0

	def close(self):
		self.dev = None

	def set_speed(self, speed):
		self.speed = speed
		self.set_motor()
	
	def set_invert(self, invert):
		self.invert = invert
		self.set_motor()

	def set_motor(self):
		try:
			self.dev.ctrl_transfer(0x40, self.SET_SPEED, int(self.speed), self.invert)
		except usb.core.USBError:
			print "Could not send SET_SPEED vendor request."

	def set_k(self, k):
		self.k = k
		self.set_constants()

	def set_B(self, B):
		self.B = B
		self.set_constants()

	def set_constants(self):
		try:
			self.dev.ctrl_transfer(0x40, self.SET_CONSTANTS, int(self.k), int(self.B))
		except usb.core.USBError:
			print "Could not send SET_SPEED vendor request."

	def read_speed(self):
	   try:
		   ret = self.dev.ctrl_transfer(0xC0, self.GET_SPEED, 0, 0, 4)
	   except usb.core.USBError:
		   print "Could not send GET_SPEED vendor request."
	   else:
		   return int(ret[0]) + int(ret[1])*256;

	def read_EMF(self):
	   try:
		   ret = self.dev.ctrl_transfer(0xC0, self.GET_EMF, 0, 0, 4)
	   except usb.core.USBError:
		   print "Could not send GET_EMF vendor request."
	   else:
		   return int(ret[0]) + int(ret[1])*256;

	def read_current(self):
	   try:
		   ret = self.dev.ctrl_transfer(0xC0, self.GET_CURRENT, 0, 0, 4)
	   except usb.core.USBError:
		   print "Could not send GET_CURRENT vendor request."
	   else:
		   return int(ret[0]) + int(ret[1])*256;

	def read_ticks(self):
	   try:
		   ret = self.dev.ctrl_transfer(0xC0, self.GET_TICKS, 0, 0, 4)
	   except usb.core.USBError:
		   print "Could not send GET_TICKS vendor request."
	   else:
		   return int(ret[0]) + int(ret[1])*256;

