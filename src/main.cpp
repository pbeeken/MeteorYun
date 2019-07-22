#include <Arduino.h>
#include <Bridge.h>
#include <Streaming.h>

String CMD = "CMD";
String RESP = "RSP";
String cmd;


void setup() { 
    Bridge.begin();
    Bridge.put(RESP, "WAIT");
    Bridge.put(CMD, "");

    Serial.begin(9600);
    while( !Serial );

    Serial << "Bridge Ready!" << endl;
    Bridge.put(RESP, "READY");

}

void loop() {
    char cmdBuffer[20];
    // Get a command
    if( Bridge.get(CMD, cmdBuffer, 20) > 0 ) {
        cmd = cmdBuffer;
        Serial << "Command: '" << cmd << "'" << endl;
        Bridge.put(RESP,"BUSY");
        // This is where we do something
        String resp = processCommand(cmd);
        Bridge.put(RESP,resp);
        delay(500);
        // we are done
        Bridge.put(CMD,"");  // empty command queue
    }
}

String processCommand(String cmd) {
    if (cmd.startsWith("DS")) { // Set digital line DS ##,H|L where ## is the pin and H or L is the value, returns OK
        int pin = (int) cmd.substring(3,4).toInt();
        int val = cmd.charAt(6)=='H'?HIGH:LOW;
        digitalWrite(pin,val);
        return "OK";
    } else
    if (cmd.startsWith("DQ")) { // Query digital line DQ ## where ## is the pin, returns 'OK H|L'
        int pin = (int) cmd.substring(3,4).toInt();
        int val = digitalRead(pin);
        return val==1?"OK H":"OK L";
    } else
    if (cmd.startsWith("AS")) { // Set analog line AS ##,### where ## is the pin and ### is the value, returns OK
        int pin = (int) cmd.substring(3,4).toInt();
        int val = (int) cmd.substring(5,7).toInt();
        analogWrite(pin,val);
        return "OK";
    } else
    if (cmd.startsWith("AQ")) { // Set analog line AQ ##,### where ## is the pin, returns 'OK ###'
        int pin = (int) cmd.substring(3,4).toInt();
        String rc = "OK ";
        rc = rc + analogRead(pin);
        return rc;
    }

}