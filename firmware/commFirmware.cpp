#include <Arduino.h>
#include <Streaming.h>
#include <AccelStepper.h>
//#include <MultiStepper.h>

//const int DEBUG
const int LED_13 = 13;
const String OK_RESP = String("OK");
const String ERR_RESP = String("ERR");

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

AccelStepper horz(AccelStepper::HALF4WIRE, HM_PIN1, HM_PIN3, HM_PIN2, HM_PIN4);
AccelStepper vert(AccelStepper::HALF4WIRE, VM_PIN1, VM_PIN3, VM_PIN2, VM_PIN4);
//MultiStepper combo;

const int LASER_PULSE_PIN = 3;
/** 
 * Stepper setup
 **/
const float MAX_SPEED = 2000.0;
const long horz_BACKLASH = 2250;  // distance to restore to center when limit switch 
const long horz_LOWERLIMIT = -2000;
const long horz_UPPERLIMIT = 2000;

const long vert_BACKLASH = 2000;  // distance to restore to center when limit switchb
const long vert_LOWERLIMIT = -2000;
const long vert_UPPERLIMIT = 2000;

/**
 * TODO: Need to set upper and lower limits for movement
 * NB: we have an open loop system and other than the initial zeroing
 * don't have any feedback as to where we are. The mechanics is not
 * set up for robust overrun (overruns may break the plastic holders)
 * so we need to put some margins on where we are allowed based on the
 * internal counters, at least.
 **/



/**
 * Forward declarations
 **/
String processCommand(String cmd);
bool getCommand(String& cmd);
void zeroMotorHorz(long);
void zeroMotorVert(long);

/**
 * Command static 
 **/
String Cmd = "";

/**
 * Initial setup
 */
void setup() {

  // setup the serical connection
  Serial.begin(115200);
  while( !Serial );
  Cmd.reserve(30);
  
  // setup the limit switches 
  // (n.b. because the switches are debounced and 
  //       processed a pullup prob. isn't necessary)
  pinMode(H_LIMIT_PIN, INPUT_PULLUP);
  pinMode(V_LIMIT_PIN, INPUT_PULLUP);

  // home the motors
  // zeroMotorHorz(horz_BACKLASH);
  // zeroMotorVert(horz_BACKLASH);
  
  // Set the maximum steps per second:
  horz.setMaxSpeed(MAX_SPEED); 
  horz.setAcceleration(10000.0);
  vert.setMaxSpeed(MAX_SPEED); 
  vert.setAcceleration(10000.0);

  // Build the multistepper
  //combo.addStepper(horz);
  //combo.addStepper(vert);

  // signal to the client we are done with setup.
  Serial << "READY" << endl;
}

/**
 *  We talk .
 */ 
void loop() {

  // Only do something if there is a command
  if( getCommand(Cmd) ) { 
    // Process command and return the result.
    String resp = processCommand(Cmd);
    Serial << resp << endl;
  
    // ready for next command
    Serial << F("READY") << endl;
    }

  // manage steps.
  horz.run();
  vert.run();
}

/** ########################################################
 *  ###  Basic Controls                                  ###
 *  ###                                                  ###
 *  ########################################################
 **/

/**
 * detect the limit of the horzontal bar to a defined point
 * @return bool True if limit is hit False otherwise
 **/
bool limitDetectHorz() {
  int state = digitalRead(H_LIMIT_PIN);
  return state==0?true:false;
}

/**
 * detect the limit of the vertcal bar to a defined point
 * @return bool True if limit is hit False otherwise
 **/
bool limitDetectVert() {
  int state = digitalRead(V_LIMIT_PIN);
  return state==0?true:false;
}

/**
 * step the horzontal motor slowly until it hits the limit
 * @param setPoint the final Δposition to move to after 
 *                 finding limit before reseting to 0.
 **/
