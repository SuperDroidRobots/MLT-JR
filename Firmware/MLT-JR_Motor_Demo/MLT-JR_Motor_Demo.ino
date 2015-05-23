//=========================HEADER=============================================================
/*

   MLT-JR Motor Movement Demonstration
   AUTHOR: Jason Traud
   DATE: May 23, 2015
   
   This firmware serves as a movement demonstration for the MLT-JR Tracked Platform. The 
   Arduino communicates with the Sabertooth motor controller over a software serial port.
   The commands are sent by the Sabertooth library when the .drive() and .turn() functions
   are called. Both of these commands must be sent together so that the motor controller 
   can calculate the appropriate motor mixing. If one is sent without the other the motor
   controller will not respond.   
   
   
   Hardware: Arduino Uno R3      (MCU-050-000)
             MLT-JR Programmable (TP-220-002) 
             Sabertooth 2x12     (TE-091-212)

   Connections: 
   
     Arduino D13  -   Sabertooth S1 (This is the software serial port)
     Arduino D12  -   Sabertooth S2 (This is the optional emergency stop port)
     Arduino Gnd  -   Sabertooth 0V (A common ground is needed for stable communication) 
			
   License: CCAv3.0 Attribution-ShareAlike (http://creativecommons.org/licenses/by-sa/3.0/)
   You're free to use this code for any venture. Attribution is greatly appreciated. 

//============================================================================================
*/

// ****************************************************
// Libraries
// ****************************************************
#include <SoftwareSerial.h>
#include <Sabertooth.h>    

// The Sabertooth library can be downlaoded from the link below
// http://www.dimensionengineering.com/info/arduino


// ****************************************************
// Motor Controller
// ****************************************************
SoftwareSerial SWSerial(NOT_A_PIN, 13); // RX on no pin (unused), TX on pin 13 (to S1).
Sabertooth ST(128, SWSerial); // Address 128, and use SWSerial as the serial port.

#define SAB_ESTOP 12

// ****************************************************
// Initial setup function, called once
// RETURNS: none
// ****************************************************
void setup()
{
  SWSerial.begin(9600);      // Serial communicaiton to the motor controller
  
  pinMode(SAB_STOP, OUTPUT); // Our emergency stop signal
  
  delay(200);                // Give the motor controller time to boot 
  
  ST.autobaud();             // The motor controller configures it's baud rate to 
                             // match the first character sent to it. The autobaud
                             // function sends 0xAA (0b 1010 1010). 
                            
  
  // Release the emergency stop
  digitalWrite(SAB_ESTOP, HIGH);                            
}

void loop()
{
  int power;  // Temporary variable used when sending our power command to the motor controller  
  
  // NOTE: The below drive and turn commands must be called as shown below. The motor controller
  // will not act on a drive and turn command unless both are sent.
  
  // NOTE: The drive and turn commands expect a range of -127 (full speed reverse)
  // to 127 (full speeed foward), with a value of 0 being stopped. 
  
  // Forward movement demonstration
  // ****
  // Ramp up drive forward to half speed but keep turn as 0
  for (power = 0; power <= 64; power ++) {
    ST.drive(power);
    ST.turn(0);
    delay(20);
  }

  // Reverse movement demonstration
  // ****
  // Ramp drive from half speed foward to half speed backward
  for (power = 64; power >= -64; power --) {
    ST.drive(power);
    ST.turn(0);
    delay(20);
  }

  // Complete the forward and back demonstration by ramping
  // reverse speed back down to 0 (stopped)
  for (power = -64; power <= 0; power ++) {
    ST.drive(power);
    ST.turn(0);
    delay(20);
  }
 
  // Turn Demonstration
  // ***
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

