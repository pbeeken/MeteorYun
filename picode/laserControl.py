import sys
import serial
import time
import RPi.GPIO as GPIO

## @class arduinoComm
# arduinoCommand Object
# encapsulates various methods for controlling the laser position.
class ArduinoComm:

    # serialPort = 0
    # resetPin = 20

    ## constructor 
    # @param speed baud rate as int
    # @param host port as a string
    # @param timeout as float (time to wait before throwing an error)
    def __init__(self, hostPort='/dev/ttyACM0', hostSpeed=115200, waitTime=0.5):
        GPIO.setmode(GPIO.BCM)
        self.resetPin = 20
        GPIO.setup(self.resetPin, GPIO.OUT)
        self.serialPort = serial.Serial(port=hostPort, baudrate=hostSpeed, timeout=waitTime)
        print( self.getRESP() )

    ## Get OK response after command.
    # Blocks until received    
    def getOK(self):
        response = self.serialPort.readline().strip().decode("utf-8")
        while response[:2]!="OK" or response[:3]!="ERR":
            response = self.serialPort.readline().strip().decode("utf-8")
        return response[3:]
    
    ## Get READY response signalling the arduino is waiting.
    # Blocks until received    
    def getREADY(self):
        response = self.serialPort.readline().strip().decode("utf-8")
        while response[:2]!="READY":
            response = self.serialPort.readline().strip().decode("utf-8")
        print( rsp )  # get rid of this after some testing

    ## Get Anyting
    def getRESP(self):
        return self.serialPort.readline().strip().decode("utf-8")

    ## send a command to the Arduino
    # @param cmd is the command that needs to be encoded for sending.
    # gets the acknolacknowledgement if command received.
    def sendCommand(self,cmd):
        self.serialPort.write(cmd.encode() + b'\n')

    ## shutdown the port
    def shutDown(self):
        self.serialPort.close()

    ## reset the Arduino by pulling a pin low.
    # Use this to wipe the slate clean.
    def resetArduino(self):
        GPIO.output(self.resetPin, GPIO.HIGH)
        time.sleep(0.5) # hold low for 1/2 sec
        GPIO.output(self.resetPin, GPIO.LOW)
        time.sleep(2) # Give the Arduino 2 sec to reset
    
    def flushInput(self):
        self.serialPort.flushInput()

## @class laserOps
# laserOps Object
# encapsulates various methods for high level constrol of the laser pointer.
class LaserControl(ArduinoComm):
    ## same constructor a s arduino
        
    ## reZeroAxes
    # reset the zeros of each axis
    def reZeroAxes(self):
        self.sendCommand("ZH")
        self.sendCommand("ZV")
        return self.getRESP()
    
    ## laserPower
    # @param val from 0.0 to 1.0
    def laserPower(self, intensity ):
        self.sendCommand(f"LB {int(255*intensity):03d}")
        return self.getRESP()

    ## moveTo
    # @param xy an array of absolute position in x and y
    def moveTo(self, xy):
        pass

    ## move
    # @param xy a delta movement from current position in x and y
    def move(self, xy):
        pass

