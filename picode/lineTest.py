"""
This is a test that draws a box around the limits of the 'drawing' area
"""
import time
from LaserControl import LaserControl

def main():
    """
    We presume that the system has been calibrated and zeroed.
    """
    lc = LaserControl()

    lc.laserPower(0.2)

    limit = 800
    wait = 3.0

    rc = lc.goto([ limit,  limit])
    print(f"{rc}")
    rc = lc.getREADY()  # wait for the arduino to be done
    print(f"{rc}")
#    time.sleep(wait)

    rc = lc.goto([ limit, -limit])
    print(f"{rc}")
    rc = lc.getREADY()  # wait for the arduino to be done
    print(f"{rc}")
#    time.sleep(wait)

    rc = lc.goto([-limit, -limit])
    print(f"{rc}")
    rc = lc.getREADY()  # wait for the arduino to be done
    print(f"{rc}")
#    time.sleep(wait)

    rc = lc.goto([-limit,  limit])
    print(f"{rc}")
    rc = lc.getREADY()  # wait for the arduino to be done
    print(f"{rc}")
#    time.sleep(wait)

    lc.goto([ limit,  limit])
    print(f"{rc}")
    rc = lc.getREADY()  # wait for the arduino to be done
    print(f"{rc}")
#    time.sleep(2*wait)    

    lc.goto([0,0])    
    lc.laserPower(0.0)





if __name__ == "__main__":
    main()
