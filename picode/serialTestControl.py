#!/usr/bin/python
"""
Run the stepper through some paces.
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
        speed = int(sys.argv[2])

 #   beat = 1/100 # 1 over period
    sp = serial.Serial(host, speed, timeout=0.5)

    sendCommand(b"ZH")
    sendCommand(b"ZV")

    sendCommand(b"LB 150") # 1/2 power laser

    time.sleep(1.0)

    sendCommand(b"LB 0")

    sp.close()
