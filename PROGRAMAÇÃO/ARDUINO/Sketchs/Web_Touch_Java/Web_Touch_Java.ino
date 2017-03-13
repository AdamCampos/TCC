// IMPORTANT: Adafruit_TFTLCD LIBRARY MUST BE SPECIFICALLY
// CONFIGUVERMELHO FOR EITHER THE TFT SHIELD OR THE BREAKOUT BOARD.
// SEE RELEVANT COMMENTS IN Adafruit_TFTLCD.h FOR SETUP.

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <stdint.h>
#include "TouchScreen.h"
#include <SPI.h>
#include <Ethernet.h>
#include <SoftwareSerial.h>
#include "VoiceRecognitionV3.h"

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin
#define YP A1  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 7   // can be a digital pin
#define XP 6   // can be a digital pin

#define PRETO           0x0000
#define AZUL            0x001F
#define VERMELHO        0xF800
#define AZUL_ESCURO     0xF
#define BRANCO          0xFFFF
#define MINPRESSURE     10
#define MAXPRESSURE     1000

#define vozAbre    (0)
#define vozFecha   (1)
#define vozDesce   (2)
#define vozSobe    (3)

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 600);
VR myVR(50, 51);   // 50:RX 51:TX (No Mega, no Uno usar 2 e 3)
uint8_t buf[64];
static boolean descida = true;
static boolean subida = true;
static int a = 0;
static int corSeta = 48;
//static char serial;
static float posicao;
static long tempoInicial;
String paraPagina = "null";
uint16_t identifier = 0x9328;
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(10, 170, 81, 177);
EthernetServer server(80);

/***************************************************************************************************************************************|
  |                                                                                                                                       |
  |                                                                                                                                       |
  |                                                               setup                                                                   |
  |                                                                                                                                       |
  |                                                                                                                                       |
****************************************************************************************************************************************/

void setup() {

  /*-------------------------------------------------------------------------------------------------------------------------------------*/
  //Inicia confguracao do nodulo de comando de voz
  myVR.begin(9600);
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

  /*-------------------------------------------------------------------------------------------------------------------------------------*/
  //Inicia comunicacao com porta serial padra e espera a conexao acontecer
  Serial.begin(9600);
  while (!Serial) {}

  /*-------------------------------------------------------------------------------------------------------------------------------------*/
  //Inicia configuracao do servidor web
  Ethernet.begin(mac, ip);
  server.begin();

  /*-------------------------------------------------------------------------------------------------------------------------------------*/
  //Inicia configuracao do display tft
  tft.reset();
  tft.fillScreen(PRETO);
  Serial.begin(9600);
  tft.begin(identifier);
  tft.fillScreen(BRANCO);

  //Escreve o text e o apaga 3 vezes (piscada)
  int j = 0;

  //Espessura de 3px
  while (j < 3) {
    j++;
    for (int i = 0; i < 10; i++) {
      tft.drawRect(i, i, 240 - (2 * i), 320 - (2 * i), 0x31FF);
    }

    delay(50);                                                     //Intervalo de piscada do texto
    tft.setCursor(50, 140);
    tft.setTextColor(0x31FF);
    tft.setTextSize(4);
    tft.println("UNIFOA");
    delay(50);

    tft.fillRect(40, 120, 150, 60, BRANCO);                       //Apaga o texto
  }

  for (int i = 0; i < 10; i++) {
    tft.drawRect(i, i, 240 - (2 * i), 320 - (2 * i), BRANCO);
  }

  tft.fillRect(10, 10, 220, 300, PRETO);

}

/***************************************************************************************************************************************|
  |                                                                                                                                       |
  |                                                                                                                                       |
  |                                                             webServer                                                                 |
  |                                                                                                                                       |
  |                                                                                                                                       |
****************************************************************************************************************************************/

//Este metodo conectao controlador ao modulo Ethernet, fazendo o modulo responder como um webserver, ou seja, um provedor de dados
//para clienteWebes web, como navegadores. O codigo abaixo cria a pagina em html, podendo usar o cartao sd para armazenar css e js por
//exemplo. Na aplicacao o webserver prove dados vindos do codigo interno do controlador escritos nas demais areas deste codigo e
//dados provenientes de outras fontes, como um aplicativo suÂ´pervisorio escrito em java e comunicacao por porta serial.

