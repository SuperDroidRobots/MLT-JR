//=========================HEADER=============================================================
/*
   MLT JR xBee Robot Demomonstration
   AUTHOR: Jason Traud
   DATE: 6/4/2015

   This firmware expands on the basic movement demo to integrate with our xBee wireless remote
   system (TE-900-004).
   
   Hardware: SDR Wireless xBee Remote (TE-900-004)
             Arduino Uno R3      (MCU-050-000)
             MLT-JR Programmable      (TP-220-002) 
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

// ****************************************************
// Motor Controller
// ****************************************************
SoftwareSerial SWSerial(NOT_A_PIN, 13); // RX on no pin (unused), TX on pin 13 (to S1).
Sabertooth ST(128, SWSerial); // Address 128, and use SWSerial as the serial port.

// ****************************************************
// Networking related RAM
// ****************************************************
bool sdrFound = false;		// Master Flag for SDR
bool sFound = false;		// Flag for SDR
bool dFound = false;		// Flag for SDR
bool rFound = false;		// Flag for SDR
byte byteFB;
byte byteLR;
byte Digital1;
byte checksum;

// Used for timeouts
unsigned long currentTime;
unsigned long timeOfLastGoodPacket = 0;

// ****************************************************
// Initial setup function, called once
// RETURNS: none
// ****************************************************
void setup() {
  
  delay(2000);           // Short delay to allow the motor controllers
                         // to power up before we attempt to send it commands.
  
  Serial.begin(9600);      // xBee
  SWSerial.begin(9600);    // Serial for the motor controller
  
  allStop();		// Make sure all motors are stopped for safety
}

// ****************************************************
// Main program loop
// RETURNS: none
// ****************************************************
void loop() {
  
  currentTime = millis();
  
  processSerial();     // Handle incoming data from xBee
  timeout();           // Stop the robot if we lost connection
  
  delay(10);
}

// ****************************************************
// Sets the speed of all motor controllers to zero
// RETURNS: none
// ****************************************************
void allStop() {
  ST.motor(1,0);  // Zero motors
  ST.motor(2,0);  
}


// ****************************************************
// Processes incoming serial data
//    We first test for wheter data is in the UART
//    buffer. If present, we check for our start flags
//    and once they are found we wait until we have
//    the full packet and process the data.
// RETURNS: none
// ****************************************************
void processSerial() {
  
  unsigned char inputBufferTemp;
  byte chksumTest;

  // Wait for serial
  // **************************************************
  if (Serial.available() > 0) {

    // Check for our start flags
    // ************************************************
    if (!sFound) {
      inputBufferTemp = Serial.read();
      if(inputBufferTemp == 0x53) { sFound = true; } 
      else { sFound = false; dFound = false; rFound = false; sdrFound = false; }
    }
    
    if (!dFound) {
      inputBufferTemp = Serial.read();
      if(inputBufferTemp == 0x44) { dFound = true; } 
      else { sFound = false; dFound = false; rFound = false; sdrFound = false; }
    }
    
    if (!rFound) {
      inputBufferTemp = Serial.read();
      if(inputBufferTemp == 0x52) { rFound = true; sdrFound = true;} 
      else { sFound = false; dFound = false; rFound = false; sdrFound = false; }
    }

    // Once our start flags our found, process data
    // ************************************************
    if (sdrFound && (Serial.available()  > 3 )) {
      
      // store bytes into the appropriate variables
      byteFB    = Serial.read();
      byteLR    = Serial.read();
      Digital1  = Serial.read();
      checksum  = Serial.read();
      
      // Clear flags
      sdrFound = false;
      sFound = false; 
      dFound = false; 
      rFound = false;
      
      // Calculate our checksum
      chksumTest = byteFB + byteLR + Digital1;
      
      // Compare our calculated checksum to the expected
      if (chksumTest != checksum) {	
	return;	
      }
      
      // We found a good packet, so set current time
      timeOfLastGoodPacket = currentTime;
      
      ST.drive(byteFB);
      ST.turn(byteLR);      
    } 
  } 
}

// ****************************************************
// Timeout Routine
//    We need to be cautious of data connection losses
//    and protect the robot from damage if the remote
//    is suddenly turned off. We'll wait until half a
//    second has passed since the last good packet and 
//    then call the allStop() function. 
// RETURNS: none
// ****************************************************
void timeout() {
  if (currentTime > (timeOfLastGoodPacket + 500)) {
    allStop();
    timeOfLastGoodPacket = currentTime;
  }
}
