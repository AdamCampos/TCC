//==========================================================================================================//
//                                                                                                          //
//                                        Importação de Bibliotecas                                         //
//                                                                                                          //
//==========================================================================================================//

#include <SoftwareSerial.h>
#include "VoiceRecognitionV3.h"

//==========================================================================================================//
//                                                                                                          //
//                                           Atributos de classe                                            //
//                                                                                                          //
//==========================================================================================================//

//Parametrização do comando de voz. O módulo deve estar previamente treinado e carregado.
//Método da classe VR (VoiceRecognitionV3). Seta uma porta serial com RX sendo o pino 2 do Arduino e TX o 3.
VR myVR(2,3);
uint8_t records[7];
uint8_t buf[64];
int led = 13;
#define comandoVozAbrir    (0)
#define comandoVozFechar   (1) 

//Entradas digitais
int sensorPortaEsquerdaAberta = 22;
int sensorPortaDireitaFechada = 23;
int sensorPortaEsquerdaFechada = 24;
int sensorPortaDireitaAberta = 25;
int sensorAndar1 = 26;
int sensorAndar2 = 27;
int botaoChamadaCima = 28;
int botaoChamadaBaixo = 29;
int botaoCabineEmergencia = 30;
int botaoCabineBaixo = 31;
int botaoCabineCima = 32;
int botaoAbrePorta = 33;
int botaoFechaPorta = 34;

//Saídas digitais
int habilitaMotorPortaEsquerda = 45;
int habilitaMotorPortaDireita = 46;
int abrePortaEsquerdaA = 47; //Para abrir setar em 1, para fechar setar em 0
int abrePortaEsquerdaB = 48; //Para abrir setar em 0, para fechar setar em 1
int abrePortaDireitaA = 49;
int abrePortaDireitaB = 50;
int motorSobe = 51;
int motorDesce = 52;
int motorPara = 53;

//Entradas polarizadas (lógica no método setup)z
boolean sensorPEA_EP;
boolean sensorPEF_EP;
boolean sensorPDA_EP;
boolean sensorPDF_EP;


//==========================================================================================================//
//                                                                                                          //
//                                                 Método setup                                             //
//                                                                                                          //
//==========================================================================================================//

void setup() {

  //Seta os pinos como entradas digitais e seta-os como falso.
  for (int i = 22; i <= 34; i++) {
    pinMode(i, INPUT);
    digitalWrite(i, LOW);
  }
  pinMode(led, OUTPUT);

  //Seta os pinos como saídas digitais e seta-os como falso.
  for (int i = 45; i <= 53; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }
  Serial.begin(9600);
  int matrizSensores[] = {22, 23, 24, 25, 26, 27};
  int matrizBotoes[] = {28, 29, 30, 31, 32, 33, 34};

  Serial.begin(115200);
  Serial.println("Menu VR"); 

  //Testa se o módulo não foi resetado
  if(myVR.clear() == 0){
    Serial.println("Modulo resetado.");
  }else{
    Serial.println("Modulo nao encontrado.");
    Serial.println("Resolver problemas de conexao com Arduino.");
    while(1);
  }

  //Tenta carregar os comandos previamente gravados
  if(myVR.load((uint8_t)comandoVozAbrir) >= 0){
    Serial.println("comandoVozAbrir carregado");
  }
  
  if(myVR.load((uint8_t)comandoVozFechar) >= 0){
    Serial.println("comandoVozFechar carregado");
  }

}
//==========================================================================================================//
//                                                                                                          //
//                                              Método polariza                                             //
//                                                                                                          //
//==========================================================================================================//