void webServer(String serial) {

  //Verifica a chegada de clienteWebe web
  EthernetClient clienteWeb = server.available();
  if (clienteWeb) {
    //As requisicoes http terminam com uma linha em branco
    boolean linhaAtualBranca = true;

    //Mantem o loop enquanto o cliente estiver conectado
    while (clienteWeb.connected()) {

      //Verifica se o cliente esta mandando informacao
      if (clienteWeb.available()) {

        //Le um byte do cliente
        char c = clienteWeb.read();
        //Serial.write(c);                                     // Inibido por causa do uso da porta serial para outros fins

        // Se a linha que esta chegando estiver em branco
        // pode-se a requisicao http finalizou-se e pode ser
        // iniciado
        if (c == '\n' && linhaAtualBranca) {

          //Aceita a requisicao do protocolo http. Apos a linha
          //em branco chegar e possivel enviar a resposta a requisicao
          clienteWeb.println("HTTP/1.1 200 OK");
          clienteWeb.println("Content-Type: text/html");
          clienteWeb.println("Connection: close");             // Assim que a respota e dada ao cliente e conexao e fechada
          clienteWeb.println("Refresh: 5");                    // Atualiza a pagina a cada x segundos
          clienteWeb.println();
          clienteWeb.println("<!DOCTYPE HTML>");
          clienteWeb.println("<html>");
          clienteWeb.print("Java --> ");
          clienteWeb.println(String(serial));
          clienteWeb.println("<br/>");
          clienteWeb.println("<a href=/?on2 >ON</a>");
          clienteWeb.println("<a href=/?off3 >OFF</a>");
          clienteWeb.println("&nbsp;<a href=/?off357 >ALL OFF</a><br><br>");
          clienteWeb.println("</html>");
        }
        if (c == '\n') {
          //Verifica se e uma nova linha
          linhaAtualBranca = true;
        } else if (c != '\r') {
          //Verifica se o caracter esta na mesma linha
          linhaAtualBranca = false;
        }
      }
    }
    //Da um tempo para o navegador receber a informacao
    delay(10);
    //Fecha a conexao
    clienteWeb.stop();
    Serial.println("Fechando conexao Web");
  }

}

/***************************************************************************************************************************************|
  |                                                                                                                                       |
  |                                                                                                                                       |
  |                                                             setaBaixo                                                                 |
  |                                                                                                                                       |
  |                                                                                                                                       |
****************************************************************************************************************************************/

void setaBaixo() {

  Serial.println("DESCENDO");
  String strLido = "";
  char charLido = ' ';
  boolean origemJava = false;
  boolean parar = false;
  char bufferChar[8];
  char dados[6];
  int i = 0;

  while (!parar) {


    //Mantem loop enquanto houver dados na porta serial
restart:
    while (Serial.available() > 0 && !origemJava) {

      //Le os 8 bytes seguintes
      for (int  i = 0; i < 8; i++) {

        bufferChar[i] = Serial.read();
        if (bufferChar[i - 1] == '0' && bufferChar[i] == 'j') {
          //Serial.println("PEGOU!");

          for (int k = 0; k < 6; k++) {
            dados[k] = Serial.read();
            //Serial.println("Dados: " + String(dados));
            delay(60);
          }
          origemJava = true;
          //Serial.println("PEGOU!!! ---------------------");
          break;
        }
      }
    }

    if (origemJava) {
      webServer(String(dados) + '\n');
      parar = true;
    }
    else {
      webServer(String (millis() / 1000) + '\n');
      //Serial.println("Esperando dados..." + String(millis()));
    }
    delay(50);


    corSeta ++;

    //Faz a seta azul de fundo
    for (int i = 0; i < 45; i++) {
      tft.drawFastVLine((tft.width() / 2) - i, (60 - i) , 240, corSeta);
      tft.drawFastVLine((tft.width() / 2) + i, (60 - i) , 240, corSeta);
    }

    //Faz as bordas
    for (int j = 0; j < 5; j++) {
      tft.drawFastHLine(75, 15 + j, 90, BRANCO);
    }


    for (int j = 0; j < 5; j++) {
      tft.drawFastVLine(75 - j, 15, 240, BRANCO);
      tft.drawFastVLine(165 + j, 15, 240, BRANCO);
    }

    //Faz a borda (ponta inferior da seta)
    for (int i = 0; i < 65; i++) {
      tft.drawFastVLine((tft.width() / 2) - i, (295 - i) , 15, BRANCO);
      tft.drawFastVLine((tft.width() / 2) + i, (295 - i) , 15, BRANCO);
    }

    //Apaga seta no primeiro grupo
    delay(2);
    if (a < 3) {
      for (int i = 0; i < 38; i++) {

        tft.drawFastVLine((tft.width() / 2) - i, (70 - i) + 20 * a , 30, BRANCO);
        tft.drawFastVLine((tft.width() / 2) + i, (70 - i) + 20 * a , 30, BRANCO);

        tft.drawFastVLine((tft.width() / 2) - i, (125 - i) + 20 * a , 25, BRANCO);
        tft.drawFastVLine((tft.width() / 2) + i, (125 - i) + 20 * a , 25, BRANCO);

        tft.drawFastVLine((tft.width() / 2) - i, (175 - i) + 20 * a , 20, BRANCO);
        tft.drawFastVLine((tft.width() / 2) + i, (175 - i) + 20 * a , 20, BRANCO);

        tft.drawFastVLine((tft.width() / 2) - i, (220 - i) + 20 * a , 15, BRANCO);
        tft.drawFastVLine((tft.width() / 2) + i, (220 - i) + 20 * a , 15, BRANCO);

        tft.drawFastVLine((tft.width() / 2) - i, (260 - i) + 20 * a , 10, BRANCO);
        tft.drawFastVLine((tft.width() / 2) + i, (260 - i) + 20 * a , 10, BRANCO);

        if (corSeta < 38 || corSeta > 62) {
          corSeta = 38;
        }
      }
      a++;
    }

    else {
      a = 0;
      corSeta = 38;
    }
    i++;
    origemJava = false;
  }


  Serial.println("TERMINO DE DESCIDA " + String(millis() - tempoInicial));

}

