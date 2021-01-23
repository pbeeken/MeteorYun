# Overall Plan
We want to develop a 'laser scanner' which allow someone to programatically direct a laser pointer at a wall.  The 'target' is a large mural of the planets which are arranged in a scaled view of the Solar System. One application of this device is to simply emulate the motion of a comet. The beam can be used to highlight a planet or group.  It can be used as an "alien spaceship" or any variety of ideas.  The point is to make the device flexible and programmable from a distant device.


## First Approach...
There are many possible programs to run on the Arduino we are trying to develop a kind of firmware that allows control of the Arduino from python on the ATHOS linux supervisor. Since platformio, by default, compiles and loads main.cpp as the entrypoint we create a hardlink to the firmware version we wish to load through the following:

```
rm src/main.cpp  # delete existing link if defined.
ln firmware/<firmwarefile>.cpp src/main.cpp # create a link
```

What this means is that when we commit we get two copies of the existing file (the original firmware code and the link).  

## Abandon the ATHOS...  
The YUN turns out to have some real problems with communication. It was releasesd
in close proximity to the much cheaper RaspberryPi. Why buy the more expensive device when you could use the RaspberryPi as the controller (with a full fledged **Linux**OS) rather than a severely restricted **OpenWrt**OS.  The device never seemed to catch on and little development work seemed to be accumulated. The 'Bridge' has a great deal of lag in its communications and efforts to run a serial connection was frought with errors in the transmission at high speed.

## Communications
Rather than the Athos/Arduino (read: *Yun*) we will use a RaspberryPi 3B+ as the primary controller with the Arduino as the Firmware system.

### Why this combo? 
Why not just a RaspberryPi? Couple of reasons:
  1. Availability: I have an aging Leonardo and the Pi is also largely unused.
  2. Ease of interfacing the Arduino: The Arduino is a 5V (TTL) level controller with robust in/output control. The Pi is a 3.3V device which requires some special elecronics to develop an interface.  I'd have to buy and configure special interfaces to make this work.  See 1.

## Plan now
The plan is the same.  The final device is meant to be mounted high up on a ceiling panel where my only regular access is via *WiFi*. The supervisor can be reprogrammed to control the device where the Arduino manages the details.

Overall strategy:
  1. Program the Arduino as a device (with 'Firmware') to be controlled by the supervisor.
  2. Use Python on the Pi as a supervisor to control the Arduino.

### Firmware:
The Arduino needs a set of tools to get and respond to requests for doing things with the electronics.  The only purchase I have made is a box of some cheap stepper motors which come with a simple transistor array to power the steppers in an open loop fashion. We use angle to control the beam but want to avoid non-linearities in tracking at steep angles.  There is a simple mechanical mechanism to compensate a rotational actualtor to create a linear track to rotational position involving a rack and pinion or threaded rod and a sloted arm.  I use both for different reasons but the upshot is that I get a direct relationship between a step and a change in position of the beam on a wall that doesn't change depending on what the angle of the beam (w.r.t. the center).

Observing and working with the stepper positioning of 3D printers which also operate open loop after calibration reveals when things go wrong. The steppers are very strong and can destroy the plastic framework. The 3D printers handle this by using metal components and 'slip gears' so that an error (read: overshoot) can be tolerated. I want the Firmware to do some checking on the requests so it doesn't overshoot (I can't prevent everything but at least I can check).  

Some of the things we need...
  1. ...a mechanism of calibration so that we can set a reference point.  
    - I use two unused limit switches to walk the platform to a point where we can define zero.
  2. ...commands to know how many steps we have moved.
  3. ...commands to move a requisite set of steps in either direction.
  4. ...commands to move to a paricular position.
  4. ...feedback on the success or failure of command execution.

#### methods:
  - `zeroHorz()    # zero the horizontal axis`  (do this at init and on request)
  - `zeroVert()    # zero the vertical axis`  (do this at init and on request)
  - `moveHorz()    # move relative to current position`  (step)
  - `moveVert()    # move relative to current position`  (step)
  - `moveToHorz()  # move to an absolute position`  (direct)
  - `moveToVert()  # move to an absolute position`  (direct)
  - `getPosVert()  # get the internal position`  (direct)
  - `getPosHorz()  # get the internal position`  (direct)
  - `laserBright() # set the laser intensity`  (0-off, 255-full on)
  - `queryMotors() # report if the motors are still moving`  (It is my habit to make non-blocking routines. It is possible to issue commands that arrive before a current action is complete.  We need a way to slow the commands down when needed so we don't get ahead of ourselves)

#### commands:
All commands are terminated with a **␍** (aka '\r')
  - `LH` detect limit on horizontal bar
    - returns OK HIT or OK OPEN
  - `LV` detect limit on vertical bar
    - returns OK HIT or OK OPEN
  - `ZH` zero horizontal motor
    - returns OK when done (blocks)
  - `ZV` zero vertical motor
    - returns OK when done (blocks)
  - `MH #####` move Δh to new location ##### is a signed value.
    - takes a signed value
    - returns OK #### steps to go (non-blocking)
  - `MV #####` move Δv to new location ##### is a signed value.
    - takes a signed value
    - returns OK #### steps to go (non-blocking)
  - `GH #####` moveTo [*Go*] to horizontal location, ##### is a signed value.
    - takes a signed value
    - returns OK #### steps to go (non-blocking)
  - `GV #####` moveTo [*Go*] to vertical location, ##### is a signed value.
    - takes a signed value
    - returns OK #### steps to go (non-blocking)
  - `QH` get current horizontal position.
    - returns OK #### current stored position
  - `QV` get current vertical position.
    - returns OK #### current stored position
  - `LB ###` set brightness to value ### (0 is off)
    - returns OK
  - `QM` get current movement status.
    - returns OK -V H- or -- to indicate incomplete movement in the given axis
  - `ME cc` enable or disable motors (to cool down)
    - takes H-, -V or HV to enable motors
    - takes -- to disable motors
    - return OK  


upon successful acknowledgement the command returns an 'OK' with some optional additional informantion that gives some feedback on what was understood.  READY indicates the Arduino is ready for another command. (BTW, This doesn't mean the action, e.g. movement, is complete as indicated above)  

### Supervisor:
Does the heavy lifting in terms of executing the pattern control.  The supervisor will calculate the paths and send the x,y positions to the firmware.  We need a robust communication protocol to facilitate this connection.

