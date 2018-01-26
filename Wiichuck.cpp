#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#include "Wiichuck.h"
#include <Wire.h>

void Wiichuck::init(int powerPin, int groundPin) {
	// Set the output pins to VCC and GND
	int cnt;
	if(powerPin > 0) {
	  pinMode(powerPin, OUTPUT);
	  digitalWrite(powerPin, HIGH);
	}
	
	if(groundPin > 0) {
	  pinMode(groundPin, OUTPUT);
	  digitalWrite(groundPin, LOW);
	}
	
	delay(100);
	
	Wire.begin();
	/*
	Wire.beginTransmission(Address);
#if (ARDUINO >= 100)    
	Wire.write(0x40);
	Wire.write(0x00);
#else
	Wire.send(0x40);
	Wire.send(0x00);
#endif    
	Wire.endTransmission();

	*/
	Wire.beginTransmission(Address);
	Wire.write(0xF0);
	Wire.write(0x55);
	Wire.endTransmission();
	delay(1);
	Wire.beginTransmission(0x52);
	Wire.write(0xFB);
	Wire.write(0x00);
	Wire.endTransmission();

/*
	// read the extension type from the register block        
	Wire.beginTransmission(0x52);
	Wire.write(0xFA);                    // extension type register
	Wire.endTransmission();
	Wire.beginTransmission(0x52);
	Wire.requestFrom(0x52, 6);               // request data from controller
	byte ctrlr_type[6];
	for (cnt = 0; cnt < 6; cnt++) {
		if (Wire.available()) {
			ctrlr_type[cnt] = Wire.read(); // Should be 0x0000 A420 0101 for Classic Controller, 0x0000 A420 0000 for nunchuck
		}
	}
	Wire.endTransmission();
	delay(1);
*/
	         
	// write the crypto key (zeros), in 3 blocks of 6, 6 & 4.
	Wire.beginTransmission(0x52);
	Wire.write(0xF0);                    // crypto key command register
	Wire.write(0xAA);                    // writes crypto enable notice
	Wire.endTransmission();
	delay(1);
	Wire.beginTransmission(0x52);
	Wire.write(0x40);                    // crypto key data address
	for (cnt = 0; cnt < 6; cnt++) {
		Wire.write(0x00);                    // writes 1st key block (zeros)
	}
	Wire.endTransmission();
	Wire.beginTransmission(0x52);
	Wire.write(0x40);                    // writes memory address
	for (cnt = 6; cnt < 12; cnt++) {
		Wire.write(0x00);                    // writes 2nd key block (zeros)
	}
	Wire.endTransmission();
	Wire.beginTransmission(0x52);
	Wire.write(0x40);                    // writes memory address
	for (cnt = 12; cnt < 16; cnt++) {
		Wire.write(0x00);                    // writes 3rd key block (zeros)
	}
	Wire.endTransmission();
	delay(1);
	// end device init 

	// Set default calibration
	calib.joyX = calib.joyY = 128;
	calib.accelX = calib.accelY = calib.accelZ = 125; // accel and lsb together == 500.
	calib.lsbX = calib.lsbY = calib.lsbZ = 0;
}

uint8_t Wiichuck::poll() {
	Wire.requestFrom(Address, 6);// request data from nunchuck
      
	int bytes = 0;
	while(Wire.available() && bytes < 6) {
		// receive uint8_t as an integer
#if (ARDUINO >= 100)            
		data.buffer[bytes++] = decode(Wire.read());
#else
		data.buffer[bytes++] = decode(Wire.receive());
#endif        
	}
      
	// send request for next data payload
	Wire.beginTransmission(Address);
#if (ARDUINO >= 100)        
	Wire.write(0x00); 
#else
	Wire.send(0x00); 
#endif
	Wire.endTransmission();
      
	// delay(100);
	return bytes >= 5;
}

void Wiichuck::calibrate() {
	calib.joyX = data.parsed.joyX;
	calib.joyY = data.parsed.joyY;
	calib.accelX = data.parsed.accelX;
	calib.accelY = data.parsed.accelY;
	calib.accelZ = data.parsed.accelZ;
	calib.lsbX = data.parsed.lsbX;
	calib.lsbY = data.parsed.lsbY;
	calib.lsbZ = data.parsed.lsbZ;
}

