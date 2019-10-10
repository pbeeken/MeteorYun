#!/usr/bin/python
"""
Run the stepper through some paces.
"""
import laserControl

if __name__ == "__main__":

    if (len(sys.argv) < 3) :
        laserControl = laserControl()
    else:
        laserControl = laserControl( host=sys.argv[1], speed=int(sys.argv[2]) )

    laserControl.reZeroAxes()

    laserControl.sendCommand("LB 150") # 1/2 power laser

    time.sleep(3.0)

    laserControl.sendCommand("LB 0")

    laserControl.sendCommand('ME')
