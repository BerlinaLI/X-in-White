/*!
 *  @file		ControlboxSerial.h
 *  Project		Controlbox using Serial Library
 *  @brief		Moonlight Controlbox Library for the Arduino
 *  Version		1.0
 *  @author		Robin Zhang 
 *  @date		2013/06/15
 *  License		GPL
 */

#ifndef LIB_CONTROLBOXSERIAL_H_
#define LIB_CONTROLBOXSERIAL_H_

#include <inttypes.h>
#include <Arduino.h>

#define CB_ERROR_UNRECOGNIZED_REQUEST 0
#define CB_ERROR_UNRECOGNIZED_PARAM   1
#define CB_SERIAL Serial

class ControlboxSerial {
		
public:
  ControlboxSerial();
  ~ControlboxSerial();
	
  void begin();
	
public:	
	
  // send responses to clients
  void send1bitData(uint16_t channel, boolean value);
  void send8bitData(uint16_t channel, byte value);
  void sendText(uint16_t channel, const char* text, uint16_t length);
  void sendError(uint16_t error);
	
  void send(const char* data, uint16_t length);
				
  bool read();
	
  // set callbacks on receiving requests
  void setHandleWrite1bitData(void (*fptr)(uint16_t channel, boolean value));
  void setHandleRead1bitData(void (*fptr)(uint16_t channel));
  void setHandleWrite8bitData(void (*fptr)(uint16_t channel, byte value));
  void setHandleRead8bitData(void (*fptr)(uint16_t channel));
  void setHandleWriteText(void (*fptr)(uint16_t channel, const char* text, uint16_t length));
  void setHandleReadText(void (*fptr)(uint16_t channel));
	
private:  
  char _bufIn[64]; // buffer to receive bytes
  byte _lenIn;     // length of the received bytes
  
  void (*_write1bitDataCallback)(uint16_t channel, boolean value);
  void (*_read1bitDataCallback)(uint16_t channel);
  void (*_write8bitDataCallback)(uint16_t channel, byte value);
  void (*_read8bitDataCallback)(uint16_t channel);
  void (*_writeTextCallback)(uint16_t channel, const char* text, uint16_t length);
  void (*_readTextCallback)(uint16_t channel);
  
  void parse(char* data, uint16_t length);
};

// extern ControlboxSerial Controlbox;

#endif // LIB_CONTROLBOXSERIAL_H_
