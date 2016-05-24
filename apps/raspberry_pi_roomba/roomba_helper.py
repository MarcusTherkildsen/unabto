import serial
import struct
from time import sleep
import numpy as np
import sys
import RPi.GPIO as GPIO
import argparse

#print 'Number of arguments:', len(sys.argv), 'arguments.'
#print 'Argument List:', str(sys.argv)

def initialise_serial():
	print "Initialising serial"
	ser = serial.Serial('/dev/ttyUSB0', baudrate=115200, timeout=0.1)
		
	if(ser.isOpen() == False):
		print "Opening serial port"
		ser.open()
	else:
		print "Serial port already open"
		pass

	sleep(0.2)
	return ser

def write_serial(ser, values):

	if str(type(values)) == "<type 'int'>":
		s = struct.pack('!B', values)
		ser.write(s)
	elif str(type(values)) == "<type 'tuple'>":
		s = struct.pack('!{0}B'.format(len(values)), *values)
		ser.write(s)
	else:
		print "Input format not understood, exiting"
	sleep(0.2)

def initialise_pins(i):
	print "Initialising pins"
	# Disable warnings
	GPIO.setwarnings(False)

	# Set wiringPi pin numbering
	GPIO.setmode(GPIO.BOARD)
	GPIO.setup(i, GPIO.OUT)

def initialise_wake_pin(i):
	print "Initialising wake pin"
	# Initialise pin to low
	GPIO.output(i, GPIO.LOW)

def wake_roomba(i):
	print "Waking Roomba"
	GPIO.output(i, GPIO.HIGH)
	sleep(0.5)
	GPIO.output(i, GPIO.LOW)

################################### Callin' ###########################

# wiringPi pin number to activate the optocoupler
i = 7

# Normal calling order

# unabto_main should do this
# system("python /home/pi/roomba_helper.py initialise &");

# unabto_application will, for now have a "clean" and "stop" function
# system("python /home/pi/roomba_helper.py clean &");
# system("python /home/pi/roomba_helper.py stop &");

if len(sys.argv) < 2:
	print 'No arguments given, doing nothing'

elif sys.argv[1] == 'initialise':
	initialise_pins(i)
	initialise_wake_pin(i)
	wake_roomba(i)
	ser = initialise_serial()
	#print "Going into safe mode"
	#write_serial(ser, (128, 131))
	print "Set display to NAb T/O"
	write_serial(ser, (163, 55, 119, 124, 93))

elif sys.argv[1] == 'clean':
	initialise_pins(i)
	wake_roomba(i)
	ser = initialise_serial()
	#print "Going into safe mode"
	#write_serial(ser, (128, 131))
	print "Set display to NAb T/O"
	write_serial(ser, (163, 55, 119, 124, 93))
	print "Starting normal clean cycle"
	write_serial(ser, 135)

elif sys.argv[1] == 'stop':
	initialise_pins(i)
	wake_roomba(i)
	ser = initialise_serial()
	print "Stopping Roomba"
	write_serial(ser, (133))