void polariza() {

  //Exibe o status de cada sensor - físico
  //Os comentários marcados com /// foram utilizados no Debug apenas
  ///Serial.print("Fisico>> ");
  //O sensor de porta aberta é NF. Quando a porta está totalmente aberta seu nível lógico é 0.
  if (digitalRead(sensorPortaEsquerdaAberta)) {
    ///Serial.print("ESQ AB acionado | ");
  } else {
    ///Serial.print("ESQ AB nao acionado | ");
  }
  if (digitalRead(sensorPortaDireitaAberta)) {
    ///Serial.print("DIR AB acionado | ");
  } else {
    ///Serial.print("DIR AB nao acionado | ");
  }

  //O sensor de porta fechada é NA. Quando a porta está totalmente fechada seu nível lógico é 1.
  if (digitalRead(sensorPortaEsquerdaFechada)) {
    ///Serial.print("ESQ FC acionado | ");
  } else {
    ///Serial.print("ESQ FC nao acionado | ");
  }
  if (digitalRead(sensorPortaDireitaFechada)) {
    ///Serial.print("DIR FC acionado | ");
  } else {
    ///Serial.print("DIR FC nao acionado | ");
  }

  //Lógica de polarização de entradas
  //Sensores das portas

  //Sensor de porta totalmente aberta. Este sensor é NF. O ponto esperado é 1. Quando comuta para 0 ele foi pressionado.
  sensorPEA_EP = digitalRead(sensorPortaEsquerdaAberta) ^ 1;
  //Sensor de porta totalmente aberta. Este sensor é NF. O ponto esperado é 1. Quando comuta para 0 ele foi pressionado.
  sensorPDA_EP = digitalRead(sensorPortaDireitaAberta) ^ 1;
  //Sensor de porta totalmente fechada. Este sensor é NA. O ponto esperado é 0. Quando comuta para 1 ele foi pressionado.
  sensorPEF_EP = digitalRead(sensorPortaEsquerdaFechada) ^ 0;
  //Sensor de porta totalmente fechada. Este sensor é NA. O ponto esperado é 0. Quando comuta para 1 ele foi pressionado.
  sensorPDF_EP = digitalRead(sensorPortaDireitaFechada) ^ 0;


  //Exibe o status de cada sensor - polarizado
  Serial.println("");
  Serial.print("Logico>> ");
  if (sensorPEA_EP && sensorPEF_EP) {
    Serial.println("Dois sensores opostos do lado esquerdo acionados!");
  }
  if (sensorPEA_EP) {
    Serial.print("Esquerda aberta | ");
  }
  if (sensorPEF_EP) {
    Serial.print("Esquerda fechada | ");
  }

  if (sensorPDA_EP && sensorPDF_EP) {
    Serial.println("Dois sensores opostos do lado direito acionados!");
  }
  if (sensorPDA_EP) {
    Serial.print("Direita aberta | ");
  }
  if (sensorPDF_EP) {
    Serial.print("Direita fechada | ");
  }
  Serial.println("");
  //delay(1000);

}

//==========================================================================================================//
//                                                                                                          //
//                                              Método abrePortas                                           //
//                                                                                                          //
//==========================================================================================================//
void abrePortas(String lado) {

  //Escreve no monitor qual porta foi pedida para ser aberta
  String str = "Pedido de abertura da porta " + lado;
  Serial.println(str);

  //Lê os sensores fim-de-curso
  //Primeiro testa se a porta solicitada foi a da esquerda
  if (lado.equals("Esquerda")) {
    //Agora verifica se a porta já se encontra aberta
    if (sensorPEA_EP) {
      Serial.println("Porta esquerda totalmente aberta");
      //Certifica-se de parar o motor de abertura da porta esquerda
      digitalWrite(abrePortaEsquerdaA, LOW); //Retira pulso para abertura
      digitalWrite(abrePortaEsquerdaB, LOW); //Retira pulso para abertura
      digitalWrite(habilitaMotorPortaEsquerda, LOW); //Inibe ponte-h da porta da esquerda
    }
    else {
      //Se a porta da esquerda não se encontrar totalmente aberta o controlador solicita sua abertura
      Serial.println("-----Abrindo porta esquerda");
      //Certifica-se de mandar abrir a porta esquerda
      digitalWrite(abrePortaEsquerdaA, HIGH); //Ativa pulso para abertura
      digitalWrite(abrePortaEsquerdaB, LOW); //Retira pulso para abertura
      digitalWrite(habilitaMotorPortaEsquerda, HIGH); //Habilita ponte-h da porta da esquerda
    }

    //Depois testa se a porta solicitada foi a da direita
  } else   if (lado.equals("Direita")) {
    //Agora verifica se a porta já se encontra aberta
    if (sensorPDA_EP) {
      Serial.println("Porta direita totalmente aberta");
      //Certifica-se de parar o motor de abertura da porta esquerda
      digitalWrite(abrePortaDireitaA, LOW); //Retira pulso para abertura
      digitalWrite(abrePortaDireitaB, LOW); //Retira pulso para abertura
      digitalWrite(habilitaMotorPortaDireita, LOW); //Inibe ponte-h da porta da direita
    }
    else {
      //Se a porta da direita não se encontrar totalmente aberta o controlador solicita sua abertura
      Serial.println("-----Abrindo porta direita");
      //Certifica-se de mandar abrir a porta esquerda
      digitalWrite(abrePortaDireitaA, HIGH); //Ativa pulso para abertura
      digitalWrite(abrePortaDireitaB, LOW); //Retira pulso para abertura
      digitalWrite(habilitaMotorPortaDireita, HIGH); //Habilita ponte-h da porta da direita
    }
  }

}

