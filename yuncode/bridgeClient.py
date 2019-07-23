#!/usr/bin/python

import time
import sys

sys.path.insert(0, '/usr/lib/python2.7/bridge/')
from bridgeclient import BridgeClient as bridgeclient

#main function
if __name__ == "__main__":

    CMD = "CMD"
    RESP = "RSP"


    bc = bridgeclient()

    message = ""
    while message != "READY":
        message = bc.get(RESP)

    bc.put(CMD,'PM 08,O')

    while True:
        bc.put(CMD,'DW 08,H')
        # result = bc.get(RESP)
        # print result
        time.sleep(0.1)
        bc.put(CMD,'DW 08,L')
        # result = bc.get(RESP)
        # print result
        time.sleep(0.1)
