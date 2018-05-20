#include <mcp_can.h>
#include <SPI.h>

// cs pin is D9
const int SPI_CS_PIN = 9;
MCP_CAN CAN(SPI_CS_PIN);

void setup() {
  Serial.begin(115200);

  // B-CAN runs at 125kbps
  while( CAN_OK != CAN.begin(CAN_125KBPS) ){
    Serial.println( "CAN BUS Shield init fail" );
    Serial.println( "  Init CAN BUS Shield again" );
    delay( 100 );
  }
  Serial.println( "CAN BUS Shield init OK!" );
  Serial.println( "1. Park  2. Low  3. High  4. F-Fog  5. Error" );
}

unsigned long id = 0xAF81110;
unsigned char msg[6] = {0x80, 0, 0, 0, 0, 0};

void loop() {
  int in;
  if( Serial.available() ){
    in = Serial.parseInt();
    unsigned char mask[6] = {0, 0, 0, 0, 0, 0};
    if( in == 0 ){ // e-brake 80 00 00 00 00 00
      mask[0] = 0x80;
    }
    if( in == 1 ){ // marker  40 40 00 00 00 00
      mask[0] = 0x40;
      mask[1] = 0x40;
    }
    if( in == 2 ){ // low     02 00 00 00 00 00
      mask[0] = 0x02;
    }
    if( in == 3 ){ // high    01 00 00 00 00 00
      mask[0] = 0x01;
    }
    if( in == 4 ){ // f-fog   00 80 00 00 00 00
      mask[1] = 0x80;
    }
    if( in == 5 ){ // error   00 00 00 00 00 40
      mask[5] = 0x40;
    }

    // apply mask
    for( int i = 0 ; i < 6 ; ++i ){
      msg[i] ^= mask[i];
    }

    // send for 2 seconds
    for( int i = 0 ; i < 2000/300 ; ++i ){
      sendCode();
      delay( 300 );
    }
    Serial.println( "Done." );
  }
}

void sendCode() {
  Serial.print( millis() );
  Serial.print( "\t" );
  Serial.print( "Sending message: " );
  printCode();
  CAN.sendMsgBuf( id, 1, 6, msg );
}

void printCode() {
  Serial.print( id, HEX );
  Serial.print( ":" );
  for( int i = 0 ; i < 6 ; ++i ){
    Serial.print( " " );
    if( msg[i] < 0x10 ) Serial.print( "0" );
    Serial.print( msg[i], HEX );
  }
  Serial.println();
}