//==========================================================================================================//
//                                                                                                          //
//                                             Método fechaPortas                                           //
//                                                                                                          //
//==========================================================================================================//
void fechaPortas(String lado) {

  //Escreve no monitor qual porta foi pedida para ser aberta
  String str = "Pedido de fechamento da porta " + lado;
  Serial.println(str);

  //Lê os sensores fim-de-curso
  //Primeiro testa se a porta solicitada foi a da esquerda
  if (lado.equals("Esquerda")) {
    //Agora verifica se a porta já se encontra fechada
    if (sensorPEF_EP) {
      Serial.println("Porta esquerda totalmente fechada");
      //Certifica-se de parar o motor de fechamento da porta esquerda
      digitalWrite(abrePortaEsquerdaA, LOW); //Retira pulso para fechamento
      digitalWrite(abrePortaEsquerdaB, LOW); //Retira pulso para fechamento
      digitalWrite(habilitaMotorPortaEsquerda, LOW); //Inibe ponte-h da porta da esquerda
    }
    else {
      //Se a porta da esquerda não se encontrar totalmente fechada o controlador solicita seu fechamento
      Serial.println("-----Fechando porta esquerda");
      //Certifica-se de mandar fechar a porta esquerda
      digitalWrite(abrePortaEsquerdaA, LOW); //Ativa pulso para fechamento
      digitalWrite(abrePortaEsquerdaB, HIGH); //Retira pulso para fechamento
      digitalWrite(habilitaMotorPortaEsquerda, HIGH); //Habilita ponte-h da porta da esquerda
    }

    //Depois testa se a porta solicitada foi a da direita
  } else   if (lado.equals("Direita")) {
    //Agora verifica se a porta já se encontra fechada
    if (sensorPDF_EP) {
      Serial.println("Porta direita totalmente fechada");
      //Certifica-se de parar o motor de abertura da porta esquerda
      digitalWrite(abrePortaDireitaA, LOW); //Retira pulso para abertura
      digitalWrite(abrePortaDireitaB, LOW); //Retira pulso para abertura
      digitalWrite(habilitaMotorPortaDireita, LOW); //Inibe ponte-h da porta da direita
    }
    else {
      //Se a porta da direita não se encontrar totalmente aberta o controlador solicita sua abertura
      Serial.println("-----Fechando porta direita");
      //Certifica-se de mandar abrir a porta esquerda
      digitalWrite(abrePortaDireitaA, LOW); //Ativa pulso para abertura
      digitalWrite(abrePortaDireitaB, HIGH); //Retira pulso para abertura
      digitalWrite(habilitaMotorPortaDireita, HIGH); //Habilita ponte-h da porta da direita
    }
  }

}

//==========================================================================================================//
//                                                                                                          //
//                                                Método botoes                                             //
//                                                                                                          //
//==========================================================================================================//

