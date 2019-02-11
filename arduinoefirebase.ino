/*
 Autor: Gilson de Freitas - Rondonópolis-MT
 Código do ebook: Arduino e Firebase
 Data: 12/02/2019
 Versão do Arduíno: 1.8.8
 
 Descrição: Como sincronizar dados na nuvem com o Google Firebase - RealtimeDatabase.
            
 Compoentes: 1 Arduino Esp8266 12E,
             1 cabo de carregador de celular,
             1 Protoboard ou mais (facilita o projeto),
             2 pushbuttons ou 2 fins de curso
               - 2 resistores 10k ohms
               - 4 jumpers macho macho (20 cm)
             6 LEDs 5mm (qualquer cor)
               - 6 resistores 330 ohms 
               - 14 jumpers macho macho (20cm)
 
*/

//Bibliotecas////////////////////////////////////////////////////
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

//Configuração do Firebase///////////////////////////////////////
#define FIREBASE_HOST "arduino-e-firebase.firebaseio.com"
#define FIREBASE_AUTH "ATGTyH1BgtnEoHWGoW2IgGFKYVyEm1SDYL2Xz9e7"

//Configuração da sua WiFi///////////////////////////////////////
#define WIFI_SSID "VIVO-E380"
#define WIFI_PASSWORD "57603FE380"

//Declaração das Variáveis de "valores"//////////////////////////
byte  WC118_ALARME = 0;   //0:desligado, 1:ligado
float WC118_HUM_J1 = 0;   //Valor da Humidade do Jardim 1
byte  WC118_LUZ_Q1 = 0;   //0:desligado, 1:ligado
byte  WC118_LUZ_Q2 = 0;   //0:desligado, 1:ligado
byte  WC118_LUZ_B1 = 0;   //0:desligado, 1:ligado
byte  WC118_LUZ_S1 = 0;   //0:desligado, 1:ligado
byte  WC118_LUZ_C1 = 0;   //0:desligado, 1:ligado
byte  WC118_LUZ_J1 = 0;   //0:desligado, 1:ligado
byte  WLUZ         = 0;   //Variavel de auxilio para as luzes
byte  WC118_POR_Q1 = 0;   //0:Fechada, 1:Aberta
byte  WC118_POR_Q2 = 0;   //0:Fechada, 1:Aberta
byte  WC118_POR_B1 = 0;   //0:Fechada, 1:Aberta
byte  WC118_POR_S1 = 0;   //0:Fechada, 1:Aberta
byte  WC118_POR_C1 = 0;   //0:Fechada, 1:Aberta
float WC118_TEM_J1 = 0;   //0:Fechada, 1:Aberta

// 1 segundo = 1000
// 1 minuto  =   60 segundos: então 60 * 1000 = 60000
// 1 hora    =   60 minutos : então 60 * 60000 = 3600000

int tempoLuzes = 20000;//20000 = 20 segundos
unsigned long ultimotempoLuzes = 0;

int tempoClima = 3600000;//3.600.000 = 1 hora
unsigned long ultimotempoClima = 0;

//Declaração dos "pinos" a serem contralados pelos valores das Variáveis
byte  PC118_LUZ_Q1 = D0;//Conectar ao pino D0
byte  PC118_LUZ_Q2 = D1; //Conectar ao pino D1
byte  PC118_LUZ_B1 = D2; //Conectar ao pino D2
byte  PC118_LUZ_S1 = D3; //Conectar ao pino D3
byte  PC118_LUZ_C1 = D4; //Conectar ao pino D4
byte  PC118_LUZ_J1 = D5; //Conectar ao pino D5
//byte  PC118_POR_Q1 =   ;//Conectar ao pino __
//byte  PC118_POR_Q2 =   ;//Conectar ao pino __
//byte  PC118_POR_B1 =   ;//Conectar ao pino __
byte  PC118_POR_S1 = D6;//Conectar ao pino D6
byte  PC118_POR_C1 = D7;//Conectar ao pino D7

void setupFirebase()
{
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Serial.println("Firebase Conectado.");
}

void setupWifi()
{
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Conectando.");

  while(WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }

  setupFirebase();
  
  Serial.println("");
  Serial.println("Conectado. meu IP é");
  Serial.println(WiFi.localIP());
}

