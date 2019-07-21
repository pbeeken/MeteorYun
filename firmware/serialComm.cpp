#include <Arduino.h>
#include <Bridge.h>

// The mission here is to develop firmware to control a laser pointer to simulate
// a comet going through the solar system we have projected on a wall.  (Well, once
// we get going I'll have complete x/y control of the pointer over the 10' wide 3'
// high mural.)
// This file is a naive serial communication tool meant to talk to a primative python
// terminal emulator on the Yun.
//
// This is the arduino 'firmware' portion of the code that is intended to run on
// an old Arduino Yun.

const int LED_13 = 13;
const int LED_8 = 8;

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_13, OUTPUT);
  pinMode(LED_8, OUTPUT);

  // begin bridge
  Bridge.begin();
  Console.begin();

  while (!Console);
  Console.println("TALKTOME");

}


void loop() {

  String msg = Console.readString();
  Console.print( "I got: " );
  Console.println( msg );

}