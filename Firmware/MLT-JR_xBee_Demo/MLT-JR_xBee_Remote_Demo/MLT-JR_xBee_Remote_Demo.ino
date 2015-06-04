//=========================HEADER=============================================================
/*
   MLT JR xBee Remote Demomonstration
   AUTHOR: Jason Traud
   DATE: 6/4/2015

   This firmware expands on the basic movement demo to integrate with our xBee wireless remote
   system (TE-900-004).
   
   Hardware: SDR Wireless xBee Remote (TE-900-004)
             MLT-JR Programmable      (TP-220-002) 
             
   Connections:         
     Arduino A0   -   Joystick Y-Axis (Front and back motion)
     Arduino A1   -   Joystick X-Axis (Left and right motion)
     Arduino 5V   -   Power for Joystick potentiometers
     Arduino Gnd  -   Ground for Joystick potentiometers
             			
   License: CCAv3.0 Attribution-ShareAlike (http://creativecommons.org/licenses/by-sa/3.0/)
   You're free to use this code for any venture. Attribution is greatly appreciated. 
//============================================================================================
*/

// ****************************************************
// Libraries
// ****************************************************
// None

// ****************************************************
// Joystick input and temporary variables
// ****************************************************
int FB;     // Forward and Back
int LR;     // Left and Right
double tempFB, tempLR;

// ****************************************************
// Networking related RAM
// ****************************************************
byte byteFB;
byte byteLR;
byte Digital1;
byte checksum;

// Reverse bits
bool revA0 = 1;  // These are used to easily flip
bool revA1 = 0;  // the direction of the joystick

// Misc variables
int deadband = 5; 

// ****************************************************
// Initial setup function, called once
// RETURNS: none
// ****************************************************
void setup() {
  Serial.begin(9600);   // COM to xBee 
}

// ****************************************************
// Main program loop
// RETURNS: none
// ****************************************************
void loop() {
  captureData();	// Retrieves new data
  packetizeData();	// Assembles data to be sent
  transmitData();       // Send data off to xBee
  //debugData();        // print out formatted data for debug
	
  delay(50);	// Delay needed for consistant communication
}

// ****************************************************
// Retrieves our analog and digital input information
// RETURNS: none
// ****************************************************
void captureData() {
  FB = analogRead(A0);
  LR = analogRead(A1);

  // Reverse input analogs if flagged
  if (revA0)	{ FB = 1024 - FB; }
  if (revA1)	{ LR = 1024 - LR; }

  // The switch is towards the joystick (full speed)
  tempFB = map(FB, 0, 1024, -126, 126);	
  tempLR = map(LR, 0, 1024, -126, 126);
  
  // Compares our input data to a defined "deadband."
  // This is done because analog joysticks are rarely
  // perfect and the center point can vary slightly.
  if ((tempFB > (0 - deadband)) && (tempFB < deadband) )	
  { tempFB = 0; }
  if ((tempLR > (0 - deadband)) && (tempLR < deadband) )	
  { tempLR = 0; } 

  // The digital byte is for your use if you want to add
  // buttons to the remote
  Digital1 = 0;	
}

// ****************************************************
// Assembles our data to be sent
// RETURNS: none
// ****************************************************
void packetizeData() {

  // Shift analog data up so we have a range of
  // 0-255
  tempFB = tempFB + 127;
  tempLR = tempLR + 127;

  // Assemble the digital packet
  // Add status of switches to digital1 here
  
  // convert transmitted data to bytes
  byteFB = byte(tempFB);
  byteLR = byte(tempLR);

  // calculate checksum
  checksum = byteFB + byteLR + byte(Digital1);
}

// ****************************************************
// Sends our data to an xBee
// RETURNS: none
// ****************************************************
void transmitData() {  
  Serial.write("S");	
  Serial.write("D");
  Serial.write("R");	
  Serial.write(byteFB);
  Serial.write(byteLR);
  Serial.write(byte(Digital1));
  Serial.write(checksum);
}

// ****************************************************
// Format our data to print to our debug port (USB)
// RETURNS: none
// ****************************************************
void debugData() {
  Serial.print("[FB: ");  Serial.print(byteFB, HEX);          Serial.print("]");
  Serial.print("[LR: ");  Serial.print(byteLR, HEX);          Serial.print("]");
  Serial.print("[D1: ");  Serial.print(byte(Digital1), BIN);  Serial.print("]");
  Serial.println(" ");
}
