//Carrega bibliotecas de porta serial extra e do comando de vox
#include <SoftwareSerial.h>
#include "VoiceRecognitionV3.h"

//Instancia a classe VR passando o RX/TX como parametro
VR myVR(50, 51);   // 50:RX 51:TX (No Mega, no Uno usar 2 e 3)

//Nao sei o que isto faz
//uint8_t records[7]; // save record
uint8_t buf[64];

//Assinatura dos Comandos
#define vozAbre    (0)
#define vozFecha   (1)
#define vozDesce   (2)
#define vozSobe    (3)

void setup()
{
  myVR.begin(9600);
  Serial.begin(115200);

  if (myVR.clear() == 0) {
    Serial.println("Modulo reiniciado");
  } else {
    Serial.println("Modulo nao encontrado");
    while (1);
  }

  if (myVR.load((uint8_t)vozAbre) >= 0) {
    Serial.println("vozAbre carregado");
  }

  if (myVR.load((uint8_t)vozFecha) >= 0) {
    Serial.println("vozFecha carregado");
  }

  if (myVR.load((uint8_t)vozSobe) >= 0) {
    Serial.println("vozSobe carregado");
  }

  if (myVR.load((uint8_t)vozDesce) >= 0) {
    Serial.println("vozDesce carregado");
  }
}

void loop()
{
  int ret;
  ret = myVR.recognize(buf, 50);
  if (ret > 0) {
    switch (buf[1]) {
      case vozAbre:
        Serial.println("Comando " + String(buf[1]));
        break;
      case vozFecha:
        Serial.println("Comando " + String(buf[1]));
        break;
      case vozSobe:
        Serial.println("Comando " + String(buf[1]));
        break;
      case vozDesce:
        Serial.println("Comando " + String(buf[1]));
        break;
      default:
        Serial.println("Voz nao reconhecida");
        break;
    }
  }
}



