#include <Arduino.h>
#include <Streaming.h>
#include <BasicStepperDriver.h>

//#define DEBUG
/** 
 * Stepper setup
 */
const int RPM = 80;
const int STEPS_PER_REV = 80;  // change this to fit the number of steps per revolution

// All the wires needed for full functionality
const int DIR = 2;
const int STEP = 4;
const int ENABLE = 7;
const int MICROSTEPS = 1;
BasicStepperDriver xaxis(STEPS_PER_REV, DIR, STEP,ENABLE);

const int LIMITDET = 3;
int  MOTORPOSITION = 0;
const int UPPERLIMIT = 550;  // number of 1/2 steps to left end from zeroing on right end

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

#define CMDPORT Serial  //  # Serial1 is too unreliable.

/**
 * Initial setup
 */
void setup() {

  #ifdef DEBUG
      Serial.begin(115200);
      while(!Serial);
      Serial << F("READY!!!") << endl;
  #endif
    
    CMDPORT.begin(115200);
    while( !CMDPORT );

    CMDPORT << F("ARDUINO READY") << endl;

    xaxis.begin(RPM, MICROSTEPS);
    xaxis.setEnableActiveState(HIGH);
    xaxis.disable(); // Shut it down.
    pinMode(LIMITDET,INPUT_PULLUP);


}

/**
 *  We talk .
 */ 
void loop() {

  String cmd = getCommand();
  if( cmd.length() > 0 ) {
    CMDPORT << F("BUSY '") << cmd << F("'") << endl;

        // This is where we do something
  #ifdef DEBUG
    Serial << "Command: '" << cmd << "'" << endl; // DEBUG
  #endif
  
        String resp = processCommand(cmd);
        CMDPORT << resp << endl;
  
  #ifdef DEBUG
    Serial << F(" +-Resp: '") << resp << F("'") << endl; // DEBUG
  #endif
        // ready for next command
        CMDPORT << F("READY") << endl;
    }
    
}

String digiWrite(String cmd) {
  // Set digital line DW ##,H|L where ## is the pin and H or L is the value, returns OK
  int pin = (int) cmd.substring(0,2).toInt();
  switch (cmd.charAt(3)) {
      case 'H': digitalWrite(pin, HIGH); break;
      case 'L': digitalWrite(pin, LOW); break;
      default: return F("ERR");
      }

  #ifdef DEBUG
    Serial << " dw p:" << pin << " v:" << cmd.charAt(6) << endl;  // DEBUG
  #endif

  return F("OK");
}

String digiRead(String cmd) {
  // Query digital line DR ## where ## is the pin, returns 'OK H|L'
  int pin = (int) cmd.substring(0,2).toInt();
  int val = digitalRead(pin);

  #ifdef DEBUG
          Serial << "dr p:" << pin << " v:" << val << endl;
  #endif

  return val==0?F("OK L"):F("OK H");
}

String analWrite(String cmd) {
  // Set analog line AS ##,### where ## is the pin and ### is the value, returns OK
  int pin = (int) cmd.substring(0,2).toInt();
  int val = (int) cmd.substring(3,5).toInt();
  analogWrite(pin, val);

  #ifdef DEBUG
    Serial << " aw p:" << pin << " v:" << val << endl;  // DEBUG
  #endif

  return F("OK");
}

String analRead(String cmd) {
  int val = 0;
  // Set analog line AQ A#,### where ## is the pin, returns 'OK ###'
  if (cmd.charAt(0)=='A') {
    int pin = (int) cmd.substring(1,2).toInt();
    val = analogRead(18+pin);
  } else
    return "ERR " + cmd;
  
  #ifdef DEBUG
    Serial << " ar p:" << pin << " v:" << val << endl;  // DEBUG
  #endif

  return "OK "+String(val);
}

