//==========================================================================================================//
//                                                                                                          //
//                                           Atributos de classe                                            //
//                                                                                                          //
//==========================================================================================================//

//Entradas digitais
int sensorPortaEsquerdaAberta = 22;
int sensorPortaDireitaAberta = 23;
int sensorPortaEsquerdaFechada = 24;
int sensorPortaDireitaFechada = 25;
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
int habilitaMotoresPorta = 47;
int ledPermissaoAbertura = 48;
int abrePortaEsquerda = 49;
int abrePortaDireita = 50;
int motorSobe = 51;
int motorDesce = 52;
int motorPara = 53;

//Inicialização
static boolean inicio = false;

//==========================================================================================================//
//                                                                                                          //
//                                                 Método setup                                             //
//                                                                                                          //
//==========================================================================================================//

void setup() {
  //Variáveis de classe

  //Seta os pinos como entradas digitais e seta-os como falso.
  for (int i = 22; i <= 34; i++) {
    pinMode(i, INPUT);
    digitalWrite(i, LOW);
  }

  //Seta os pinos como saídas digitais e seta-os como falso.
  for (int i = 47; i <= 53; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }

  static int matrizSensores[] = {22, 23, 24, 25, 26, 27};
  static int matrizBotoes[] = {28, 29, 30, 31, 32, 33, 34};

  Serial.println (sizeof(matrizSensores) / (sizeof(int)));

  Serial.begin(9600);

}

//==========================================================================================================//
//                                                                                                          //
//                                                 Método loop                                              //
//                                                                                                          //
//==========================================================================================================//

void loop() {

  Serial.println(millis());
  //Ao ser solicitada a abertura das portas o método "podeAbrirPortas" é chamado para verificação.


  /*boolean podeAbrirPortas = abrePortas();
    String permissao;
    if (podeAbrirPortas == 1) {
    permissao = "Abertura permitida";
    }
    else {
    permissao = "Abertura não permitida";
    }

    //Serial.println(permissao);*/
  /*******************************************************************************************************/
  /*//Todo botão apertado sofrerá a ação de selo (latching) e será um toggle




      unsigned long agora;
      agora = millis() / 1000;
      Serial.print(matrizBotoes[i]);
      Serial.print(" ");
      Serial.println(agora);
    }
    }*/
  /*******************************************************************************************************/
}


//==========================================================================================================//
//                                                                                                          //
//                                              Método abrePortas                                           //
//                                                                                                          //
//==========================================================================================================//
boolean abrePortas() {
  //As portas só podem ser abertas se a cabine estiver parada em um dos dois andares
  //Os sensores "sensorAndar1" e "sensorAndar2" ativados são permissivos para abertura das portas

  String str1 = "Sensor 1 = " + sensorAndar1;
  String str2 = " Sensor 2 = " + sensorAndar2;
  String strFinal = str1 + str2 + millis();
  Serial.println(strFinal);

  boolean podeAbrirPortas;

  if (sensorAndar1 == HIGH || sensorAndar2 == HIGH) {
    podeAbrirPortas = true;
  }
  else {
    podeAbrirPortas = false;
  }
  return podeAbrirPortas;
}



