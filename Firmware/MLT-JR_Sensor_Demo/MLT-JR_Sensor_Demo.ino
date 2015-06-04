//=========================HEADER=============================================================
/*
   MLT JR Sensor Demomonstration
   AUTHOR: Jason Traud
   DATE: 5/23/2015

   This firmware expands on the basic movement demo to include a front mounted IR sensor. 
   With the addition of an IR sensor to the front of the robot allows us to perform a bit
   of autonomy.
   
   In this example we are using the analog feedback to do some basic object advoance by
   stopping and turning when we see something. To help prevent issues with a random bad
   reading, a program counter is used so that we have to detect an object for half about
   half of a second before turning. 
   
   Hardware: Arduino Uno R3      (MCU-050-000)
             MLT-JR Programmable (TP-220-002) 
             Sabertooth 2x12     (TE-091-212)
             IR Sensor           (TS-061-000)

   Connections:    
     Arduino Gnd  -  IR sensor ground  (Black)
     Arduino A0   -  IR sensor output  (White)
     Arduino 5V   -  IR sensor Vcc     (Red)
     
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
#include <SoftwareSerial.h>        // Serial Library
#include <Sabertooth.h>            // Sabertooth Library

// ****************************************************
// Hardware Pin Definitions
// ****************************************************
#define SAB_PORT 13
#define SAB_ESTOP 12
#define SENSOR A0 

// ****************************************************
// Motor Controller
// ****************************************************
SoftwareSerial SWSerial(NOT_A_PIN, 13); // RX on no pin (unused), TX on pin 13 (to S1).
Sabertooth ST(128, SWSerial);           // Address 128, and use SWSerial as the serial port.

// ****************************************************
// Misc Constants and Globals
// ****************************************************
word Sensor_Dis = 0;      // Holds analog sensor information
word counter = 0;         // program counter used for object detection

// ****************************************************
// Initial setup function, called once
// RETURNS: none
// ****************************************************
void setup()
{	
  
  // Initialize serial
  Serial.begin(9600);   // Set Baud Rate for debug serial output
  SWSerial.begin(9600); // Set the baud rate for motor controller communication
  
  // Emergency stop signal for the Sabertooth motor controller
  pinMode(SAB_ESTOP, OUTPUT); 
  
  delay(200);
  ST.autobaud();  
  
  // Release the emergency stop
  digitalWrite(SAB_ESTOP, HIGH);
}

// ****************************************************
// Main program loop
// The 
// RETURNS: none
// ****************************************************
void loop()
{
  Sensor_Dis = analogRead(A0); // Read in current sensor data
  
  // DEBUG
  Serial.print("[");  Serial.print(counter);    Serial.print("]");
  Serial.print("[");  Serial.print(Sensor_Dis); Serial.println("]");
  
  // Test to see if we have detected an object 
  if(Sensor_Dis> 250) {
    
    // Due to the delay at the bottom of the main loop, six counts will
    // bring us to about a half second. At this point we know that
    // there's something in front of the robot and it isn't a bad reading
    if(counter>5) {
      
      ST.drive(0);  // Stop the robot and turn
      ST.turn(75); 
      
      delay(1900); // Wait for an arbitrary about of time
      counter=0;   // Reset our counter
    }
    
    counter=counter+1;  // Increment our counter
  }
  
  // If no obstacle is found, continue driving
  else {
    ST.drive(95);  
    ST.turn(0); 
    counter=0;
  }
     
  // Slow ourselves down. if we send data to the motor controllers too fast then 
  delay(75);
}

// ****************************************************
// Stops motors and restricts movement
// RETURNS: none 
// ****************************************************
void allStop()
{
  ST.drive(0);  // Zero motors
  ST.turn(0);
  digitalWrite(SAB_ESTOP, HIGH);  // Trigger eStop
}
