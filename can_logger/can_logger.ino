#include <mcp_can_dfs.h>
#include <mcp_can.h>

// cs pin is D9
const int SPI_CS_PIN = 9;
MCP_CAN CAN(SPI_CS_PIN);

void setup() {
  // fast serial
  Serial.begin(115200);
  // B-CAN runs at 125kbps
  while(CAN.begin(CAN_125KBPS) != CAN_OK) {
    Serial.println("CAN BUS Shield init fail");
    Serial.println("  Init CAN BUS Shield again");
    delay(100);
  }
  Serial.println("CAN BUS Shield init OK!");
}

// buffers
byte len;
byte buf[8];

void loop() {
  if(CAN.checkReceive() == CAN_MSGAVAIL) {
    if(CAN.readMsgBuf(&len, buf) == CAN_OK) {
      Serial.print(millis());
      Serial.print("\t");
      Serial.print(CAN.getCanId(), HEX);
      Serial.print("\t");
      for(int i = 0; i < len; ++i) {
        if(buf[i] < 0x10) Serial.print("0"); // pad 0x00-0x0F
        Serial.print(buf[i], HEX);
        Serial.print(" ");
      }
      Serial.println();
    }
  }
}
