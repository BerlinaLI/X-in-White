/*!
 *  @file		ControlboxSerial.cpp
 *  Project		Controlbox using Serial Library
 *  @brief		Moonlight Controlbox Library for the Arduino
 *  Version		1.1
 *  @author		Robin Zhang 
 *  @date		2013/06/15
 *  License		GPL
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ControlboxSerial.h"

// ControlboxSerial Controlbox;

ControlboxSerial::ControlboxSerial()
{   
  // clear buffer to receive bytes
  _lenIn = 0; 
  
  // initialise callbacks to NULL pointer
  _write1bitDataCallback = NULL;
  _read1bitDataCallback  = NULL;
  _write8bitDataCallback = NULL;
  _read8bitDataCallback  = NULL;
  _writeTextCallback     = NULL;
  _readTextCallback      = NULL;
}


ControlboxSerial::~ControlboxSerial()
{

}

void ControlboxSerial::begin()
{

}

void ControlboxSerial::send1bitData(uint16_t channel, boolean value)
{
    char bufOut[16];
    sprintf(bufOut, "{=D1#%u#%u}", channel, (value ? 1 : 0));
    send(bufOut, strlen(bufOut));
}

void ControlboxSerial::send8bitData(uint16_t channel, byte value)
{
    char bufOut[16];
    sprintf(bufOut, "{=D8#%u#%u}", channel, value);
    send(bufOut, strlen(bufOut));
}

void ControlboxSerial::sendText(uint16_t channel, const char* text, uint16_t length)
{
    char bufOut[16];
    sprintf(bufOut, "{=TX#%u#", channel);
    send(bufOut, strlen(bufOut));
    send(text, length);
    send("}", 1);
}

void ControlboxSerial::sendError(uint16_t error)
{
    char bufOut[16];
    sprintf(bufOut, "{ERR#%u}", error);
    send(bufOut, strlen(bufOut));
}

void ControlboxSerial::send(const char* data, uint16_t length)
{
  uint16_t n;
  for (n=0 ; n<length ; n++) CB_SERIAL.write(data[n]); 
}

bool ControlboxSerial::read()
{  
  if (CB_SERIAL.available() > 0) {
    char c = CB_SERIAL.read();
      
    if (_lenIn >= sizeof(_bufIn)) _lenIn = 0; // buffer is full, then clear the buffer
    _bufIn[_lenIn] = c;
    _lenIn++;
      
    //Serial.println(c);
        
    if (c=='{') {
      // leading character found
      // drop all characters received before
      _bufIn[0] = c;
      _lenIn = 1;  
      
    } else if (c=='}') {
      // ending character found
      // parse received characters
      // and launch callbacks
      parse(_bufIn, _lenIn);
      _lenIn = 0;
      return true;
    }  
  }
  
  return false;
}



void ControlboxSerial::parse(char* data, uint16_t length)
{
  if (length<3 || data[0]!='{' || data[length-1]!='}') return;
  
  data[0] = 0;
  data[length-1] = 0;
  
  char* p[3]; // pointers to parameters
  int len[3]; // length of parameters
  
  int t = 0;
  p[t] = data+1;
  len[t] = 0;
  for (int n=1 ; n< length-1 ; n++) {
    if (data[n]!='#') {
      len[t]++;
    } else {
      data[n] = 0;
      if (t==2) break; // skip rest if more parameters exist
      t++;
      p[t] = data+n+1;
      len[t] = 0;
    }
  }
  
  // launch callback
  if (strcmp(p[0], "!D1")==0 && t==2) {
    if (_write1bitDataCallback != NULL) _write1bitDataCallback(atoi(p[1]), atoi(p[2]));	
  } else if (strcmp(p[0], "?D1")==0 && t==1) {
    if (_read1bitDataCallback != NULL) _read1bitDataCallback(atoi(p[1]));	
  } else if (strcmp(p[0], "!D8")==0 && t==2) {
    if (_write8bitDataCallback != NULL) _write8bitDataCallback(atoi(p[1]), atoi(p[2]));	
  } else if (strcmp(p[0], "?D8")==0 && t==1) {
    if (_read8bitDataCallback != NULL) _read8bitDataCallback(atoi(p[1]));	
  } else if (strcmp(p[0], "!TX")==0 && t==2) {
    if (_writeTextCallback != NULL) _writeTextCallback(atoi(p[1]), p[2], len[2]);	
  } else if (strcmp(p[0], "?TX")==0 && t==1) {
    if (_readTextCallback != NULL) _readTextCallback(atoi(p[1]));	
  } else {
    sendError(CB_ERROR_UNRECOGNIZED_REQUEST);  
  }
  
}

void ControlboxSerial::setHandleWrite1bitData(void (*fptr)(uint16_t channel, boolean value))
{
    _write1bitDataCallback = fptr;
}

void ControlboxSerial::setHandleRead1bitData(void (*fptr)(uint16_t channel))
{
    _read1bitDataCallback = fptr;
}

void ControlboxSerial::setHandleWrite8bitData(void (*fptr)(uint16_t channel, byte value))
{
    _write8bitDataCallback = fptr;
}

void ControlboxSerial::setHandleRead8bitData(void (*fptr)(uint16_t channel))
{
    _read8bitDataCallback = fptr;
}

void ControlboxSerial::setHandleWriteText(void (*fptr)(uint16_t channel, const char* text, uint16_t length))
{
    _writeTextCallback = fptr;
}

void ControlboxSerial::setHandleReadText(void (*fptr)(uint16_t channel))
{
    _readTextCallback = fptr;
}
