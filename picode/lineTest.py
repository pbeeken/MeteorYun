"""
This is a test that draws a box around the limits of the 'drawing' area
"""
import time
from LaserControl import LaserControl

def main():
    """
    We presume that the system has been calibrated and zeroed.
    """
    start = time.monotonic()
    limit = 1600

    lc = LaserControl()

    lc.laserPower(0.2)

    rc = lc.gotoVert(-limit)
    print(f"{rc}")
    lc.gotoHoriz(limit)
    print(f"{rc}")

    while (time.monotonic()-start) < 3.0:
        rc = lc.queryStatus()
        print(f"{rc}")

    # lc.gotoHoriz(-limit)
    # lc.gotoVert(limit)
    # lc.gotoHoriz(limit)
    # lc.gotoVert(-limit)

    lc.laserPower(0.0)


if __name__ == "__main__":
    main()