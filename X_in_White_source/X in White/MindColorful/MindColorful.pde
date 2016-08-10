// import UDP library
import hypermedia.net.*;
import processing.net.*; 
import processing.serial.*;

UDP udp;
Client tcp;
Serial com;

int poor = 200;       // 0~200
int med = 0, att = 0; // 0~100 mind wave sensor values
int r, g, b;          // 0~100 current light color
int r1, g1, b1;       // 0~100 target light color

boolean sketchFullScreen() {
  return true;
}

void setup() {
  size(displayWidth, displayHeight);
  //size(320, 320);
  frameRate(10); 
  
  udp = new UDP( this, 50000 );
  udp.listen( true );
  
  //tcp = new Client(this, "127.0.0.1", 58168); 
  String portName = Serial.list()[0];
  com = new Serial(this, portName, 9600);
  
  r=0; g=0; b=100;
  r1=r; g1=g; b1=b;
  refreshColor();
}

void draw() {
/*
    r = r1;
    g = g1;
    b = b1;
    refreshColor();
*/
  if (abs(r1-r)>3 || abs(g1-g)>3 || abs(b1-b)>3) {
    if (r1>r) r+=3; else if (r1<r) r-=3;
    if (g1>g) g+=3; else if (g1<g) g-=3;
    if (b1>b) b+=3; else if (b1<b) b-=3;
    refreshColor();
  }
}

void mousePressed() {
  exit();
}

void receive(byte[] data) {
  
  if (data.length==3 && data[0]==0 && data[1]==0) {
    // poor signal
    if (data[2] <=200 && data[2]>=0) poor = data[2];
    println("poor signal: " + poor);
  }

  if (data.length==3 && data[0]==0 && data[1]==1) {
    // attention
    if (data[2] <=100 && data[2]>=0) att = data[2];
    println("attention: " + att);
  }
  
  if (data.length==3 && data[0]==0 && data[1]==2) {
    // meditation
    if (data[2] <=100 && data[2]>=0) med = data[2];
    println("meditation: " + med);
  }
  
  if (poor < 30) {
    
    if (att<70 && med>70) {
      // meditation hi
      r1 = 100;
      g1 = 0;
      b1 = 0;
    } else if (att>70 && med<70) {
      // attention hi
      r1 = 0;
      g1 = 100;
      b1 = 0;
    } else if (att<40 && med<40) {
      // both low
      r1 = 0;
      g1 = 0;
      b1 = 100;
    } else if (att>70 && med>70) {
      // both hi
      r1 = 100;
      g1 = 100;
      b1 = 0;
    } else if ((att<40 && med>40) || (att>40 && med<40)) {
      // either low
      r1 = 0;
      g1 = 100;
      b1 = 100;
    } else {
      // other
      r1 = 100;
      g1 = 0;
      b1 = 100;
    }
  }
}

void refreshColor() {
    
    com.write("{!D8#1#" + int(map(r,0,100,0,25)) + "}" + 
              "{!D8#2#" + int(map(g,0,100,0,50)) + "}" +
              "{!D8#3#" + int(map(b,0,100,0,30)) + "}");
    
    background(int(map(r,0,100,0,255)),
               int(map(g,0,100,0,255)),
               int(map(b,0,100,0,255)));
}
