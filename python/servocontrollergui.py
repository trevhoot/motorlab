from Tkinter import *
import device


class Slider():
  def __init__(self):
    self.pan = 0.5*65536
    self.tilt = 0.5*65536

  def set_pan(pan):
    self.pan = pan

  def set_tilt(tilt):
    self.tilt = tilt

  def get_pan():
    return self.pan

  def get_tilt():
    return self.tilt

def sel():
   selection = "Value = " + str(ping)
   label.config(text = selection)

dev = device.Device()

def percent_to_fixed(val):
  return int(int(val) * 655.35)


def pan(value):
  dev.set_pan(percent_to_fixed(value))

def tilt(value):
  dev.set_tilt(percent_to_fixed(value))

root = Tk()
val1 = DoubleVar()
val2 = DoubleVar()
ping = 0 #This will be the value returned in miniproject 3.
scale1 = Scale( root, variable = val1 , length = 65536, orient = HORIZONTAL, label = "pan", command = pan)
scale1.pack(anchor=CENTER)
scale2 = Scale( root, variable = val2 , length = 65536, orient = HORIZONTAL, label = "tilt", command = tilt)
scale2.pack()

button = Button(root, text="Ping", command=sel)
button.pack(anchor=CENTER)

label = Label(root)
label.pack()
root.pack_propagate(0)
root.mainloop()
