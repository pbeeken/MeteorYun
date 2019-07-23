#include <Arduino.h>
#include <Streaming.h>


String processCommand(String cmd);
String getCommand(void);


// Initial setup
void setup() { 
    Serial1.begin(115200);
    while( !Serial1 );

    Serial1 << "READY" << endl;

}

// Keep examining the key/value pairs for messages.
void loop() {

  String cmd = getCommand();
  if( cmd.length() > 0 ) {
//        Serial << "Command: '" << cmd << "'"; // DEBUG
        Serial1 << "BUSY" << endl;

        // This is where we do something
        String resp = processCommand(cmd);
        Serial1 << resp << endl;
        Serial1 << "READY" << endl;
    }
}

String processCommand(String cmd) {
    if (cmd.startsWith("DW")) { // Set digital line DS ##,H|L where ## is the pin and H or L is the value, returns OK
        int pin = (int) cmd.substring(3,5).toInt();
        switch (cmd.charAt(6)) {
            case 'H': digitalWrite(pin, HIGH); break;
            case 'L': digitalWrite(pin, LOW); break;
            default: return "ERR";
            }
//        Serial << " dw p:" << pin << " v:" << cmd.charAt(6) << endl;  // DEBUG
        return "OK";
    } else
    if (cmd.startsWith("DR")) { // Query digital line DQ ## where ## is the pin, returns 'OK H|L'
        int pin = (int) cmd.substring(3,5).toInt();
        int val = digitalRead(pin);
//        Serial << "dr p:" << pin << " v:" << val << endl;
        return val==0?"OK L":"OK H";
    } else
    if (cmd.startsWith("AW")) { // Set analog line AS ##,### where ## is the pin and ### is the value, returns OK
        int pin = (int) cmd.substring(3,5).toInt();
        int val = (int) cmd.substring(5,8).toInt();
        analogWrite(pin, val);
//        Serial << " aw p:" << pin << " v:" << val << endl;  // DEBUG
        return "OK";
    } else
    if (cmd.startsWith("AR")) { // Set analog line AQ ##,### where ## is the pin, returns 'OK ###'
        int pin = (int) cmd.substring(3,5).toInt();
        String rc = "OK ";
        int val = analogRead(pin);
        rc = rc + val;
//        Serial << " ar p:" << pin << " v:" << val << endl;  // DEBUG
        return rc;
    } else
    if (cmd.startsWith("PM")) { // Set analog line PM ##,I|O|P where ## is the pin, Input, Output, InputPullup returns 'OK ###'
        int pin = (int) cmd.substring(3,5).toInt();
        switch (cmd.charAt(6)) {
            case 'I': pinMode(pin,INPUT); break;
            case 'O': pinMode(pin,OUTPUT); break;
            case 'P': pinMode(pin,INPUT_PULLUP); break;
            default: return "ERR";
        }
//        Serial << " pm p:" << pin << " v:" << cmd.charAt(6) << endl;  // DEBUG
        return "OK";
    }

    return "NULL";
}

String getCommand() {
  return Serial1.readStringUntil('\n');
}