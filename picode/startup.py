from LaserControl import LaserControl

lc = LaserControl()
lc.laserPower(0.4)
lc.reZeroAxes()
lc.laserPower(0.0)


