# Program to read serai data directly from port.
# Refer to http://www.mikeburdis.com/wp/notes/plotting-serial-port-data-using-python-and-matplotlib/ For starters

import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib import style
import numpy as np

import serial

#initialize serial port
ser = serial.Serial('/dev/ttyACM0', 9600)
# ser.port = '/dev/ttyACM0' #Arduino serial port
# ser.baudrate = 9600
ser.timeout = 10 #specify timeout when using readline()
# ser.open()
if ser.is_open==True:
	print("\nAll right, serial port now open. Configuration:\n")
	print(ser, "\n") #print serial parameters
	
