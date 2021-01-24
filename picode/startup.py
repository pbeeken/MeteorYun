import time
import sys
from LaserControl import LaserControl

start = time.monotonic()

if sys.platform == 'win32':
    hport = 'COM3'
else:
    hport = '/dev/ttyACM0'


lc = LaserControl(hostPort=hport)

lc.laserPower(0.4)

lc.reZeroAxes()
while (time.monotonic()-start)<10.0:
    rc = lc.queryStatus()
    print(f"{rc}")

lc.laserPower(0.0)