void configurarPinos()
{
  //Configuração dos pinos de saída
  pinMode(PC118_LUZ_Q1,OUTPUT);
  pinMode(PC118_LUZ_Q2,OUTPUT);
  pinMode(PC118_LUZ_B1,OUTPUT);
  pinMode(PC118_LUZ_S1,OUTPUT);
  pinMode(PC118_LUZ_C1,OUTPUT);
  pinMode(PC118_LUZ_J1,OUTPUT);

  //Configuração dos pinos de entrada
  //pinMode(PC118_POR_Q1,INPUT_PULLUP);
  //pinMode(PC118_POR_Q2,INPUT_PULLUP);
  //pinMode(PC118_POR_B1,INPUT_PULLUP);
  pinMode(PC118_POR_S1,INPUT_PULLUP);
  pinMode(PC118_POR_C1,INPUT_PULLUP);

  Serial.println("Pinos Configurados.");
}

void setup()
{
  Serial.begin(115200);
  setupWifi();
  configurarPinos();
}

void loop()
{
  if (WiFi.status() != WL_CONNECTED){
    setupWifi();//Configura o firebase
  };

  //Controle de Temperatura //////////////////////////////////////////////////
  if (millis() < ultimotempoClima)  {
    ultimotempoClima = millis(); // ajustar ultimotempoClima quando o millis() reiniciar a contagem... 
  }

  //SE É A PRIMEIRA VEZ OU ULTRAPASSOU O LIMITE DE TEMPO, ENTÃO COMPARA A TEMPERATURA
  if ((ultimotempoClima == 0) || ( (millis() - ultimotempoClima) >= tempoClima)) {
    ultimotempoClima = millis();

    //SENSOR DE TEMPERATURA E HUMIDADE ////////////////////////////
    WC118_TEM_J1 = 37.3; //pegar a TEMPERATURA do sensor local e atribuir na variável WC118_TEM_J1
    Firebase.setFloat("C118_TEM_J1",WC118_TEM_J1); //setFloat=>envia o valor decimal para a variável C118_TEM_J1 com 37.3
    
    WC118_HUM_J1 = 62.7; //pegar a HUMIDADE do sensor local e atribuir na variável WC118_HUM_J1
    //setFloat=>envia o valor decimal para variavel C118_HUM_J1 com 62.7
    Firebase.setFloat("C118_HUM_J1",WC118_HUM_J1);
  } 



  //Controle de Luzes  /////////////////////////////////////////////////////////
  if (millis() < ultimotempoLuzes)  {
    ultimotempoLuzes = millis(); // para quando o millis() reiniciar a contagem... 
  }

  //SE É A PRIMEIRA VEZ OU ULTRAPASSOU O LIMITE DE TEMPO, ENTÃO VERIFICA AS LUZES
  if ((ultimotempoLuzes == 0) || ( (millis() - ultimotempoLuzes) >= tempoLuzes)) {
    ultimotempoLuzes = millis();    
    
    //ACIONAMENTO DAS LUZES ///////////////////////////////////////
    WLUZ = Firebase.getInt("C118_LUZ_TODAS");    //getInt <--pega  o valor inteiro da variável C118_LUZ_TODAS do Firebase
    
    if (WLUZ != 2){//0:desliga, 1:liga, 2:modo manual
      // modo automatico: (Desligar todas as lampadas)
  
      if (WLUZ == 0){
        Serial.println("Todas as Lampadas foram Desligadas");//Imprime no monitor serial
      }else{
        Serial.println("Todas as Lampadas foram Ligadas");//Imprime no monitor serial
      }
  
      WC118_LUZ_Q1 = WLUZ;
      digitalWrite(PC118_LUZ_Q1,WLUZ); // Liga ou Desliga o pino
      Firebase.setInt("C118_LUZ_Q1", WLUZ);//setInt <--envia  o valor inteiro da variável C118_LUZ_Q1 do Firebase
      
      WC118_LUZ_Q2 = WLUZ;
      digitalWrite(PC118_LUZ_Q2,WLUZ); // Liga ou Desliga o pino
      Firebase.setInt("C118_LUZ_Q2", WLUZ);//setInt <--envia  o valor inteiro da variável C118_LUZ_Q2 do Firebase
      
      WC118_LUZ_B1 = WLUZ;
      digitalWrite(PC118_LUZ_B1,WLUZ); // Liga ou Desliga o pino
      Firebase.setInt("C118_LUZ_B1", WLUZ);//setInt <--envia  o valor inteiro da variável C118_LUZ_B1 do Firebase
      
      WC118_LUZ_S1 = WLUZ;
      digitalWrite(PC118_LUZ_S1,WLUZ); // Liga ou Desliga o pino
      Firebase.setInt("C118_LUZ_S1", WLUZ);//setInt <--envia  o valor inteiro da variável C118_LUZ_S1 do Firebase
      
      WC118_LUZ_C1 = WLUZ;
      digitalWrite(PC118_LUZ_C1,WLUZ); // Liga ou Desliga o pino
      Firebase.setInt("C118_LUZ_C1", WLUZ);//setInt <--envia  o valor inteiro da variável C118_LUZ_C1 do Firebase
      
      WC118_LUZ_J1 = WLUZ;
      digitalWrite(PC118_LUZ_J1,WLUZ); // Liga ou Desliga o pino
      Firebase.setInt("C118_LUZ_J1", WLUZ);//setInt <--envia  o valor inteiro da variável C118_LUZ_J1 do Firebase
      
      //Volta o valor 2:modo manual
      Firebase.setInt("C118_LUZ_TODAS",2);//setInt===>envia o valor inteiro para a variável C118_LUZ_TODAS do Firebase
    }
    
    if (WLUZ == 2){
      // modo manual: (lampada por lampada)
      WLUZ = Firebase.getInt("C118_LUZ_Q1");//getInt <--pega  o valor inteiro da variável C118_LUZ_Q1 do Firebase  
      if (WC118_LUZ_Q1 != WLUZ) {           // Se o o valor desta luz local for diferente do valor lido do firebase
        WC118_LUZ_Q1 = WLUZ;                // então ajusta o valor local exatamente com o valor do firebase
        digitalWrite(PC118_LUZ_Q1,WLUZ);    // liga ou desliga o pino
        Serial.println("WC118_LUZ_Q1: ");Serial.print(WLUZ); //Imprime no monitor serial se está ligado ou ou desligado.
      }
      
      WLUZ = Firebase.getInt("C118_LUZ_Q2");//getInt <--pega  o valor inteiro da variável C118_LUZ_Q2 do Firebase
      if (WC118_LUZ_Q2 != WLUZ) {           // Se o o valor desta luz local for diferente do valor lido do firebase
        WC118_LUZ_Q2 = WLUZ;                // então ajusta o valor local exatamente com o valor do firebase
        digitalWrite(PC118_LUZ_Q2,WLUZ);    // liga ou desliga o pino
        Serial.println("WC118_LUZ_Q2: ");Serial.print(WLUZ); //Imprime no monitor serial se está ligado ou ou desligado.
      }
      
      WLUZ = Firebase.getInt("C118_LUZ_B1");//getInt <--pega  o valor inteiro da variável C118_LUZ_B1 do Firebase  
      if (WC118_LUZ_B1 != WLUZ) {           // Se o o valor desta luz local for diferente do valor lido do firebase
        WC118_LUZ_B1 = WLUZ;                // então ajusta o valor local exatamente com o valor do firebase
        digitalWrite(PC118_LUZ_B1,WLUZ);    // liga ou desliga o pino
        Serial.println("WC118_LUZ_B1: ");Serial.print(WLUZ); //Imprime no monitor serial se está ligado ou ou desligado.
      }
      
      WLUZ = Firebase.getInt("C118_LUZ_S1");//getInt <--pega  o valor inteiro da variável C118_LUZ_S1 do Firebase  
      if (WC118_LUZ_S1 != WLUZ) {           // Se o o valor desta luz local for diferente do valor lido do firebase
        WC118_LUZ_S1 = WLUZ;                // então ajusta o valor local exatamente com o valor do firebase
        digitalWrite(PC118_LUZ_S1,WLUZ);    // liga ou desliga o pino
        Serial.println("WC118_LUZ_S1: ");Serial.print(WLUZ); //Imprime no monitor serial se está ligado ou ou desligado.
      }
      
      WLUZ = Firebase.getInt("C118_LUZ_C1");//getInt <--pega  o valor inteiro da variável C118_LUZ_C1 do Firebase  
      if (WC118_LUZ_C1 != WLUZ) {           // Se o o valor desta luz local for diferente do valor lido do firebase
        WC118_LUZ_C1 = WLUZ;                // então ajusta o valor local exatamente com o valor do firebase
        digitalWrite(PC118_LUZ_C1,WLUZ);    // liga ou desliga o pino
        Serial.println("WC118_LUZ_C1: ");Serial.print(WLUZ); //Imprime no monitor serial se está ligado ou ou desligado.
      }
      
      WLUZ = Firebase.getInt("C118_LUZ_J1");//getInt <--pega  o valor inteiro da variável C118_LUZ_J1 do Firebase  
      if (WC118_LUZ_J1 != WLUZ) {           // Se o o valor desta luz local for diferente do valor lido do firebase
        WC118_LUZ_J1 = WLUZ;                // então ajusta o valor local exatamente com o valor do firebase
        digitalWrite(PC118_LUZ_J1,WLUZ);    // liga ou desliga o pino
        Serial.println("WC118_LUZ_J1: ");Serial.print(WLUZ); //Imprime no monitor serial se está ligado ou ou desligado.
      }
            
    }
  }



  //ACIONAMENTO DAS PORTAS //////////////////////////////////////
  //o controle de portas deverá ser ajustado em tempo real, não passando por tempo de millis
  //se aluma porta foi aberta ou fechada, envia os dados para o Firebase
  /*
  if (WC118_POR_Q1 != digitalRead(PC118_POR_Q1))
  {
    WC118_POR_Q1 = digitalRead(PC118_POR_Q1);
    Firebase.setInt("C118_POR_Q1",WC118_POR_Q1);     //setInt===>envia o valor inteiro para a variável C118_POR_Q1 do Firebase
    delay(500);// aguarda 0.5 segundo para evitar de variar o valor

    if (WC118_POR_Q1 == 0) {
      Serial.println("Porta: PC118_POR_Q1 fechada");
    }else{
      Serial.println("Porta: PC118_POR_Q1 Aberta");
    }
  }

  if (WC118_POR_Q2 != digitalRead(PC118_POR_Q2))
  {
    WC118_POR_Q2 = digitalRead(PC118_POR_Q2);
    Firebase.setInt("C118_POR_Q2",WC118_POR_Q2);     //setInt===>envia o valor inteiro para a variável C118_POR_Q2 do Firebase
    delay(500);// aguarda 0.5 segundo para evitar de variar o valor

    if (WC118_POR_Q2 == 0) {
      Serial.println("Porta: PC118_POR_Q2 fechada");
    }else{
      Serial.println("Porta: PC118_POR_Q2 Aberta");
    }
  }
   
  if (WC118_POR_B1 != digitalRead(PC118_POR_B1))
  {
    WC118_POR_B1 = digitalRead(PC118_POR_B1);
    Firebase.setInt("C118_POR_B1",WC118_POR_B1);     //setInt===>envia o valor inteiro para a variável C118_POR_B1 do Firebase
    delay(500);// aguarda 0.5 segundo para evitar de variar o valor

    if (WC118_POR_B1 == 0) {
      Serial.println("Porta: PC118_POR_B1 fechada");
    }else{
      Serial.println("Porta: PC118_POR_B1 Aberta");
    }
  }
  */
  if (WC118_POR_S1 != digitalRead(PC118_POR_S1))
  {
    WC118_POR_S1 = digitalRead(PC118_POR_S1);
    Firebase.setInt("C118_POR_S1",WC118_POR_S1);     //setInt===>envia o valor inteiro para a variável C118_POR_S1 do Firebase
    delay(500);// aguarda 0.5 segundo para evitar de variar o valor

    if (WC118_POR_S1 == 0) {
      Serial.println("Porta: PC118_POR_S1 fechada");
    }else{
      Serial.println("Porta: PC118_POR_S1 Aberta");
    }
  }
   
  if (WC118_POR_C1 != digitalRead(PC118_POR_C1))
  {
    WC118_POR_C1 = digitalRead(PC118_POR_C1);
    Firebase.setInt("C118_POR_C1",WC118_POR_C1);     //setInt===>envia o valor inteiro para a variável C118_POR_C1 do Firebase
    delay(500);// aguarda 0.5 segundo para evitar de variar o valor

    if (WC118_POR_C1 == 0) {
      Serial.println("Porta: PC118_POR_C1 fechada");
    }else{
      Serial.println("Porta: PC118_POR_C1 Aberta");
    }
  }  

  //delay(10000);
  // é bom deixar este delay de 10 segundos enquanto você esttiver realizando testes
  // isto evitará você consumir muitos dados, caso deixar alguma programação errada
}
