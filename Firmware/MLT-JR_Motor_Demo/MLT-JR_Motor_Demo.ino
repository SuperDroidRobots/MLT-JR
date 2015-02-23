//=========================HEADER=============================================================
/*

   MLT-JR Motor Movement Demonstration
   AUTHOR: Jason Traud
   DATE: February 23, 2015
   
   This firmware serves as a movement demonstration for the MLT-JR Tracked Platform
   
   Hardware: Arduino MegaR3      (MCU-049-000)
             MLT-JR Programmable (TP-220-002) 
             Sabertooth 2x12     (TE-091-212)
			
   License: CCAv3.0 Attribution-ShareAlike (http://creativecommons.org/licenses/by-sa/3.0/)
   You're free to use this code for any venture. Attribution is greatly appreciated. 

//============================================================================================
*/

// ****************************************************
// Libraries
// ****************************************************
#include <SoftwareSerial.h>
#include <Sabertooth.h>


// ****************************************************
// Motor Controller
// ****************************************************
SoftwareSerial SWSerial(NOT_A_PIN, 49); // RX on no pin (unused), TX on pin 11 (to S1).
Sabertooth ST(128, SWSerial); // Address 128, and use SWSerial as the serial port.


// ****************************************************
// Initial setup function, called once
// RETURNS: none
// ****************************************************
void setup()
{
  SWSerial.begin(9600);
  ST.autobaud();
}

void loop()
{
  int power;  // Temporary variable used when sending our power command to the motor controller  
  
  // NOTE: The below drive and turn commands must be called as shown below. The motor controller
  // will not act on a drive and turn command unless both are sent.
  
  // Ramp up drive forward to half speed
  for (power = 0; power <= 64; power ++) {
    ST.drive(power);
    ST.turn(0);
    delay(20);
  }

  // Ramp drive forward half speed to drive backward half speed
  for (power = 64; power >= -64; power --) {
    ST.drive(power);
    ST.turn(0);
    delay(20);
  }

  // Ramp down drive backward until stopped  
  for (power = -64; power <= 0; power ++) {
    ST.drive(power);
    ST.turn(0);
    delay(20);
  }

  // Ramp up turn right to half speed
  for (power = 0; power <= 64; power ++) {
    ST.drive(0);
    ST.turn(power);
    delay(20);
  }
  
  // Ramp turn right half speed to turn left half speed
  for (power = 64; power >= -64; power --) {
    ST.drive(0);
    ST.turn(power);
    delay(20);
  }
  
  // Ramp turn left back down to a stopped condition
  for (power = -64; power <= 0; power ++) {
    ST.drive(0);
    ST.turn(power);
    delay(20);
  }
}

