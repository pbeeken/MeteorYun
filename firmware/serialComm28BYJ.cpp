#include <Arduino.h>
#include <Streaming.h>
#include <AccelStepper.h>

//const int DEBUG
const int LED_13 = 13;

// Motor pin definitions:
const int HM_PIN1  = 5;     // IN1 on the ULN2003 driver
const int HM_PIN2  = 6;     // IN2 on the ULN2003 driver
const int HM_PIN3  = 7;     // IN3 on the ULN2003 driver
const int HM_PIN4  = 8;     // IN4 on the ULN2003 driver
const int H_LIMIT_PIN  = 2;

const int VM_PIN1  = 9;     // IN1 on the ULN2003 driver
const int VM_PIN2  = 10;    // IN2 on the ULN2003 driver
const int VM_PIN3  = 11;    // IN3 on the ULN2003 driver
const int VM_PIN4  = 12;    // IN4 on the ULN2003 driver
const int V_LIMIT_PIN  = 4;

AccelStepper horiz(AccelStepper::HALF4WIRE, HM_PIN1, HM_PIN3, HM_PIN2, HM_PIN4);
AccelStepper vert(AccelStepper::HALF4WIRE, VM_PIN1, VM_PIN3, VM_PIN2, VM_PIN4);


const int LASER_PULSE_PIN = 3;
/** 
 * Stepper setup
 */
const float MAX_SPEED = 2000.0;
const long HORIZ_BACKLASH = 2750;
const long HORIZ_LOWERLIMIT = -2000;
const long HORIZ_UPPERLIMIT = 2000;

/**
 * Forward declarations
 */
String processCommand(String cmd);
bool getCommand(String& cmd);
String digiWrite(String cmd);
String digiRead(String cmd);
String analWrite(String cmd);
String analRead(String cmd);
String setMode(String cmd);
String zeroMotor(String cmd);
void zeroMotorHoriz(long setPoint);

String Cmd = "";

/**
 * Initial setup
 */
void setup() {
    
  Serial.begin(115200);
  while( !Serial );
  Cmd.reserve(30);
  
  pinMode(H_LIMIT_PIN, INPUT_PULLUP);
  zeroMotorHoriz(HORIZ_BACKLASH);
  
  // Set the maximum steps per second:
  horiz.setMaxSpeed(MAX_SPEED); 
  horiz.setAcceleration(10000.0);

  Serial << F("ARDUINO READY") << endl;

}



/**
 *  We talk .
 */ 
void loop() {

  if( getCommand(Cmd) ) {
    Serial << "BUSY {" << Cmd << "}" << endl; 

//    String resp = "NOTHING...";
    String resp = processCommand(Cmd);
    Serial << resp << endl;
  
    // ready for next command
    Serial << F("READY") << endl;
    }

  horiz.run();
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

bool limitDetectHoriz() {
  int state = digitalRead(H_LIMIT_PIN);
  return state==0?true:false;
}

void zeroMotorHoriz(long setPoint) {
  // save the maximum speed.
  float oldMaxSpeed = horiz.maxSpeed();

  // set a slow speed with high acceleration
  horiz.setMaxSpeed(600);
  horiz.setAcceleration(1000.0);

  // temp let our current position be 0 (seems to be needed so we move backward)
  horiz.setCurrentPosition(0L);
  horiz.setSpeed(-400);
  // back into the limit switch.
  while(!limitDetectHoriz()) {
    horiz.runSpeed();
  }

  // run forward to what will be the new zero.
  horiz.setSpeed(600);
  horiz.setCurrentPosition(0L);
  horiz.runToNewPosition(setPoint);

  // restore conditions
  horiz.setCurrentPosition(0L);
  horiz.setMaxSpeed( oldMaxSpeed );
}

String zeroMotor(String cmd) {
  if (cmd.charAt(0) == 'H') {
    zeroMotorHoriz(HORIZ_BACKLASH);
    return F("OK");
    }

  if (cmd.charAt(0) == 'V') {
    return F("PASS");
  }
  return F("ERR");
}

String limitDetect(String cmd) {
  if (cmd.charAt(0)=='H') {
    bool state = limitDetectHoriz();
    return String( state==0?F("OK HIT"):F("OK OPEN") );
  }

  if (cmd.charAt(0)=='V') {
    return F("PASS");
  }

  return F("ERR");
}

String moveMotor(String cmd) {
  long steps = cmd.substring(0,5).toInt();

  if ( horiz.currentPosition()+steps >= HORIZ_LOWERLIMIT && horiz.currentPosition()+steps <= HORIZ_UPPERLIMIT ) {
    horiz.move(steps);
  } else 
    return "ERR too many steps:" + String(steps);

  return String( "OK " );
}

String moveMotorTo(String cmd) {
  long location = cmd.substring(0,5).toInt();

  if ( location >= HORIZ_LOWERLIMIT && location <= HORIZ_UPPERLIMIT ) {
    horiz.moveTo(location);
  } else 
    return "ERR too far:" + String(location);

  return String( "OK " );
}

String lightLaser(String cmd) {
  int brightness = (int) cmd.substring(0,3).toInt();
  analogWrite(LASER_PULSE_PIN, brightness);

  return F("OK");
}

String motorEnable(String cmd) {
  if (cmd.startsWith("ON"))
    horiz.enableOutputs();
  else
    horiz.disableOutputs();

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
    if (cmd.startsWith(F("ZM"))) { // Zero Motor  ZM H|V            ret OK
      retmsg = zeroMotor( cmd.substring(3) );

    } else
    if (cmd.startsWith(F("LD"))) { // Limit Detect LD              ret OK HIT|OPEN
      retmsg = limitDetect( cmd.substring(3) );

    } else
    if (cmd.startsWith(F("MM"))) { // Move Motor (rel) MM +|-####        ret OK
      retmsg = moveMotor( cmd.substring(3) );

    } else
    if (cmd.startsWith(F("MT"))) { // Move MotorTo (abs) MT +|-####     ret OK
      retmsg = moveMotorTo( cmd.substring(3) );

    } else
    if (cmd.startsWith(F("LL"))) { // Laser Light level LL ###     ret OK
      retmsg = lightLaser( cmd.substring(3) );

    } else
    if (cmd.startsWith(F("ME"))) { // Enable Motor MM ON|OFF       ret OK
      retmsg = motorEnable( cmd.substring(3) );
    } else

    if (cmd.startsWith(F("DG"))) { // DistanceToGo DG               ret OK ###
      retmsg = "OK "+String(horiz.distanceToGo());
    }else

    if (cmd.startsWith(F("QP"))) { // DistanceToGo DG               ret OK ###
      retmsg = "OK "+String(horiz.currentPosition());
    }

    return retmsg;
}

// Non-blocking command retrieval
bool getCommand(String& cmd) {
  static bool wasComplete = true;

  if (wasComplete) {
    wasComplete = false;
    cmd = "";
    delay(500);
  }

  while (Serial.available() > 0) {
    char cc;
    cc = Serial.read();
    if (cc == '\n') {
      wasComplete = true;
      return wasComplete;
    } else
      if (isPrintable(cc))
        cmd += cc;
  }
  return false;
}

/**
PM 13,O
DW 13,H
DW 13,LDW 09,L


PM 09,O
DW 09,H
DW 09,L
AW 09,050
LL 200

AW 09,150
AW 09,200
AW 09,250
ZM
AR A1
 */
