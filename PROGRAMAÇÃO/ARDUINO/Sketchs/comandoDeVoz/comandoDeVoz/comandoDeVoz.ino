#include <SoftwareSerial.h>
#include "VoiceRecognitionV3.h"
SoftwareSerial porta(2, 3); // RX, TX

void setup() {

  pinMode(13, OUTPUT);

  delay(200);
  porta.begin(9600);
  Serial.begin(115200);

}

void loop() {
  porta.write(0xAA02000A);
  delay(200);
  porta.listen();
  String x = String(millis() / 1000);
  Serial.println(porta.read() + "\t" +  x);

}
