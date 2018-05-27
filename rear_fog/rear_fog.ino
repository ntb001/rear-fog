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

// CAN bus shield: cs pin is D9
const int SPI_CS_PIN = 9;
MCP_CAN CAN(SPI_CS_PIN);

// CAN IDs
const unsigned long HEADLIGHT_ID = 0xAF81110;
const unsigned long DIMMER_ID    = 0x12F85450;

// state tracking
bool rFogState      = false;
bool switchActive   = false;
bool headlightState = false;
int dimmer          = 11;

void setup() {
  // init IP indicator
  pinMode(IP_LED, OUTPUT);

  // init relay
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, HIGH);

  // init switch
  pinMode(SWITCH, INPUT_PULLUP);
  bSwitch.attach(SWITCH);
  bSwitch.interval(5);

  // init CAN bus
  // B-CAN runs at 125kbps
  while(CAN_OK != CAN.begin(CAN_125KBPS)) {
    delay(100);
  }
}

void loop() {
  // check CAN for message
  unsigned char len = 0;
  unsigned char buf[8];
  if(CAN_MSGAVAIL == CAN.checkReceive()) {
    if(CAN.readMsgBuf(&len, buf) == CAN_OK) {
      unsigned long canId = CAN.getCanId();
      if(canId == HEADLIGHT_ID) {
        headlightState = (buf[0] & 0x02) != 0;
      }
      else if(canId == DIMMER_ID) {
        if(buf[0] == 0xD6) { // bright/daytime
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
      rFogState = !rFogState;
    }
  }
  else { // switch is off
    switchActive = false;
  }

  // turn off with headlights
  rFogState = rFogState && headlightState; // overrides switch input

  // set output
  if(rFogState) {
    analogWrite(IP_LED, dimmer);
    digitalWrite(RELAY, LOW);
  }
  else {
    analogWrite(IP_LED, 0);
    digitalWrite(RELAY, HIGH);
  }
}