/***************************************************************************************************************************************|
  |                                                                                                                                       |
  |                                                                                                                                       |
  |                                                             setaCima                                                                  |
  |                                                                                                                                       |
  |                                                                                                                                       |
****************************************************************************************************************************************/

void setaCima() {

  Serial.println("SUBINDO");

  Serial.println("DESCENDO");
  String strLido = "";
  char charLido = ' ';
  boolean origemJava = false;
  boolean parar = false;
  char bufferChar[8];
  char dados[6];
  int i = 0;

  while (!parar) {


    //Mantem loop enquanto houver dados na porta serial
restart:
    while (Serial.available() > 0 && !origemJava) {

      //Le os 8 bytes seguintes
      for (int  i = 0; i < 8; i++) {

        bufferChar[i] = Serial.read();
        if (bufferChar[i - 1] == '0' && bufferChar[i] == 'j') {
          //Serial.println("PEGOU!");

          for (int k = 0; k < 6; k++) {
            dados[k] = Serial.read();
            //Serial.println("Dados: " + String(dados));
            delay(60);
          }
          origemJava = true;
          //Serial.println("PEGOU!!! ---------------------");
          break;
        }
      }
    }

    if (origemJava) {
      webServer(String(dados) + '\n');
      parar = true;
    }
    else {
      webServer(String (millis() / 1000) + '\n');
      //Serial.println("Esperando dados..." + String(millis()));
    }
    delay(50);

    //Faz a seta azul de fundo
    for (int i = 0; i < 45; i++) {
      tft.drawFastVLine((tft.width() / 2) - i, (15 + i) , 240, corSeta);
      tft.drawFastVLine((tft.width() / 2) + i, (15 + i) , 240, corSeta);
    }

    //Faz as bordas
    for (int j = 0; j < 5; j++) {
      tft.drawFastHLine(71, 305 - j, 99, BRANCO);
    }


    for (int j = 0; j < 5; j++) {
      tft.drawFastVLine(75 - j, 65, 240, BRANCO);
      tft.drawFastVLine(165 + j, 65, 240, BRANCO);
    }

    //Faz a borda (ponta inferior da seta)
    for (int i = 0; i < 65; i++) {
      tft.drawFastVLine((tft.width() / 2) - i, (15 + i) , 15, BRANCO);
      tft.drawFastVLine((tft.width() / 2) + i, (15 + i) , 15, BRANCO);
    }

    //Apaga seta no primeiro grupo
    delay(2);
    if (a < 3) {
      for (int i = 0; i < 38; i++) {

        //        tft.drawFastVLine((tft.width() / 2) - i, (260 + i) - 20 * a , 30, BRANCO);
        //        tft.drawFastVLine((tft.width() / 2) + i, (260 + i) - 20 * a , 30, BRANCO);

        tft.drawFastVLine((tft.width() / 2) - i, (220 + i) - 20 * a , 10, BRANCO);
        tft.drawFastVLine((tft.width() / 2) + i, (220 + i) - 20 * a , 10, BRANCO);

        tft.drawFastVLine((tft.width() / 2) - i, (175 + i) - 20 * a , 15, BRANCO);
        tft.drawFastVLine((tft.width() / 2) + i, (175 + i) - 20 * a , 15, BRANCO);

        tft.drawFastVLine((tft.width() / 2) - i, (125 + i) - 20 * a , 20, BRANCO);
        tft.drawFastVLine((tft.width() / 2) + i, (125 + i) - 20 * a , 20, BRANCO);

        tft.drawFastVLine((tft.width() / 2) - i, (70 + i) - 20 * a , 25, BRANCO);
        tft.drawFastVLine((tft.width() / 2) + i, (70 + i) - 20 * a , 25, BRANCO);

        if (corSeta < 38 || corSeta > 62) {
          corSeta = 38;
        }
      }
      a++;
    }

    else {
      a = 0;
      corSeta = 38;
    }
  }

  Serial.println("TERMINO DE SUBIDA");
}