String setMode(String cmd) {
  // Set analog line PM ##,I|O|P where ## is the pin, Input, Output, InputPullup returns 'OK ###'
  int pin = (int) cmd.substring(0,2).toInt();
  switch (cmd.charAt(3)) {
    case 'I': pinMode(pin,INPUT); break;
    case 'O': pinMode(pin,OUTPUT); break;
    case 'P': pinMode(pin,INPUT_PULLUP); break;
    default: return F("ERR");
  }
  #ifdef DEBUG
    Serial << " pm p:" << pin << " v:" << cmd.charAt(6) << endl;  // DEBUG
  #endif

  return F("OK");
}

String limitDetect(String cmd) {
  int state = digitalRead(LIMITDET);

  return String( state==0?F("OK HIT"):F("OK OPEN") );
}

String zeroMotor(String cmd) {
  xaxis.enable();

  // walk slowly up to limit switch
  while(digitalRead(LIMITDET)!=0) {
    xaxis.move(-1);
    delay(5);
  }

  xaxis.move(10);  // back off
  MOTORPOSITION = 0;  // set the zero point

  xaxis.disable();
  return F("PASS");
}

String moveMotor(String cmd) {
  int steps = (int) cmd.substring(0,3).toInt();
  int dir = cmd.charAt(4)=='R'? -1: 1;

  steps = dir*steps;

  if ( MOTORPOSITION+steps >= 0 && MOTORPOSITION+steps < UPPERLIMIT ) {
  //  xaxis.enable();
    xaxis.move(steps);
  //  xaxis.disable();
    MOTORPOSITION += steps;
  } else return "ERR too many steps:" + String(steps);

  return String( "POS " ) + MOTORPOSITION;
}

String lightLaser(String cmd) {
  int brightness = (int) cmd.substring(0,3).toInt();
  analogWrite(5,brightness);

  return F("OK");
}

String motorEnable(String cmd) {
  if (cmd.startsWith("ON"))
    xaxis.enable();
  else
    xaxis.disable();

  return F("OK");
}

String processCommand(String cmd) {

  String retmsg = F("NULL");

    if (cmd.startsWith(F("DW"))) { // Digital Write  DW ##,H|L    ret OK
      retmsg = digiWrite(cmd.substring(3));

    } else
    if (cmd.startsWith(F("DR"))) { // Digital Read  DR ##         ret OK H|L
      retmsg = digiRead(cmd.substring(3));

    } else
    if (cmd.startsWith(F("AW"))) { // Analog Write AW ##,###       ret OK
      retmsg = analWrite( cmd.substring(3) );

    } else
    if (cmd.startsWith(F("AR"))) { // Analog Read AR ##            ret OK ####
      retmsg = analRead( cmd.substring(3) );

    } else
    if (cmd.startsWith(F("PM"))) { // Pin Mode PM ##,I|O|P         ret OK
      retmsg = setMode( cmd.substring(3) );

    } else
    if (cmd.startsWith(F("ZM"))) { // Zero Motor  ZM               ret OK
      retmsg = zeroMotor( cmd.substring(3) );

    } else
    if (cmd.startsWith(F("LD"))) { // Limit Detect LD              ret OK HIT|OPEN
      retmsg = limitDetect( cmd.substring(3) );

    } else
    if (cmd.startsWith(F("MM"))) { // Move Motor MM ###,L|R        ret OK
      retmsg = moveMotor( cmd.substring(3) );

    } else
    if (cmd.startsWith(F("LL"))) { // Laser Light level LL ###     ret OK
      retmsg = lightLaser( cmd.substring(3) );

    } else
    if (cmd.startsWith(F("ME"))) { // Enable Motor MM ON|OFF       ret OK
      retmsg = motorEnable( cmd.substring(3) );
    }

    return retmsg;
}

String getCommand() {
  String cmd;
  char cc;
  do {
    cc = CMDPORT.read();
    if(isPrintable(cc)) cmd += cc;
  } while( cc != '\n' );

  return cmd;
}


/**
PM 13,O
DW 13,H
DW 13,LDW 09,L


PM 09,O
DW 09,H
DW 09,L
AW 09,050
AW 09,100
AW 09,150
AW 09,200
AW 09,250
ZM
AR A1
 */
