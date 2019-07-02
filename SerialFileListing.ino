
#include "SerialFileListing.h"

SerialFileListing sfList(64);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Starting");
  sfList.setSerial(&Serial);
  sfList.goFolder("/");
  sfList.poll();
}

void loop() {
  // put your main code here, to run repeatedly:
  //sfList.goFolder("/");
  //delay(1000);
  sfList.poll();
}