/***************************************************************************************************************************************|
  |                                                                                                                                       |
  |                                                                                                                                       |
  |                                                           numeroDisplay                                                               |
  |                                                                                                                                       |
  |                                                                                                                                       |
****************************************************************************************************************************************/

void numeroDisplay(String numero) {

  //Desenha cÃ­rculo em volta do numeral
  for (int i = 0; i < 20; i++) {
    tft.drawCircle((tft.width() / 2), (tft.height() / 2), 65 + i, 7 + 3 * i);
    //Desenha o nÃºmero do pavimento (1)
    tft.setCursor((tft.width() / 2) - 35, (tft.height() / 2) - 55);
    tft.setTextColor(7 + 6 * i);
    tft.setTextSize(15);
    tft.println(numero);
    delay(1);
  }

  //Aguarda o usuario tocar no display
  while (true) {
    //Touch
    TSPoint  p = ts.getPoint();

    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
      Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
      break;
    }
  }

  //Limpa background e refaz a borda
  for (int i = 0; i < 10; i++) {
    tft.drawRect(i, i, 240 - (2 * i), 320 - (2 * i), 0XF);
  }

  tft.fillRect(10, 10, 220, 300, PRETO);

}



/***************************************************************************************************************************************|
  |                                                                                                                                       |
  |                                                                                                                                       |
  |                                                               loop                                                                    |
  |                                                                                                                                       |
  |                                                                                                                                       |
****************************************************************************************************************************************/

void loop() {


  //A primeira instrucao so funciona na primeira rotina.
  if (descida && subida) {
    //Limpa background e refaz a borda
    tft.fillRect(10, 10, 220, 300, PRETO);
    for (int i = 0; i < 10; i++) {
      tft.drawRect(i, i, 240 - (2 * i), 320 - (2 * i), PRETO);
    }
    numeroDisplay(" ");
    subida = true;
    descida = false;

  }

  //Desenha a seta descendo atÃ© a cabine se posicionar no primeiro piso
  else if (descida) {
    setaBaixo();
    subida = true;
    descida = false;
    //Limpa background e refaz a borda
    tft.fillRect(10, 10, 220, 300, BRANCO);
    for (int i = 0; i < 10; i++) {
      tft.drawRect(i, i, 240 - (2 * i), 320 - (2 * i), 0XF);
    }
    //Ao chegar no primeiro piso mostra o nÃºmero 1
    numeroDisplay("1");

  }


  //Ao ser pressionado o comando inverte o sentido
  //Desenha a seta subindo atÃ© a cabine se posicionar no segundo piso
  else if (subida) {
    setaCima();
    subida = false;
    descida = true;
    //Limpa background e refaz a borda
    tft.fillRect(10, 10, 220, 300, BRANCO);
    for (int i = 0; i < 10; i++) {
      tft.drawRect(i, i, 240 - (2 * i), 320 - (2 * i), 0XF);
    }
    numeroDisplay("2");

  }
  delay(20);

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


