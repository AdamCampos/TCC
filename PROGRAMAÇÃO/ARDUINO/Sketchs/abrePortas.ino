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
int displayA = 44;
int displayB = 45;
int displayC = 46;
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

  //Declara os pinos como entradas digitais e seta-os como falso.
  for (int i = 22; i <= 34; i++) {
    pinMode(i, INPUT);
    digitalWrite(i, LOW);
  }

  //Declara os pinos como saídas digitais e seta-os como falso.
  for (int i = 44; i <= 53; i++) {
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

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  //Texto exibido por todo o loop
  String str;
  String linha;
  linha = "________________________________________________________________________________________________";

  Serial.println(linha);

//teste
  digitalWrite(53,HIGH);

  str = "Ativo ha ";
  str = str + millis() / 1000;
  if (millis() / 1000 <= 1) {
    str = str + " segundo";
  }
  else {
    str = str + " segundos";
  }



  Serial.println(str);
  delay(1000);
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////

  /*Ao ser solicitado o elevador, o sistema verifica onde a cabine está. Ela pode assumir quatro
    posições distintas: 1) Parada no piso 1; 2) Parada no piso 2; 3) Em transição do piso 1 para
    o piso 2; 4) Em transição do piso 2 para o piso 1. A variável localcabine diz ao sistema onde
    a cabine está através de um código.*/

  int localcabine = localCabine();

  if (digitalRead(botaoChamadaCima)) {
    Serial.println("Chamada no segundo piso");
  }
  if (digitalRead(botaoChamadaBaixo)) {
    Serial.println("Chamada no primeiro piso");
  }
  if (digitalRead(botaoCabineBaixo)) {
    Serial.println("Solicitaco para o primeiro piso - Cabine");
  }
  if (digitalRead(botaoCabineCima)) {
    Serial.println("Solicitacao para o segundo piso - Cabine");
  }
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
  /*As portas só podem ser abertas se a cabine estiver parada em um dos dois andares
    Os sensores "sensorAndar1" e "sensorAndar2" ativados são permissivos para abertura das portas.
    Os sensores são NA, habilitando apenas no caso de contato fechado.*/

  boolean podeAbrirPortas;

  /* Testa se algum dos sensores está habilitado, ou seja, cabine parada no andar. */
  if (sensorAndar1 == HIGH || sensorAndar2 == HIGH) {
    podeAbrirPortas = true;
  }
  else {
    podeAbrirPortas = false;
  }
  return podeAbrirPortas;
}

//==========================================================================================================//
//                                                                                                          //
//                                             Método localCabine                                           //
//                                                                                                          //
//==========================================================================================================//

int localCabine() {

  /*Uma codificação é gerada de acordo com a posição da cabine. O sistema deve averiguar constantemente o local
    anterior e o local atual para saber ons está a cabine. O código varia de 0 a 5:

    00= Não determinado
    01= Parada no piso 1
    02= Parada no piso 2
    03= Em transição do piso 1 para o piso
    04= Em transição do piso 2 para o piso 1.
    05= Erro nos sensores

    Há um sensor em cada piso. Eles são NA, assim que a cabine se desloca o sensor abre os contatos.
    O sistema faz uma tabela para identificar quando estes sensores foram atuados e quando deixaram de ser.*/

  //static int posicaoAnterior;
  //static int posicaoAtual;
  //static long tempoAtual = millis();

  /* Exemplo da tabela:

    |------------------------------------------------------------------|
    | tempo | Andar1 | Andar2 | Transição 1>2 | Transição 2>1 | Código |
    |---------------------------------------------------------|--------|
    |   0   |    X   |    -   |       -       |      -        |   01   | -> O sistema detecta através do sensor a posição 1
    |---------------------------------------------------------|--------|
    |   1   |    -   |    -   |       -       |      -        |   00   | -> O sistema não detecta a posição correta
    |---------------------------------------------------------|--------|
    |   2   |    -   |    -   |       X       |      -        |   03   | -> O sistema sabe que a cabine esteve na posição 1
    |-------|--------|--------|---------------|---------------|--------|
    |   3   |    -   |    X   |       -       |      -        |   02   | -> O sistema detecta através do sensor a posição 2
    |-------|--------|--------|---------------|---------------|--------|
    |   4   |    -   |    -   |       -       |      -        |   00   | -> O sistema não detecta a posição correta
    |-------|--------|--------|---------------|---------------|--------|
    |   5   |    -   |    -   |       -       |      X        |   04   | -> O sistema sabe que a cabine esteve na posição 2
    |-------|--------|--------|---------------|---------------|--------|
    |   6   |    X   |    X   |       -       |      -        |   05   | -> O sistema detecta erro nos sensores
    |-------|--------|--------|---------------|---------------|--------|
  */

  static int codigo = 0;
  String codigoStr;

  if (digitalRead(sensorAndar1) && digitalRead(sensorAndar2)) {
    codigo = 5;
    codigoStr = "Nao detectado. Dois sensosres atuados.";

    //Seta 101b (5d) no display
    digitalWrite(displayA, HIGH);
    digitalWrite(displayB, LOW);
    digitalWrite(displayC, HIGH);
  }
  else if (!digitalRead(sensorAndar1) && !digitalRead(sensorAndar2)) {
    codigo = 0;
    codigoStr = "Nao detectada. Nenhum sensor atuado";

    //Seta 000b (0d) no display
    digitalWrite(displayA, LOW);
    digitalWrite(displayB, LOW);
    digitalWrite(displayC, LOW);
  }
  else if (digitalRead(sensorAndar1)) {
    codigo = 1;
    codigoStr = "Primeiro andar.";

    //Seta 001b (1d) no display
    digitalWrite(displayA, HIGH);
    digitalWrite(displayB, LOW);
    digitalWrite(displayC, LOW);
  }
  else if (digitalRead(sensorAndar2)) {
    codigo = 2;
    codigoStr = "Segundo andar.";

    //Seta 010b (2d) no display
    digitalWrite(displayA, LOW);
    digitalWrite(displayB, HIGH);
    digitalWrite(displayC, LOW);
  }

  String str = "Posicao da cabine: ";
  str = str + codigoStr;
  Serial.println(str);


  return codigo;

}
