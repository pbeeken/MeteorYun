#include <Arduino.h>
#include <Bridge.h>

// The mission here is to develop firmware to control a laser pointer to simulate
// a comet going through the solar system we have projected on a wall.  (Well, once
// we get going I'll have complete x/y control of the pointer over the 10' wide 3'
// high mural.)
// This file is a naive telnet communication tool meant to talk to a primative python
// terminal emulator on the Yun. Real
//
// This is the arduino 'firmware' portion of the code that is intended to run on
// an old Arduino Yun.

const int LED_13 = 13;
const int LED_8 = 8;

const String prompt = "A>";
String cmd = "";

void setup() {
  pinMode(LED_13, OUTPUT);
  pinMode(LED_8, OUTPUT);

    // Initialize Console and wait for port to open:
  Bridge.begin();
  Console.begin();

  // Wait for Console port to connect
  while (!Console);

  Console.print(prompt);
}


void loop() {

  if (Console.available() > 0) {  // character is available
    digitalWrite(LED_13,HIGH);

    char c = Console.read(); // read the next char received
    // look for the newline character, this is the last character in the string
 
    if (c == '\n') {
      // output command
      Console.println("Command recd:'"+cmd+"'");
      Console.println(prompt);
      cmd = "";  // clear the name string
    } else {  	 // if the buffer is empty Cosole.read() returns -1
      cmd += c; // append the read char from Console to the name string
    }

  } else {
    delay(10);
    digitalWrite(LED_13,LOW);
  }

}

// String displayASCII( String input ) {
//   String ret = "";
//   for (int i=0; i<input.length(); i++ ) {
//   }
// }