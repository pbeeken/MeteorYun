#include <Arduino.h>
#include <Bridge.h>
#include <Stream.h>

const char CMD[] = "CMD";
const char RESP[] = "RSP";
String cmd;


void setup() { 
    Bridge.begin();
    Bridge.put("RSP", "READY");
}

void loop() {
    // If our command string is null we are in trouble.
    if (!cmd.reserve(20)) return;
    // Get a command
    Bridge.get(CMD, cmd.begin(), 20);

    println("I got ")
}