void zeroMotorHorz(long setPoint) {
  // save the maximum speed.
  float oldMaxSpeed = horz.maxSpeed();

  // set a slow speed with high acceleration
  horz.setMaxSpeed(600);
  horz.setAcceleration(1000.0);

  // temp let our current position be 0 (seems to be needed so we move backward)
  horz.setCurrentPosition(0L);
  horz.setSpeed(-400);
  // back into the limit switch.
  while(!limitDetectHorz()) {
    horz.runSpeed();
  }

  // run forward to what will be the new zero.
  horz.setSpeed(600);
  horz.setCurrentPosition(0L);
  horz.runToNewPosition(setPoint);

  // restore conditions
  horz.setCurrentPosition(0L);
  horz.setMaxSpeed( oldMaxSpeed );
}

/**
 * step the horzontal motor slowly until it hits the limit
 * @param setPoint the final Δposition to move to after 
 *                 finding limit before reseting to 0.
 **/
void zeroMotorVert(long setPoint) {
  // save the maximum speed.
  float oldMaxSpeed = vert.maxSpeed();

  // set a slow speed with high acceleration
  vert.setMaxSpeed(600);
  vert.setAcceleration(1000.0);

  // temp let our current position be 0 (seems to be needed so we move backward)
  vert.setCurrentPosition(0L);
  vert.setSpeed(-400);
  // back into the limit switch.
  while(!limitDetectVert()) {
    vert.runSpeed();
  }

  // run forward to what will be the new zero.
  vert.setSpeed(600);
  vert.setCurrentPosition(0L);
  vert.runToNewPosition(setPoint);

  // restore conditions
  vert.setCurrentPosition(0L);
  vert.setMaxSpeed( oldMaxSpeed );
}

/**
 * move the motor to a position relative to the current position 
 **/
String moveHorz(long steps) {
  if ( horz.currentPosition()+steps >= horz_LOWERLIMIT && horz.currentPosition()+steps <= horz_UPPERLIMIT ) {
    horz.move(steps);
  } else 
    return ERR_RESP + " " + String(steps);

  return OK_RESP + " " + horz.distanceToGo();
}

/**
 * move the motor to a position relative to the current position 
 **/
String moveVert(long steps) {
  if ( vert.currentPosition()+steps >= vert_LOWERLIMIT && vert.currentPosition()+steps <= vert_UPPERLIMIT ) {
    vert.move(steps);
  } else 
    return ERR_RESP + " " + String(steps);

  return OK_RESP + " " + vert.distanceToGo();
}

/**
 * move the motor to an absolute position as determined by the last zero 
 **/
String goToVert(long location) {
  if ( location >= vert_LOWERLIMIT && location <= vert_UPPERLIMIT ) {
    vert.moveTo(location);
  } else 
    return ERR_RESP + " " + location;

  return OK_RESP + " " + vert.distanceToGo();
}

/**
 * move the motor to an absolute position as determined by the last zero 
 **/
String goToHorz(long location) {
  if ( location >= horz_LOWERLIMIT && location <= horz_UPPERLIMIT ) {
    horz.moveTo(location);
  } else 
    return ERR_RESP + " " + location;

  return OK_RESP + " " + vert.distanceToGo();
}

/**
 * set the laser brightness (0 is off) 
 **/
void laserBright(int brightness) {
  analogWrite(LASER_PULSE_PIN, brightness);
}

/**
 * report the motor status 
 **/
String motorStatus() {
  char h = horz.isRunning() ? 'H':'-';
  char v = vert.isRunning() ? 'V':'-';
  return h + v;
}

/** 
 * Move motors together to specified delta position (blocks until done)
 **/
void moveTogether(long h, long v) {
  // long pos[2];
  // pos[0] = h;
  // pos[1] = v;
  moveHorz(h);
  moveVert(v);
  // combo.runSpeedToPosition(); // Blocks until all are in position
}

/** 
 * Move motors together to specified absolute position (blocks until done)
 **/
void goTogether(long h, long v) {
  // long pos[2];
  // pos[0] = h;
  // pos[1] = v;
  goToHorz(h);
  goToVert(v);
  // combo.runSpeedToPosition(); // Blocks until all are in position (doesn't quite work as advertised)
}

