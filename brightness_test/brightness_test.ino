/*
 * IP Indicator brightness tester
 */
const int IP_LED = 5;

void setup() {
  Serial.begin(9600);
  pinMode(IP_LED, OUTPUT);
}

void loop() {
  if(Serial.available()) {
    int dimmer = Serial.parseInt();
  }
  Serial.println(dimmer);
  analogWrite(IP_LED, dimmer);
}
