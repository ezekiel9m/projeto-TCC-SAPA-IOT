
  // Projeto de Conclusão do Curso em Ciências da Computação na UNASP - 2017/2018
  // Tema: IoT - PROPOSTA DE AUTOMAÇÃO DE PIVÔS DE IRRIGAÇÃO PARA AGRICULTURA INTELIGENTE
  // Autor: Ezequiel Muxito; Orientador: Prof. Ms. André Marcos da Silva 
  //********************************** Projeto_SAPI-IoT **********************************
  //********************* SAPI - Sistema de Automação para Pivô de Irirgação ***************
  
  #include <Arduino.h>
  #include <string.h>
  #include <RTClib.h>
  #include <EEPROM.h>
  #include <Wire.h>
  #include <TimerOne.h>
  #include <LiquidCrystal_I2C.h>

  #if defined(ARDUINO_ARCH_SAMD)
  // Para Zero, saída no console Serial USB, remova a linha abaixo se estiver usando a porta de programação para programar o Zero!
     #define Serial SerialUSB
  #endif
  
  RTC_DS1307 rtc;
   char daysOfTheWeek[7][12] = {"Domingo", "Segunda", "Terça", "Quarta", "Quinta", "Sexta", "Sabádo"}; //declaração dos dias da semana 
  LiquidCrystal_I2C lcd(0x3F,2,1,0,4,5,6,7,3, POSITIVE);
  
  #define sinal_analogico_sensor_1 A0 
  #define sinal_analogico_sensor_2 A1
  #define pino_RTC_i2C A2
  #define led_vermelho_sensor_1 5
  #define led_amarelo_sensor_1 6
  #define led_verde_sensor_1 7
  #define pino_rele_bomba 8
  #define pino_rele_pivo 9
  #define led_vermelho_sensor_2 10
  #define led_amarelo_sensor_2 11
  #define led_verde_sensor_2 12
  
  int     programas[21];
  int     botao_up = 2;
  int     botao_sete = 3;
  int     botao_dow = 4;
  int     i = 0;
  int     prog = 0;
  byte    hr = 0;
  byte    db = 0;
  byte    zr = 0;
  boolean m = 0;
  boolean oper = 0;
  int     d1 = 0;
  int     menu = 0;
  int     nive = 0;
  byte    contador = 0;
  int     offbomba = 0;
  int     offpivo = 0;
  float   bateria = 0.0;
  float   umidade_sensor_1 = 0.0;
  float   umidade_sensor_2 = 0.0;
  int     valor_analogico_sensor_1 = 0;
  int     valor_analogico_sensor_2 = 0;
   
