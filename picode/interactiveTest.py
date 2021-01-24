from LaserControl import LaserControl
import time

lc = LaserControl(hostPort='COM3')

def wait():
    start = time.monotonic()
    while (time.monotonic()-start)<10.0:
        rc = lc.queryStatus()
        print(f"{rc}")

