#include <Arduino.h>
#include <Streaming.h>
#include <BasicStepperDriver.h>

//#define DEBUG
/** 
 * Stepper setup
 */
const int RPM = 120;
const int STEPS_PER_REV = 80;  // change this to fit the number of steps per revolution

// All the wires needed for full functionality
const int DIR = 2;
const int STEP = 3;
const int ENABLE = 4;
//const int MICROSTEPS = 1;
BasicStepperDriver xaxis(STEPS_PER_REV, DIR, STEP, ENABLE);

/**
 * Forward declarations
 */
String processCommand(String cmd);
String getCommand(void);
String digiWrite(String cmd);
String digiRead(String cmd);
String analWrite(String cmd);
String analRead(String cmd);
String setMode(String cmd);
String zeroMotor(String cmd);

#define CMDPORT Serial1

/**
 * Initial setup
 */
void setup() {

  #ifdef DEBUG
      Serial.begin(115200);
      while(!Serial);
      Serial << "READY!!!" << endl;
  #endif
    
    CMDPORT.begin(115200);
    while( !CMDPORT );

    CMDPORT << "READY" << endl;

}

/**
 *  We talk .
 */ 
void loop() {

  String cmd = getCommand();
  if( cmd.length() > 0 ) {
    CMDPORT << "BUSY '" << cmd << "'" << endl;

        // This is where we do something
  #ifdef DEBUG
    Serial << "Command: '" << cmd << "'" << endl; // DEBUG
  #endif
  
        String resp = processCommand(cmd);
        CMDPORT << resp << endl;
  
  #ifdef DEBUG
    Serial << " +-Resp: '" << resp << "'" << endl; // DEBUG
  #endif
        // ready for next command
        CMDPORT << "READY" << endl;
    }
    
}

String digiWrite(String cmd) {
  // Set digital line DW ##,H|L where ## is the pin and H or L is the value, returns OK
  int pin = (int) cmd.substring(0,2).toInt();
  switch (cmd.charAt(3)) {
      case 'H': digitalWrite(pin, HIGH); break;
      case 'L': digitalWrite(pin, LOW); break;
      default: return "ERR";
      }

  #ifdef DEBUG
    Serial << " dw p:" << pin << " v:" << cmd.charAt(6) << endl;  // DEBUG
  #endif

  return "OK";
}

String digiRead(String cmd) {
  // Query digital line DR ## where ## is the pin, returns 'OK H|L'
  int pin = (int) cmd.substring(0,2).toInt();
  int val = digitalRead(pin);

  #ifdef DEBUG
          Serial << "dr p:" << pin << " v:" << val << endl;
  #endif

  return val==0?"OK L":"OK H";
}

String analWrite(String cmd) {
  // Set analog line AS ##,### where ## is the pin and ### is the value, returns OK
  int pin = (int) cmd.substring(0,2).toInt();
  int val = (int) cmd.substring(3,5).toInt();
  analogWrite(pin, val);

  #ifdef DEBUG
    Serial << " aw p:" << pin << " v:" << val << endl;  // DEBUG
  #endif

  return "OK";
}

String analRead(String cmd) {
  // Set analog line AQ ##,### where ## is the pin, returns 'OK ###'
  int pin = (int) cmd.substring(0,2).toInt();
  int val = analogRead(pin);

  #ifdef DEBUG
    Serial << " ar p:" << pin << " v:" << val << endl;  // DEBUG
  #endif

  return "OK "+val;
}

String setMode(String cmd) {
  // Set analog line PM ##,I|O|P where ## is the pin, Input, Output, InputPullup returns 'OK ###'
  int pin = (int) cmd.substring(0,2).toInt();
  switch (cmd.charAt(3)) {
    case 'I': pinMode(pin,INPUT); break;
    case 'O': pinMode(pin,OUTPUT); break;
    case 'P': pinMode(pin,INPUT_PULLUP); break;
    default: return "ERR";
  }
  #ifdef DEBUG
    Serial << " pm p:" << pin << " v:" << cmd.charAt(6) << endl;  // DEBUG
  #endif

  return "OK";
}

String zeroMotor(String cmd) {
  return "PASS";
}


String processCommand(String cmd) {
    if (cmd.startsWith("DW")) {
      return digiWrite(cmd.substring(3));

    } else
    if (cmd.startsWith("DR")) {
      return digiRead(cmd.substring(3));

    } else
    if (cmd.startsWith("AW")) {
      return analWrite( cmd.substring(3) );

    } else
    if (cmd.startsWith("AR")) {
      return analRead( cmd.substring(3) );
    } else
    if (cmd.startsWith("PM")) {
      return setMode( cmd.substring(3) );
    } else
    if (cmd.startsWith("ZR")) {
      return zeroMotor( cmd.substring(3) );
    }
    return "NULL";
}

String getCommand() {
  String cmd;
  char cc;
  do {
    cc = CMDPORT.read();
    if(cc>0 && !isControl(cc)) cmd += cc;
  } while( cc != '\n' );

  return cmd;
}