void setup() {
  Serial.begin(9600);
  lcd.begin(16,2);
  lcd.clear();
  
  Timer1.initialize(500000);
  Timer1.attachInterrupt(contaTempo);
  
   nive = EEPROM.read(50);
  for (int x=0; x<10; x++){programas[x] = EEPROM.read(x);}
  
  #ifndef ESP8266
  while (!Serial); 
  #endif

  if (! rtc.begin()) {
    Serial.println("Não foi possível encontrar o RTC");
    while (1);
  }
  if (! rtc.isrunning()) {
    Serial.println("O RTC NÃO está funcionando!");
    // A seguinte linha define o RTC para a data e hora em que este sketch foi compilado
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // Esta linha define o RTC com uma data e hora explícitas, por exemplo, para definir
    // January 21, 2014 at 3am you would call:
    //rtc.adjust(DateTime(2018, 7, 18, 19, 59, 00)); 
  }
  pinMode(pino_rele_pivo, OUTPUT);
  pinMode(pino_rele_bomba, OUTPUT); 
  
  pinMode(led_vermelho_sensor_1, OUTPUT);   
  pinMode(led_amarelo_sensor_1, OUTPUT);  
  pinMode(led_verde_sensor_1, OUTPUT); 
  pinMode(led_vermelho_sensor_2, OUTPUT);   
  pinMode(led_amarelo_sensor_2, OUTPUT);  
  pinMode(led_verde_sensor_2, OUTPUT); 
  
  pinMode(botao_up, INPUT);
  digitalWrite(botao_up, HIGH);
  pinMode(botao_dow, INPUT);
  digitalWrite(botao_dow, HIGH);
  pinMode(botao_sete, INPUT);
  digitalWrite(botao_sete, HIGH);
}
void contaTempo(){
  d1 = d1++;
  if(!menu == 0){contador = contador++;}
  if(!prog == 0){contador = contador++;}
  if(!hr == 0){contador = contador++;}
  if(!i == 0){contador = contador++;}
  if(m == 1){contador = contador++;}
  if(oper == 1 && digitalRead(pino_rele_bomba)==HIGH && ( umidade_sensor_1 || umidade_sensor_2 >= nive)){desligarSistema();}
  //if(oper == 1 && digitalRead(pino_rele_pivo)==HIGH &&   umidade_sensor_1 >= nive){desligarSistema();} 
  //if(oper == 1 && digitalRead(pino_rele_bomba)==HIGH && ( umidade_sensor_2 >= nive)){desligarSistema();}
  //if(oper == 1 && digitalRead(pino_rele_pivo)==HIGH &&   umidade_sensor_2 >= nive){desligarSistema();} 
  // ou seja a bomba vai ligar ate atingir a humidade necessaria, quando a humidade for atingida ela conta um tempo que é o delay off bomba e depois desliga.  
}  
void desligarSistema (){
  delay(1000);
  lcd.clear();
 
    //lcd.setCursor(0,0); lcd.print("desligar");
    if(digitalRead(botao_sete) == LOW ){digitalWrite(pino_rele_bomba, LOW);}
    if(digitalRead(botao_sete) == LOW ){digitalWrite(pino_rele_pivo, LOW);}
     delay(150);
  lcd.clear();
}
void menus(){  
   delay(1000);
   lcd.clear();
   while(menu > 0)
   {
      lcd.setCursor(0,0);
      if(menu==1){lcd.print("Definir Horarios"); if(digitalRead(botao_sete)==LOW){prog=1; menu=0; programa();}}
      if(menu==4){lcd.print("Desligar Sistema ");  if(digitalRead(botao_sete)==LOW){db=1; menu=0; desligarSistema();}}
      if(menu==2){lcd.print("Definir Umidade "); if(digitalRead(botao_sete)==LOW){hr = 1; menu=0; Defhumidade();}}
      if(menu==5){lcd.print("Zerar programas "); if(digitalRead(botao_sete)==LOW){zr=1; menu=0; zerarPrograma();}} 
      if(menu==6){lcd.print("Modo de Operacao"); if(digitalRead(botao_sete)==LOW){m=1; menu=0; modoOperacao();}}   
      if(digitalRead(botao_up)==LOW && menu < 6){menu++;contador = 0;}
      if(digitalRead(botao_sete)==LOW && menu > 0){menu--;contador = 0;}
      if(contador == 20){ menu = 0 ; contador = 0;}
      delay(400);
  }
   lcd.clear();
}
void programa(){
   delay(1000);
   lcd.clear(); 
        while( prog> 0 && prog<7)
        {
          lcd.clear();
          lcd.setCursor(0,0);
          if(prog==1 || prog==2){lcd.print("   Programa 1   "); lcd.setCursor(2, 1);lcd.print(programas[1],  DEC);lcd.print(':');lcd.print(programas[2]);lcd.print("hora");}
          //if(prog==3 || prog==4){lcd.print("   Programa 2  "); lcd.setCursor(2, 1);lcd.print(programas[3], DEC);lcd.print(':');lcd.print(programas[4]);lcd.print(" horas");}
         // if(prog==5 || prog==6){lcd.print("   Programa 3  "); lcd.setCursor(2, 1);lcd.print(programas[5], DEC);lcd.print(':');lcd.print(programas[6]);lcd.print(" horas");}
              
          if(digitalRead(botao_up) == LOW ){programas[prog]++;contador = 0;}
          if(digitalRead(botao_dow) == LOW && programas[prog] > 0 ){programas[prog]--;contador = 0;}
          if(digitalRead(botao_sete) == LOW){prog ++;contador = 0;}
          if(contador == 30){ prog = 0 ; contador = 0;}
          if(programas[1] > 23){programas[1] = 0; }
          if(programas[3] > 23){programas[3] = 0; }         
          if(programas[5] > 23){programas[5] = 0; }
          if(programas[2] > 59){programas[2] = 0; }
          if(programas[4] > 59){programas[4] = 0; }
          if(programas[6] > 59){programas[6] = 0; }
          delay(400);
        }
  for (int x=0; x<10; x++)
  {
  EEPROM.write(x, programas[x]);
  }
  lcd.clear(); prog = 0;
}
void zerarPrograma()
{
  lcd.setCursor(4, 0);
  lcd.clear();
  lcd.print("Zerando");

 for (int x=0; x<10; x++)
{
EEPROM.write(x, 0);
} 
for (int x=0; x<10; x++){programas[x] = EEPROM.read(x);}
lcd.clear();
}
void modoOperacao(){
  delay(500);
  lcd.clear();
  while(m > 0)
  {
    lcd.setCursor(0,0); lcd.print("Modo de Controle");
    lcd.setCursor(0,1); lcd.print("Dow = Up = Pr");
    if(digitalRead(botao_up) == LOW){oper = 0; m = 0 ;}
    if(digitalRead(botao_dow) == LOW){ oper = 1; m = 0 ;}
    if(contador == 30){ m = 0 ; contador = 0;}
  }
EEPROM.write(51, oper); 
for(int x = 0; x < 16; x++) {delay(200);lcd.setCursor(x,1);lcd.write(B11111111);}
lcd.clear(); 
}
void Defhumidade()
{  
  delay(1000);
  lcd.clear();
  while(hr > 0)
  {
    lcd.setCursor(0,0); lcd.print("Definir Umidade ");
    if(digitalRead(botao_up) == LOW && nive < 100 ){nive ++;contador = 0;}
    if(digitalRead(botao_dow) == LOW && nive > 0){nive --;contador = 0;}
    if(digitalRead(botao_sete) == LOW){hr = 0;contador = 0;}
    lcd.setCursor(5, 1);lcd.print(nive);lcd.print("%   ");
    if(contador == 20){ hr = 0 ; contador = 0;}
    delay(150);
  }
EEPROM.write(50, nive);
lcd.clear();
}    
void mostraHora()
{
   DateTime now = rtc.now();
    Serial.print(now.day(), DEC);Serial.print('/');Serial.print(now.month(), DEC);Serial.print('/');
    Serial.print(now.year(), DEC);Serial.print(" (");Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    //Mostrar data no lcd 
    lcd.setCursor(12,1);lcd.print(now.day());lcd.print('/');lcd.print(now.month());
    //lcd.print('/');lcd.print(now.year());
                                                                                     
    Serial.print(") ");Serial.print(now.hour(), DEC);Serial.print(':');Serial.print(now.minute(), DEC);
    Serial.print(':');Serial.print(now.second(), DEC);
    //Mostrar hora no lcsd
    lcd.setCursor(8,0);lcd.print(now.hour(),DEC);lcd.print(':');lcd.print(now.minute(),DEC);lcd.print(':');
    lcd.print(now.second(),DEC);Serial.println(); 
}
void modoUmidade_sensor_1(){
  //Controle de bateria do RTC 
  bateria = (float (analogRead(pino_RTC_i2C)) / float (1023) * 5); // convertendo valor análogico para ponto flutuante (numeros com virgula)
  if (bateria >= 2.0){
      Serial.print("Bateria em bom estado: ");
      Serial.print (bateria);
      Serial.println("");
    }
    else if (bateria < 2.0){
      Serial.print("Bateria em mal estado: ");
      Serial.print (bateria);
      Serial.println("");
      }
  //Mostra o valor da porta analogica no serial monitor
  Serial.print("Porta analogica Sensor_1: "); Serial.print(valor_analogico_sensor_1); Serial.println("");

  //************************************** CONTROLE DO MODO DE UMIDADE DO SENSOR 1 NA PORTA ANALOGICO A0 ****************************************************
  
  //Solo encharcado, umidade excessivo, acende led vermelho
  if (valor_analogico_sensor_1 > 70 && valor_analogico_sensor_1 <= 100)
  {
    Serial.println(" Status: Solo encharcado: umidade excessivo");apagaleds_sensor_1();digitalWrite(led_vermelho_sensor_1, HIGH);delay(100);digitalWrite(led_vermelho_sensor_1, LOW); 
    //digitalWrite(pino_rele_bomba, LOW);digitalWrite(pino_rele_pivo,LOW);
    lcd.setCursor(0,0);lcd.print("off b.."); 
    lcd.setCursor(0,1); lcd.print(" ");lcd.setCursor(0,1);lcd.print("Enchar");lcd.setCursor(7,1);lcd.print(valor_analogico_sensor_1,1);
    lcd.setCursor(10,1);lcd.print("%");Serial.println(""); 
  }
  //Solo umido, acende o led verde
  if (valor_analogico_sensor_1 > 50 && valor_analogico_sensor_1 <= 70)
  {
    Serial.println(" Status: Solo umido"); apagaleds_sensor_1(); digitalWrite(led_verde_sensor_1, HIGH);
    lcd.setCursor(0,0); lcd.print("off b.."); lcd.setCursor(0,1); lcd.print(" ");lcd.setCursor(0,1);
    lcd.print("Umido:");lcd.setCursor(7,1);lcd.print(valor_analogico_sensor_1,1);lcd.setCursor(10,1);lcd.print("%");Serial.println(""); 
  }
  //Solo com umidade moderado, acende led amarelo
  if (valor_analogico_sensor_1 > 30 && valor_analogico_sensor_1 <= 50)
  {
    Serial.println(" Status: Solo com umidade moderado"); apagaleds_sensor_1(); digitalWrite(led_amarelo_sensor_1, HIGH);
    //digitalWrite(pino_rele_bomba, LOW);digitalWrite(pino_rele_pivo,LOW); 
    lcd.setCursor(0,0); lcd.print("off b.."); lcd.setCursor(0,1); lcd.print(" ");lcd.setCursor(0,1);lcd.print("Moder:");
    lcd.setCursor(7,1);lcd.print(valor_analogico_sensor_1,1);lcd.setCursor(10,1);lcd.print("%");Serial.println(""); 
  }
  //Solo seco, acende led vermelho
  if (valor_analogico_sensor_1 >= 0 && valor_analogico_sensor_1 <= 30)
  {
    Serial.println(" Status: Solo seco: irrigação ativa");apagaleds_sensor_1();digitalWrite(led_vermelho_sensor_1, HIGH); 
    digitalWrite(pino_rele_bomba, LOW);digitalWrite(pino_rele_pivo,LOW);
    lcd.setCursor(0,0);lcd.print("b ativo"); 
    lcd.setCursor(0,1); lcd.print(" ");lcd.setCursor(0,1);lcd.print("Seco.:");lcd.setCursor(7,1);lcd.print(valor_analogico_sensor_1,1);
    lcd.setCursor(10,1);lcd.print("%");Serial.println(""); 
  }
  else
  {                
    digitalWrite(pino_rele_bomba, HIGH); digitalWrite(pino_rele_pivo, HIGH);
   }
  delay(1000); 
}

 void apagaleds_sensor_1(){
    digitalWrite(led_verde_sensor_1, LOW); digitalWrite(led_amarelo_sensor_1, LOW); digitalWrite(led_vermelho_sensor_1, LOW); 
}

