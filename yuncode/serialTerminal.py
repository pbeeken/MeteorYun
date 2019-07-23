from __future__ import print_function
# Serial connection
import sys
import serial

#main function
if __name__ == "__main__":

    msg = ""

    if (len(sys.argv) < 3) :
        host = '/dev/ttyATH0'
        speed = 115200
    else :
        host = sys.argv[1]
        port = int(sys.argv[2])

    sp = serial.Serial(host, speed, timeout=0.5)

    while True:
        msg = input("msg: ")
        if msg=="DONE":
            break
        sp.write(msg)
        
        resp = sp.read(255)
        print( resp )

    print( "We are done." )