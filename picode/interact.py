"""
    This is a collection of test and sample routines to help with the building of components of the 
"""
from LaserControl import LaserControl
import time

lc = LaserControl(hostPort='COM3')

def wait():
    start = time.monotonic()
    while (time.monotonic()-start)<10.0:
        rc = lc.queryStatus()
        print(f"{rc}")