void modoUmidade_sensor_2(){

  Serial.print("Porta analogica: "); Serial.print(valor_analogico_sensor_2); Serial.println("");

  
  // ************************************** CONTROLE DO MODO DE UMIDADE DO SENSOR 2 NA PORTA ANALOGICO A1 ****************************************************
  
  //Solo encharcado, umidade excessivo, acende led vermelho
  if (valor_analogico_sensor_2 > 70 && valor_analogico_sensor_2 <= 100)
  {
    Serial.println(" Status: Solo encharcado: umidade excessivo");apagaleds_sensor_2();digitalWrite(led_vermelho_sensor_2, HIGH);delay(100);digitalWrite(led_vermelho_sensor_2, LOW); 
    //digitalWrite(pino_rele_bomba, LOW);digitalWrite(pino_rele_pivo,LOW);
    lcd.setCursor(0,0);lcd.print("off b.."); 
    lcd.setCursor(0,1); lcd.print(" ");lcd.setCursor(0,1);lcd.print("Enchar");lcd.setCursor(7,1);lcd.print(valor_analogico_sensor_2,1);
    lcd.setCursor(10,1);lcd.print("%");Serial.println(""); 
  }
  //Solo umido, acende o led verde
  if (valor_analogico_sensor_2 > 50 && valor_analogico_sensor_2 <= 70)
  {
    Serial.println(" Status: Solo umido"); apagaleds_sensor_2(); digitalWrite(led_verde_sensor_1, HIGH);
    lcd.setCursor(0,0); lcd.print("off b.."); lcd.setCursor(0,1); lcd.print(" ");lcd.setCursor(0,1);
    lcd.print("Umido:");lcd.setCursor(7,1);lcd.print(valor_analogico_sensor_2,1);lcd.setCursor(10,1);lcd.print("%");Serial.println(""); 
  }
  //Solo com umidade moderado, acende led amarelo
  if (valor_analogico_sensor_2 > 30 && valor_analogico_sensor_2 <= 50)
  {
    Serial.println(" Status: Solo com umidade moderado"); apagaleds_sensor_2(); digitalWrite(led_amarelo_sensor_2, HIGH);
    //digitalWrite(pino_rele_bomba, LOW);digitalWrite(pino_rele_pivo,LOW); 
    lcd.setCursor(0,0); lcd.print("off b.."); lcd.setCursor(0,1); lcd.print(" ");lcd.setCursor(0,1);lcd.print("Moder:");
    lcd.setCursor(7,1);lcd.print(valor_analogico_sensor_2,1);lcd.setCursor(10,1);lcd.print("%");Serial.println(""); 
  }
  //Solo seco, acende led vermelho
  if (valor_analogico_sensor_2 >= 0 && valor_analogico_sensor_2 <= 30)
  {
    Serial.println(" Status: Solo seco: irrigação ativa");apagaleds_sensor_2();digitalWrite(led_vermelho_sensor_2, HIGH); 
    digitalWrite(pino_rele_bomba, LOW);digitalWrite(pino_rele_pivo,LOW);
    lcd.setCursor(0,0);lcd.print("b ativo"); 
    lcd.setCursor(0,1); lcd.print(" ");lcd.setCursor(0,1);lcd.print("Seco.:");lcd.setCursor(7,1);lcd.print(valor_analogico_sensor_2,1);
    lcd.setCursor(10,1);lcd.print("%");Serial.println(""); 
  }
  else
  {                
    digitalWrite(pino_rele_bomba, HIGH); digitalWrite(pino_rele_pivo, HIGH);
   }
  delay(1000);
 
  }
 void apagaleds_sensor_2(){ 
    digitalWrite(led_verde_sensor_2, LOW); digitalWrite(led_amarelo_sensor_2, LOW); digitalWrite(led_vermelho_sensor_2, LOW);
}
 
void loop() {
   
    valor_analogico_sensor_1 =  map(analogRead(sinal_analogico_sensor_1),1023, 0,0, 100); //conversão para mostrar valor inteiro de umidade no lcd  
    valor_analogico_sensor_2 =  map(analogRead(sinal_analogico_sensor_2),1023, 0,0, 100);
    umidade_sensor_1  =  (valor_analogico_sensor_1); 
    //umidade_sensor_2  =  (valor_analogico_sensor_2);
    
    if(digitalRead(botao_sete)==LOW){menu = 1; menus();}
    modoUmidade_sensor_1();
    //modoUmidade_sensor_2();
    mostraHora(); 
}


