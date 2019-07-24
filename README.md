# Meteor

The goal here is to develop an x-y "laser painter" that can cover a 10' wide, 3' high 3D display of our solar system.

The actuators are linear stepper motors scavenged from old CD drives. The x axis is a classic slide rotator translation

which provides a linearly scaled $x$ and $\dot{x}$ translation of the motion of the actuator.

- s: linear position of actuator (linearly proportional to rotation of stepper) [width: $\pm$ 15mm]
- d: distance from arm axis perpendicular to pin path [32 mm]
- l: distance to wall [3200 mm]
- x: position along the wall [width: $\pm$ 1500mm]

$$\tan(\theta)=\mathcal{s}/\mathcal{d} \cdots \tan(\theta)=x/\mathcal{l}$$

$$x/ \mathcal{l} = \mathcal{s}/\mathcal{d} \Rightarrow x = \mathcal{s} \cdot \mathcal{l}/\mathcal{d}$$

The y axis is more complicated but since it is over a shorter distance linear approximations can be applied.

## Supplies
 - An old Arduino Yun. very servicable and provides internet (WiFi and Ethernet) connectivity, especially useful given the final assembly will be mounted on the ceiling.
 - A small supply of old CD linear acutators.
 - A small bucket of left over limit switches.
 - Two Pololu DRV8834 stepper controllers.
 - other assorted left overs from other projects.

## Hardware strategy
 - Onshape has the design of the [Base Mount](https://cad.onshape.com/documents/a09b76387c29de53a9b7bb77/w/2c0b749e489004aa757cb41f/e/bc97e45bc6b50e7f7d630fe8).
 - An old RadioShack prototyping shield will hold the stepper controllers and assundry electronics for controlling the axis.
   - Each axis has a limit switch to servo the position.
   - Consider being able to select 2 different speeds for axis
 - Yun allows WiFi access so we can 'tele-commute'
   - [communication](http://mohanp.com/arduino-yun-serial-port-over-tcp/) between Athos9331 and 32u4 is well summarized but obscure.
   - We have a choice: A **telnet** emulator or straight **serial** connection.
      - advantage to **serial** is speed and simplicity.
        - *serial isn't really an option.*
        - Launching the [**Bridge**](https://jpmens.net/2013/09/23/understanding-arduino-yun-s-bridge/) library occupies the **Serial1** port 
        - Overriding the Bridge library means bypassing all the communications
        - we would need to write our own protocol  
      - advantage to **telnet** is the ability to bypass Yun 
        - *not so easy*
        - Yun is still a passthrough 
        - I haven't been successful on connecting directly to the Arduino as the telnet client is fake.
      - Third way as a result of reading about the [**Bridge**](https://jpmens.net/2013/09/23/understanding-arduino-yun-s-bridge/) design is to use messaging.
        - the base class Bridge allows for the creation of key/value pairs
        - This happens very fast and information uses memory on the Yun
        - A protocol can easily be setup using messaging.
        - Bridge ensures accurate communications.
    - After some experiments I direct serial communication is the way to go. I can maintain realtime 100Hz (possibly faster) communication 
    between the ATHOS and the Arduino.  I abandon the Bridge archetecture and wrote my own protocol (which is modelled after the messaging system)

## Compilation
Look for [reference](https://github.com/arduino/Arduino/wiki/Build-Process) on build methodology to understand all the problems that cropped up on this project.

Platformio, by default, picks out the file called main.cpp in the src folder. I have several versions of the '_firmware_' that is required to run on the Arduino depending on the communication plan. What I do is keep the different "*firmwares*" in a folder (of that name) and create a hard link to a file called main.cpp in src. 

## Yun experiments
I have tried many different variants of facilitating communication between the Yun and the Arudino.  

### HTTP Server
I tried _**httpServer**_ which seemed to work most reliably but incurred the most overhead.  

### TELNET
I tried _**telnet**_ which I believed would allow operation from the Yun as well as a remote device.  This was slow and didn't allow outside communication because the telnet client wasn't complete.  

### BASE messaging
We then went to the _**base**_ messaging protocol (_key/value pairs_) but revealed the source of the low bandwidth. All these other protocols relied of this messaging. Finally I went to total serial control.  

### Serial Control
There are two serial (pseudo serial) ports.  One is accessible through the USB port which connects to the Arduino (**Serial**) and the other is an internal (**Serial1**)and hardwired to the ATHOS processor.  As long as you never launch the Bridge object the bridge software doesn't take over this port.  If only it were that easy.  Evidently the port on the ATHOS side gets poluted with some kind of crossover with a logging port and I can't use it to communicate with the Arduino reliably.  THIS IS THE NIGHTMARE PART: it seemed random.  I got it to work and after some time it would stop.  I played with a number of controls and background processes but couldn't get reliable connection through **Serial1**.  Control through **Serial** worked, however.
