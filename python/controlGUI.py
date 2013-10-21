from Tkinter import *
import device


class GUI():
	def __init__(self):
		self.dev = device.Device()
		
		self.root = Tk()
		self.speed = DoubleVar()
		self.k = DoubleVar()
		self.B = DoubleVar()
		self.invert = 0
		scale1 = Scale(self.root, variable = self.speed , from_ = 0, to = 65535, orient = HORIZONTAL, label = "speed", command = self.set_speed)
		scale1.pack(anchor=CENTER)

		checkbutton = Checkbutton(self.root, text="Invert", indicatoron = 0, variable = self.invert, command=self.set_invert)
		checkbutton.pack(anchor=CENTER)

		b1 = Button(self.root, text = "Tick Count", command = self.get_ticks)
		b1.pack(anchor=CENTER)

		b2 = Button(self.root, text = "Get Current", command = self.get_current)
		b2.pack(anchor=CENTER)

		scale2 = Scale(self.root, variable = self.k , from_ = 100, to = 0, orient = VERTICAL, label = "K", command = self.set_k)
		scale2.pack(side = LEFT)
		
		scale3 = Scale(self.root, variable = self.B , from_ = 100, to = 0, orient = VERTICAL, label = "B", command = self.set_B)
		scale3.pack(side = RIGHT)
		
		
		label = Label(self.root)
		label.pack()

		self.root.pack_propagate(0)
		self.root.mainloop()

	def set_speed(self, speed):
		self.dev.set_speed(speed)

	def set_invert(self):
		self.invert = not self.invert 
		self.dev.set_invert(self.invert)

	def set_k(self, k):
		self.k = k
		self.dev.set_k(k)

	def set_B(self, B):
		self.B = B
		self.dev.set_B(B)

	def get_ticks(self):
		ticks = self.dev.read_ticks()
		print ticks

	def get_current(self):
		current = self.dev.read_current()
		print current



g = GUI()
