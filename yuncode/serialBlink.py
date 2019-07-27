#!/usr/bin/python
#from __future__ import print_function
"""
    This is really a speed tester. How close can we get to the claimed frequency by issuing commands
    through the python interpreter.

    Requirements: Arduino needs to be running serialComm.cpp (or variant) firmware
"""

import sys
import serial
import time

def getResp():
    rsp = sp.readline().strip()
    while rsp!=b'READY':
        print( rsp, end=',' )
        rsp = sp.readline().strip()
    print( rsp )

def sendCommand(cmd):
    sp.write(cmd + b"\n") # zero motor position
    getResp()


if __name__ == "__main__":

    if (len(sys.argv) < 3) :
        host = '/dev/ttyACM0'
        speed = 115200 # doesn't really matter as we are using USB.
    else :
        host = sys.argv[1]
        port = int(sys.argv[2])

 #   beat = 1/100 # 1 over period
    sp = serial.Serial(host, speed, timeout=0.5)

    sendCommand(b"ME ON")
    sendCommand(b"ZM")
    sendCommand(b"ME OFF")

    sendCommand(b"LL 100") # 1/2 power laser

    sendCommand(b"ME ON")
    sendCommand(b"MM 040,L")
    sendCommand(b"ME OFF")

    time.sleep(1.0)

    sendCommand(b"ME ON")
    for i in range(20):
        sendCommand(b"MM 004,L")
#        time.sleep(0.)
    sendCommand(b"ME OFF")

    sendCommand(b"ME ON")
    for i in range(20):
        sendCommand(b"MM 004,L")
        time.sleep(0.2)
        sendCommand(b"MM 004,R")
        time.sleep(0.2)
    sendCommand(b"ME OFF")


    sendCommand(b"LL 000") # laser off

    sp.close()
