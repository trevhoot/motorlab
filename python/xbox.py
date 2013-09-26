import device
import pygame

def main():
  dev = device.Device()
  pygame.init()
  pygame.joystick.init()
  xbox = pygame.joystick.Joystick(0)
  xbox.init()

  done = False
  while not done:
    for event in pygame.event.get(): #also updates the joystick objects
      if event.type == pygame.QUIT:
        done=True

    #Direct pan tilt control
    x = (xbox.get_axis(0)+1) * 65535/2
    y = (-xbox.get_axis(4)+1) * 65535/2
    dev.set_positions(x,y)

if __name__ == "__main__":
  main()
