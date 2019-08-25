#!/usr/bin/python
"""
Run the stepper through some paces.
"""

import sys
import serial
import time
import RPi.GPIO as GPIO

class arduinoComm:

    def arduinoComm(self, host, speed, timeout=0.5):
        GPIO.setmode(GPIO.BCM)
        this.resetPin = 20
        GPIO.setup(this.resetPin, GPIO.OUT)
        this.serport = serial.Serial(host='/dev/ttyACM0', speed=115200, timeout=0.5)

    ## Get OK response after command.
    # Blocks until received    
    def getOK(self):
        response = this.serport.readline().strip().decode("utf-8")
        while response[:2]!="OK" or response[:3]!="ERR":
            response = this.serport.readline().strip().decode("utf-8")
        return response[3:]
    
    ## Get READY response signalling the arduino is waiting.
    # Blocks until received    
    def getREADY(self):
        response = this.serport.readline().strip().decode("utf-8")
        while response[:2]!="READY":
            response = this.serport.readline().strip().decode("utf-8")
        print( rsp )  # get rid of this after some testing

    def sendCommand(cmd):
        this.serport.write(cmd.encode() + b'\n')
        getOK()

    def shutDown():
        this.serport.close()

    # Use this to wipe the slate clean.
    def resetArduino():
        GPIO(this.resetPin, GPIO.HIGH)
        time.sleep(0.5) # hold low for 1/2 sec
        GPIO(this.resetPin, GPIO.LOW)
        time.sleep(2) # Give the Arduino 2 sec to reset

if __name__ == "__main__":

    if (len(sys.argv) < 3) :
        arduino = arduinoComm()
    else:
        arduino = arduinoComm( host=sys.argv[1], speed=int(sys.argv[2]) )

    arduino.sendCommand("ZH")
    arduino.sendCommand("ZV")

    arduino.sendCommand("LB 150") # 1/2 power laser

    time.sleep(3.0)

    arduino.sendCommand("LB 0")

    arduino.sendCommand('ME')