void botoes() {


  //Botoes internos da cabine
  /*Abertura e fechamento das portas
    Se o botão de abertura ou fechamento da cabine for acionado ele solicita
    a abertura ou fechamento de ambas as portas até que os sensores sejam todos acionados.*/
  String lado;
  if (digitalRead(botaoAbrePorta) && digitalRead(botaoFechaPorta)) {
    //Ambos botoes pressionados simultaneamente. O controlador não atualiza os comandos
    Serial.println("Ambos botoes acionados - cabine");
    digitalWrite(abrePortaEsquerdaA, LOW);
    digitalWrite(abrePortaEsquerdaB, LOW);
    digitalWrite(abrePortaDireitaA, LOW);
    digitalWrite(abrePortaDireitaB, LOW);
    digitalWrite(habilitaMotorPortaEsquerda, LOW); //Inibe ponte-h da porta da esquerda
    digitalWrite(habilitaMotorPortaDireita, LOW); //Inibe ponte-h da porta da direita

  }
  //Caso os sensores já estejam detectando ambas as portas abertas o loop não é realizado
  else if (digitalRead(botaoAbrePorta) && (!sensorPEA_EP || !sensorPDA_EP)) {
    Serial.println("Botao de abertura acionado - cabine");
    do {
      lado = "Esquerda";
      abrePortas(lado);
      lado = "Direita";
      abrePortas(lado);
      polariza();
    }
    while (!sensorPEA_EP && !sensorPDA_EP);
  }

  //Caso os sensores já estejam detectando ambas as portas fechadas o loop não é realizado
  else if (digitalRead(botaoFechaPorta) && (!sensorPEF_EP || !sensorPDF_EP)) {
    Serial.println("Botao de fechamento acionado - cabine");
    do {
      lado = "Esquerda";
      fechaPortas(lado);
      lado = "Direita";
      fechaPortas(lado);
      polariza();
    }
    while (!sensorPEF_EP && !sensorPDF_EP);
  }
  else {
    //Nenhum botão acionado. Os motores devem parar e deixar o sistema automático os posicionar
    Serial.println("Nao ha pedido de abertura ou fechamento");
    digitalWrite(abrePortaEsquerdaA, LOW);
    digitalWrite(abrePortaEsquerdaB, LOW);
    digitalWrite(abrePortaDireitaA, LOW);
    digitalWrite(abrePortaDireitaB, LOW);
    digitalWrite(habilitaMotorPortaEsquerda, LOW); //Inibe ponte-h da porta da esquerda
    digitalWrite(habilitaMotorPortaDireita, LOW); //Inibe ponte-h da porta da direita
  }
}

//==========================================================================================================//
//                                                                                                          //
//                                           Método mostraAssinatura                                        //
//                                                                                                          //
//==========================================================================================================//

//<ostra assinatura gravada no módulo de voz durante treinamento.
void mostraAssinatura(uint8_t *buf, int comprimento)
{
  int i;
  for(i=0; i<comprimento; i++){
    if(buf[i]>0x19 && buf[i]<0x7F){
      Serial.write(buf[i]);
    }
    else{
      Serial.print("[");
      Serial.print(buf[i], HEX);
      Serial.print("]");
    }
  }
}

//==========================================================================================================//
//                                                                                                          //
//                                                Método menuVR                                             //
//                                                                                                          //
//==========================================================================================================//

//Mostra na porta serial o Menu do módulo
void menuVR(uint8_t *buf)
{
  Serial.println("VR Indice\tGrupo\tNumeroGravacao\tAssinatura");

  Serial.print(buf[2], DEC);
  Serial.print("\t\t");

  if(buf[0] == 0xFF){
    Serial.print("NONE");
  }
  else if(buf[0]&0x80){
    Serial.print("UG ");
    Serial.print(buf[0]&(~0x80), DEC);
  }
  else{
    Serial.print("SG ");
    Serial.print(buf[0], DEC);
  }
  Serial.print("\t");

  Serial.print(buf[1], DEC);
  Serial.print("\t\t");
  if(buf[3]>0){
    mostraAssinatura(buf+4, buf[3]);
  }
  else{
    Serial.print("Nada");
  }
  Serial.println("\r\n");
}

//==========================================================================================================//
//                                                                                                          //
//                                                 Método loop                                              //
//                                                                                                          //
//==========================================================================================================//

void loop() {

  int ret;
  ret = myVR.recognize(buf, 50);
  if(ret>0){
    switch(buf[1]){
      case comandoVozAbrir:
        digitalWrite(led, HIGH);
        break;
      case comandoVozFechar:
        digitalWrite(led, LOW);
        break;
      default:
        Serial.println("Comando nao reconhecido");
        break;
    }
    /** voice recognized */
    menuVR(buf);
  }

  do {

    //Exibe no monitor o tempo em que o programa está rodando
    Serial.println("-------------------------------------------------------------------");
    Serial.println(String((millis() / 1000)) + " segundos ativo" );
    polariza();
    botoes();
  }
  while (Serial.available() > 0);

}
