# Serial connection
import sys
import pySerial

#main function
if __name__ == "__main__":

    msg = ""

    if (len(sys.argv) < 3) :
        host = '/dev/ttyATH0'
        speed = 115200
    else :
        host = sys.argv[1]
        port = int(sys.argv[2])

    sp = open(host,"rw")

    while True:
        msg = input("msg: ")
        if msg=="DONE":
            break
        sp.write(msg)
        resp = sp.readline(255)
        print resp

    print "We are done."