/**
 * enable or disable motors (keep 'em cool) 
 **/
void motorEnable(bool hMotorOn, bool vMotorOn) {
  if (hMotorOn)
    horz.enableOutputs();
  else
    horz.disableOutputs();

  if (vMotorOn)
    vert.enableOutputs();
  else
    vert.disableOutputs();

}

/** ########################################################
 *  ###  Command Handling                                ###
 *  ###                                                  ###
 *  ########################################################
 **/
// convenience function
inline 
bool is(const String& cmd, const char* toMatch) {
  return cmd.startsWith(toMatch);
}

// act on the commands
String processCommand(String cmd) {

  String retmsg = F("ERR Command failed");

    if (is(cmd, "LH")) { // LimitDetect on horizontal  ret OK
      retmsg = OK_RESP + String(limitDetectHorz()?" HIT":" OPEN");
    }

    else
    if (is(cmd, "LV")) { // LimitDetect on vertical  ret OK
      retmsg = OK_RESP + String(limitDetectVert()?" HIT":" OPEN");
    }
 
    else
    if (is(cmd, "MH")) { // Move Δh return where we are to go
       retmsg = moveHorz( cmd.substring(3).toInt() );
    }// Move Δh return where we are to go

    else
    if (is(cmd, "MV")) { // Move Δh return where we are to go
       retmsg = moveVert( cmd.substring(3).toInt() );
    }

    else
    if (is(cmd, "GH")) { // Move to abs pos h return where we are to go
       retmsg = goToHorz( cmd.substring(3).toInt() );
    }

    else
    if (is(cmd, "GV")) { // Move to abs pos v return where we are to go
       retmsg = goToVert( cmd.substring(3).toInt() );
    }

    else
    if (is(cmd, "MT")) { // Move to Δ h,v pos
      int sep = cmd.indexOf(",");
      if (sep<0) return ERR_RESP + " two values expected.";
      long h = cmd.substring(3,sep).toInt();
      long v = cmd.substring(sep+1).toInt();
      moveTogether( h, v );
      retmsg = OK_RESP + " " + h + " " + v;
    }

    else
    if (is(cmd, "GT")) { // Move to abs h,v pos
      int sep = cmd.indexOf(",");
      if (sep<0) return ERR_RESP + " two values expected.";
      long h = cmd.substring(3,sep).toInt();
      long v = cmd.substring(sep+1).toInt();
      goTogether( h, v );
      retmsg = OK_RESP + " " + h + " " + v;
    }

    else
    if (is(cmd, "ZH")) { // Move Δh return where we are to go
       zeroMotorHorz( horz_BACKLASH );
       retmsg = OK_RESP;
    }

    else
    if (is(cmd, "ZV")) { // Move Δh return where we are to go
       zeroMotorVert( vert_BACKLASH );
       retmsg = OK_RESP;
    }

    else
    if (is(cmd, "QH")) { // Move Δh return where we are to go
       retmsg = OK_RESP + " " + horz.currentPosition();
    }

    else
    if (is(cmd, "QV")) { // Move Δh return where we are to go
       retmsg = OK_RESP + " " + vert.currentPosition();
    }

    else
    if (is(cmd, "LB")) {
      laserBright( (int) cmd.substring(3).toInt() );
      retmsg = OK_RESP;
    }

    else
    if (is(cmd, "QM")) {
      retmsg = OK_RESP + motorStatus();
    }

    else
    if (is(cmd, "ME")) {
      motorEnable(
        cmd.indexOf('H')>0 ? true : false,
        cmd.indexOf('V')>0 ? true : false);
      retmsg = OK_RESP;
    }

    return retmsg;
}

// Non-blocking command retrieval
bool getCommand(String& cmd) {
  static bool wasComplete = true;

  if (wasComplete) {
    wasComplete = false;
    cmd = "";
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
