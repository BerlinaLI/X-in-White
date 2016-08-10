X in White

There are three program parts in this package, you could use that to rebuild a "X in White" installation of yours.

1, MindTest
2, MindColorful
3, RGBLightControl

NeuroSky <-- bluetooth --> MindTest <-- upd --> MindColorful <-- serial --> RGBLightControl
 device                                          processing                     arduino

MindTest is a C program, you need use gcc or other compiler to build the executable file. The source code and a Mac OS X binary is included in this package. MindTest connects NeuroSky MindWave device via a bluetooth serial port, and also transmits brainwave messages to MindColorful through UDP.

MindColorful is a program working in Processing. The program receives brainwave signals and updates computer screen and external RGB LED colors by the meditation and attention changing. 

RGBLightControl is a program for your Arduino controller. It receives commands through USB serial port to change the colors of RGB LED lights.
