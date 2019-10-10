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
        print(self.getRESP())

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
        return response[3:]  # get rid of this after some testing

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
    ## same constructor as arduino
    aspectRatio = 60.5/13.  # xdis/ydis

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

    ## goTo
    # @param xy an array of absolute position in x and y
    def goto(self, xy):
        self.sendCommand(f"GT {int(xy[0]):4d},{int(xy[1]):4d}")
        return self.getRESP()

    ## goTo verticaly
    # @param y an absolute position in y
    def gotoVert(self, y):
        self.sendCommand(f"GV {int(y):4d}")
        return self.getRESP()

    ## goTo horizontally
    # @param x an absolute position in x
    def gotoHoriz(self, x):
        self.sendCommand(f"GH {int(x):4d}")
        return self.getRESP()

    ## move
    # @param xy a delta movement from current position to x and y
    def move(self, xy):
        self.sendCommand(f"MT {int(xy[0]):4d},{int(xy[1]):4d}")
        return self.getRESP()

    ## move verticaly
    # @param y a delta movement from current position to y
    def moveVert(self, y):
        self.sendCommand(f"MV {int(y):4d}")
        return self.getRESP()

    ## move horizontal
    # @param x a delta movement from current position to x
    def moveHoriz(self, x):
        self.sendCommand(f"MH {int(x):4d}")
        return self.getRESP()

    ## query horizontal distance
    def queryHoriz(self):
        self.sendCommand(f"QH")
        return self.getRESP()

    ## query horizontal distance
    def queryVert(self):
        self.sendCommand(f"QV")
        return self.getRESP()
