#include <mcp_can_dfs.h>
#include <mcp_can.h>

// cs pin is D9
const int SPI_CS_PIN = 9;
MCP_CAN CAN(SPI_CS_PIN);

void setup() {
  Serial.begin(115200);

  // B-CAN runs at 125kbps
  while(CAN_OK != CAN.begin(CAN_125KBPS)) {
    Serial.println("CAN BUS Shield init fail");
    Serial.println("  Init CAN BUS Shield again");
    delay(100);
  }
  Serial.println("CAN BUS Shield init OK!");
}

void loop() {
  unsigned char len = 0;
  unsigned char buf[8];

  // check if data coming
  if(CAN_MSGAVAIL == CAN.checkReceive()){
    CAN.readMsgBuf(&len, buf);
    unsigned long canId = CAN.getCanId();
    Serial.print(millis());
    Serial.print("\t");
    Serial.print(canId, HEX);
    Serial.print("\t");
    for(int i = 0; i < len; ++i) {
      if(buf[i] < 0x10) Serial.print("0");
      Serial.print(buf[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  }
}
