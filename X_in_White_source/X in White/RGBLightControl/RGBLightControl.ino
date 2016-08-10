#include <EEPROM.h>
#include "ControlboxSerial.h"

#define PIN_R 11
#define PIN_G 10
#define PIN_B 9

ControlboxSerial controlbox;
int workingMode = 1;
byte currentR = 0, currentG = 0, currentB = 0;
byte targetR = 0xff, targetG = 0xff, targetB = 0xff;

void setup() {
  
  // init serial 
  Serial.begin(9600);
  controlbox.setHandleWrite8bitData(write8bitDataCallback);
  controlbox.setHandleRead8bitData(read8bitDataCallback);
  controlbox.setHandleWrite1bitData(write1bitDataCallback);
  controlbox.setHandleRead1bitData(read1bitDataCallback);  
  controlbox.begin();
  
  // set pin mode
  pinMode(PIN_R, OUTPUT);
  pinMode(PIN_G, OUTPUT);
  pinMode(PIN_B, OUTPUT);

  // working mode setting
  workingMode = 1; //EEPROM.read(0);
  
  // read preferred color
  targetR = 0; //EEPROM.read(1);
  targetG = 0; //EEPROM.read(2);
  targetB = 0; //EEPROM.read(3);
}

void loop() {
  controlbox.read();
  
  if (workingMode & 1) {
    // is demo mode
    int st = (millis() % 6000) / 1000;
    switch (st) {
      case 0: targetR = 255; targetG = 0;   targetB = 0;   break;
      case 1: targetR = 255; targetG = 0;   targetB = 255; break;
      case 2: targetR = 0;   targetG = 0;   targetB = 255; break;
      case 3: targetR = 0;   targetG = 255; targetB = 255; break;
      case 4: targetR = 0;   targetG = 255; targetB = 0;   break;
      case 5: targetR = 255; targetG = 255; targetB = 0;   break;
    }
  }
  
  if (currentR > targetR)
    currentR--;
  else if (currentR < targetR)
    currentR++;
  if (currentG > targetG)
    currentG--;
  else if (currentG < targetG)
    currentG++;
  if (currentB > targetB)
    currentB--;
  else if (currentB < targetB)
    currentB++;
  
  analogWrite(PIN_R, (byte)currentR);
  analogWrite(PIN_G, (byte)currentG);
  analogWrite(PIN_B, (byte)currentB);
  delay(2);
}

// controlbox communication callback
void write8bitDataCallback(uint16_t channel, byte value)
{
  if (channel>=1 && channel<=3) {

    workingMode = 0;

    switch (channel) {
      case 1: targetR = value; //EEPROM.write(1, value); 
              break;
      case 2: targetG = value; //EEPROM.write(2, value); 
              break;
      case 3: targetB = value; //EEPROM.write(3, value); 
              break;
    }
    
    //controlbox.send8bitData(channel, value);
  } else {
    controlbox.sendError(CB_ERROR_UNRECOGNIZED_PARAM);
  }
}

void read8bitDataCallback(uint16_t channel)
{
  if (channel>=1 && channel<=3) {
    
    byte value = 0;
    switch (channel) {
      case 1: value = currentR; break;
      case 2: value = currentG; break;
      case 3: value = currentB; break;
    }
    controlbox.send8bitData(channel, value);
  } else {
    controlbox.sendError(CB_ERROR_UNRECOGNIZED_PARAM);
  }
}

void write1bitDataCallback(uint16_t channel, boolean value)
{
  if (channel==1) {
    
    workingMode = workingMode & (value ? 1 : 0);
    if (value) 
      workingMode |= 0b00000001;
    else
      workingMode &= 0b11111110;
   
   EEPROM.write(0, workingMode);
    
    //controlbox.send1bitData(channel, value);
  } else {
    controlbox.sendError(CB_ERROR_UNRECOGNIZED_PARAM);
  }
}

void read1bitDataCallback(uint16_t channel)
{
  if (channel==1) {
    controlbox.send1bitData(channel, workingMode & 1);
  } else {
    controlbox.sendError(CB_ERROR_UNRECOGNIZED_PARAM);
  }
}
