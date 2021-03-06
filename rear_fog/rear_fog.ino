/*
 * Rear fog light controller
 */
#include <Bounce2.h>
#include <mcp_can_dfs.h>
#include <mcp_can.h>

// instrument panel indicator LED
const int IP_LED = 5;

// relay
const int RELAY = 7;

// signal stalk switch
const int SWITCH = 4;
Bounce bSwitch = Bounce();

// CAN bus shield
const int SPI_CS_PIN = 9; // CS pin is D9
MCP_CAN CAN(SPI_CS_PIN);

// CAN IDs
const unsigned long HEADLIGHT_ID = 0xAF81110;
const unsigned long DIMMER_ID    = 0x12F85450;

// state tracking
bool rFogState      = false;
bool switchActive   = false;
bool headlightState = false;
int dimmer          = 11;

// buffers
unsigned char len;
unsigned char buf[8];

void setup() {
  // init IP indicator
  pinMode(IP_LED, OUTPUT);

  // init relay
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, HIGH);

  // init switch
  pinMode(SWITCH, INPUT_PULLUP); // switch shorts to ground
  bSwitch.attach(SWITCH);
  bSwitch.interval(5);

  // init CAN bus
  while(CAN_OK != CAN.begin(CAN_125KBPS)) { // B-CAN runs at 125kbps
    delay(100);
  }
}

void loop() {
  // check CAN for message
  if(CAN.checkReceive() == CAN_MSGAVAIL) {
    if(CAN.readMsgBuf(&len, buf) == CAN_OK) {
      if(CAN.getCanId() == HEADLIGHT_ID) {
        headlightState = (buf[0] & 0x02) != 0;
      }
      else if(CAN.getCanId() == DIMMER_ID) {
        if(buf[0] == 0xD6) { // max-daytime
          dimmer = 50;
        }
        else { // nighttime
          // ipDim values 0x41-0x55
          // map to 1-11
          dimmer = (buf[0] - 0x3F) / 2;
        }
      }
    }
  }

  // check switch input
  bSwitch.update();
  if(bSwitch.read() == LOW) { // switch is on
    if(switchActive == false) {
      switchActive = true;
      rFogState = !rFogState; // toggle state
    }
  }
  else { // switch is off
    switchActive = false;
  }

  // turn off with headlights
  rFogState = rFogState && headlightState; // overrides switch input

  // set output
  analogWrite(IP_LED, dimmer * rFogState);
  digitalWrite(RELAY, !rFogState); // relay is low trigger
}
