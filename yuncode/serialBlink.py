#!/usr/bin/python
from __future__ import print_function
"""
    This is really a speed tester. How close can we get to the claimed frequency by issuing commands
    through the python interpreter.

    Requirements: Arduino needs to be running serialComm.cpp (or variant) firmware
"""

import sys
import serial
import time

if __name__ == "__main__":

    if (len(sys.argv) < 3) :
        host = '/dev/ttyATH0'
        speed = 115200
    else :
        host = sys.argv[1]
        port = int(sys.argv[2])

    beat = 1/100 # 1 over period
    sp = serial.Serial(host, speed, timeout=0.5)

    sp.write("DM 08,O\n")

    while True:
        sp.write("DW 08,H\n")
        time.sleep(beat)
        sp.write("DW 08,L\n")
        time.sleep(beat)
