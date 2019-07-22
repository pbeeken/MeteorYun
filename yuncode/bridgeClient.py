#!/usr/bin/python

import time
import sys

sys.path.insert(0, '/usr/lib/python2.7/bridge/')
from bridgeclient import BridgeClient as bridgeclient

#main function
if __name__ == "__main__":

    bc = bridgeclient()


    while True:
            message = bc.get("CMD")
            print message
            bc.put('hello','world')
            time.sleep(1)
            bc.put('hello','you')
