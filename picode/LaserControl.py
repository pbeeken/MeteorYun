import sys
import serial
import time
import RPi.GPIO as GPIO

## @class arduinoComm
# arduinoCommand Object
# encapsulates various methods for controlling the laser position.
class ArduinoComm:

    # resetPin = 20


    ## constructor 
    # @param speed baud rate as int
    # @param host port as a string
    # @param timeout as float (time to wait before throwing an error)
    def __init__(self, hostPort='/dev/ttyACM0', hostSpeed=115200, waitTime=0.5):

        # set up the reset pin.
        GPIO.setwarnings(False) # for the next few lines.  When we reopen this object we get an unnecessary warning.
        GPIO.setmode(GPIO.BCM)
        self.resetPin = 20
        GPIO.setup(self.resetPin, GPIO.OUT)
        GPIO.output(self.resetPin, GPIO.LOW)
        GPIO.setwarnings(False) # Turn it back on.

        # open the serial port
        self.serialPort = serial.Serial(port=hostPort, baudrate=hostSpeed, timeout=waitTime)
        print(self.getRESP())

    ## Get OK response after command.
    # Blocks until received    
    def getOK(self):
        response = getRESP()
        while response[:2]!="OK" or response[:3]!="ERR":
            response = getRESP()
        return response
    
    ## Get READY response signalling the arduino is waiting.
    # Blocks until received    
    def getREADY(self):
        response = getRESP()
        while response[:5]!="READY":
            response = getRESP()
        return response  # get rid of this after some testing

    ## Get Anyting
    def getRESP(self):
        return self.serialPort.readline().strip().decode("utf-8")

    ## send a command to the Arduino
    # @param cmd is the command that needs to be encoded for sending.
    # gets the acknolacknowledgement if command received.
    def sendCommand(self,cmd):
        self.serialPort.flushInput()   # flush anything in the incomiong pipe so we only see the response
        self.serialPort.write(cmd.encode() + b'\n')

    ## shutdown the port
    def closePort(self):
        self.serialPort.close()

    ## reset the Arduino by pulling a pin low.
    # Use this to wipe the slate clean.
    def resetArduino(self):
        # remember details
        portSettings = self.serialPort.get_settings()
        # complete hack.  When we reset the arduino the device point /dev/ttyACM0 is
        # reactivated and /dev/ttyACM1 and visa versa so we restore the settings (prob
        # unnnecessary) and toggle the name.  This was wrong. The Leonardo seems to
        # need a full 10s or more to reset.

        # shut down the serial port and wait
        self.serialPort.close()
        time.sleep(0.5)

        # toggle the reset pin to the Arduino
        GPIO.output(self.resetPin, GPIO.HIGH)
        time.sleep(0.3) # hold low for 1/2 sec
        GPIO.output(self.resetPin, GPIO.LOW)
        time.sleep(10.0) # Give the Arduino a few to reset (yeah, it seems to need a full 10s)

        # reopen serial port the following 'apply' is prob. overkill but why not?
        self.serialPort.apply_settings(portSettings)
        # portName = self.serialPort.port
        # swap the port name (see above)  NEVERMIND.
        # if portName[-1] is '0':
        #     self.serialPort.port = portName[:-1] + '1'
        # else:
        #     self.serialPort.port = portName[:-1] + '0'
        self.serialPort.open()
        print(self.getRESP())
    
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

    def queryStatus(self):
        self.sendCommand(f"QM")
        return self.getRESP()