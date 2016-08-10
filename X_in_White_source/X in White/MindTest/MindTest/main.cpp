/*

 you need use stty in command line to set baud rate before run MindTest program
 stty -f /dev/cu.MindWaveMobile-DevA 57600
 
 */


#include <stdlib.h>
#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "ThinkGearStreamParser.h"

int udp_send(const char* msg, int len) {
    
    int skt = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (skt == -1) return(-1);
    
    struct sockaddr_in add;
    memset((char *) &add, 0, sizeof(add));
    add.sin_family = AF_INET;
    add.sin_port = htons(50000);
    if (inet_aton("127.0.0.1", &add.sin_addr)==0) return(-2);
    
    if (sendto(skt, (const void*)msg, len, 0,
               (const struct sockaddr *)&add, sizeof(add))==-1) return(-3);
    
    close(skt);
    return 0;
}

/**
 * 1) Function which acts on the value[] bytes of each ThinkGear DataRow as it is received.
 */


void handleDataValueFunc(unsigned char extendedCodeLevel,
                         unsigned char code,
                         unsigned char valueLength,
                         const unsigned char *value,
                         void *customData ) {
    
    //printf("data received %d %d %d\n", extendedCodeLevel, code, value[0]);
    
    unsigned int val = value[0] & 0xFF;

    char msg[] = {0, 0, 0};

    if( extendedCodeLevel == 0 ) {
        switch( code ) {
                /* [CODE]: POOR_SIGNAL 0~200 */
            case( 0x02 ):
                printf( "Poor Signal: %d\n", value[0] & 0xFF );
                
                msg[1] = 0; msg[2] = val;
                udp_send(msg, sizeof(msg));
                break;

                /* [CODE]: ATTENTION eSense 0~100 */
            case( 0x04 ):
                printf( "Attention Level: %d\n", value[0] & 0xFF );
                
                msg[1] = 1; msg[2] = val;
                udp_send(msg, sizeof(msg));
                break;
                
                /* [CODE]: MEDITATION eSense 0~100 */
            case( 0x05 ):
                printf( "Meditation Level: %d\n", value[0] & 0xFF );
                
                msg[1] = 2; msg[2] = val;
                udp_send(msg, sizeof(msg));
                break;
                
                /* [CODE]: Blink Strength 1~255 */
            case( 0x16 ):
                printf( "Eye Blink Strength: %d\n", value[0] & 0xFF );
                break;
                
                /* Other [CODE]s */
                /*
            default:
                printf( "EXCODE level: %d CODE: 0x%02X vLength: %d\n", extendedCodeLevel, code, valueLength );
                printf( "Data value(s):" );
                int i;
                for(i=0; i<valueLength; i++ ) printf( " %02X", value[i] & 0xFF ); printf( "\n" );
                */
        }
    }
}

/**
 * Program which reads ThinkGear Data Values from a COM port.
 */
int main( int argc, char **argv ) {
    
    /* 2) Initialize ThinkGear stream parser
     */
    ThinkGearStreamParser parser;
    THINKGEAR_initParser( &parser, PARSER_TYPE_PACKETS,
                         handleDataValueFunc, NULL );
    
    /* TODO: Initialize 'stream' here to read from a serial data
     * stream, or whatever stream source is appropriate for your
     * application. See documentation for "Serial I/O" for your
     * platform for details.
     */
    
    FILE *stream = fopen( "/dev/cu.MindWaveMobile-DevA", "r" );
    if (!stream) {
        for (int n=1 ; n<10 ; n++) {
            char filename[1024];
            sprintf(filename, "/dev/cu.MindWaveMobile-DevA-%d", n);
            stream = fopen(filename, "r");
            if (stream) break;
        }
    }
    if (!stream) {
        fprintf(stderr, "failed to open mind wave device.\n");
        return -1;
    }
    
    /* 3) Stuff each byte from the stream into the parser. Every time
     * a Data Value is received, handleDataValueFunc() is called.
     */
    unsigned char streamByte;
    while( 1 ) {
        fread( &streamByte, 1, 1, stream );
        THINKGEAR_parseByte( &parser, streamByte );
    }
}

