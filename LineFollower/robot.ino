// put your setup code here, to run once:
// TCCR0B = TCCR0B & B11111000 | B00000100;    // set timer 0 divisor to   256 for PWM frequency of   244.14 Hz
//  TCCR0B = TCCR0B & B11111000 | B00000001;    // set timer 0 divisor to     1 for PWM frequency of 62500.00 Hz
//TCCR0B = TCCR0B & B11111000 | B00000010;    // set timer 0 divisor to     8 for PWM frequency of  7812.50 Hz
//  TCCR0B = TCCR0B & B11111000 | B00000011;    // set timer 0 divisor to    64 for PWM frequency of   976.56 Hz (The DEFAULT)
//TCCR0B = TCCR0B & B11111000 | B00000100;    // set timer 0 divisor to   256 for PWM frequency of   244.14 Hz

#include <math.h>
#define LEWO 0xFF10EF
#define PRAWO 0xFF5AA5
#define SZYBCIEJ 0xFF18E7
#define WOLNIEJ 0xFF4AB5
#define HALT 0xFF38C7
#define HOLD 0xFFFFFFFF
#define trig 3
#define echo 2
#define PRZESZKODA 12

boolean on = true;
String inputString = "";
boolean stringComplete = false; 
int lewy = 0;
int prawy = 0;
int prog;
int Ti = 2;
int Td = 5;
int dt = 1;
int ep = 0; //poprzedni uchyb
int U = 0;
int V = 255 ;
int Kp = V/3;
int C = 0;
int S_en[] = {0, 0, 0, 0, 0, 0};
int licznik = 0;
int odleglosc;

int przelicz() {
  for  (int i = 0; i < 6; i++)
    if  (S_en[i] >= 300)
      S_en[i] = 1;
    else
      S_en[i] = 0;
}

int sum () {
  return -30 * S_en[0] - 20 * S_en[1] - 10 * S_en[2] + 10 * S_en[3] + 20 * S_en[4] + 30 * S_en[5];
}

int uchyb(int en) {
  int a;

  if (S_en[2] == 0 && S_en[3] == 0 && en == 0) {
    en = ep;
  }

  C += ((ep + en) / 2) * dt;
  U = Kp * (en + (1/ Ti)*C   + Td * (en - ep) );
  U = U / 10;
  ep = en;
}

void kierunek(int nr, int VV) {
  if (VV <= 0) {

    digitalWrite(7 + 2 * nr, HIGH);
    digitalWrite(8 + 2 * nr, LOW);
    analogWrite(5+nr, -VV);
    
  } else {
    digitalWrite(7 + 2 * nr, LOW);
    digitalWrite(8 + 2 * nr, HIGH);
    analogWrite(5+nr, VV);
  }
}

void omin_przeszkode(){
 kierunek(0,-150);
 kierunek(1,150);
 delay(77);
 kierunek(0,250);
 kierunek(1,250);
 delay(100);
 kierunek(0,150);
 kierunek(1,-150);
 delay(77);
 kierunek(1,255);
 kierunek(0, 255);
 delay(140);
 kierunek(0,150);
 kierunek(1,-150);
 delay(70);
  kierunek(0,250);
 kierunek(1,250);
 int omijaj=1;
 while(omijaj==1){
    S_en[0] = analogRead(A0);
    S_en[5] = analogRead(A1);
    S_en[2] = analogRead(A4);
    S_en[3] = analogRead(A3);
    S_en[4] = analogRead(A2);
    S_en[1] = analogRead(A5);
    przelicz();
    for (int i =0; i < 6; i++){
      if(S_en[i]!=0){
        omijaj=0;
        }
      }
  } 
}

void jedz() {
  /*if (PRZESZKODA < odleglosc) {
  int abs_U;
  if (U > 0) {
    abs_U = U;
  }
  else abs_U = -U;
  if (abs_U < V) {
      if (U < 0) {
        kierunek(0, V + U);
        kierunek(1, V );
      } else {
        kierunek(0, V);
        kierunek(1, V - U);
      }
    } else {
      if (U > 0) {
        kierunek(1, 0);
        kierunek(0, V );
      } else {
        kierunek(1, V);
        kierunek(0, 0);
      }
    }
  } else {
    omin_przeszkode();
  }*/
  if (odleglosc > PRZESZKODA) {
    if (abs(U) < V) {
      if (U < 0) {
        analogWrite(5, V + U);
        analogWrite(6, V );
      } else {
        analogWrite(5, V);
        analogWrite(6, V - U);
      }
    } else {
      if (U < 0) {
        analogWrite(5, 0);
        analogWrite(6, V );
      } else {
        analogWrite(5, V);
        analogWrite(6, 0);
      }
    }
  } else {
    analogWrite(5, LOW);
    analogWrite(6, LOW);
  }
}


int okresl_czas() {
  int czas;
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  czas = pulseIn(echo, HIGH,2000);
  if (czas==0){
    czas= 9999;
    }  
  return (czas / 2) / 29.1;
}

void setup()
{
  Serial.begin(9600);
  //  irrecv.enableIRIn(); // włączenie odbierania danych
  //TCCR0B = TCCR0B & B11111000 | B00000101;    // set timer 0 divisor to  1024 for PWM frequency of    61.04 Hz
  TCCR0B = TCCR0B & B11111000 | B00000100;    // set timer 0 divisor to   256 for PWM frequency of   244.14 Hz
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(4, INPUT);
  pinMode(12, INPUT);
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  digitalWrite(8, HIGH);
  digitalWrite(9, LOW);
  digitalWrite(10, HIGH);
}

void loop() {

    S_en[0] = analogRead(A0);
    S_en[5] = analogRead(A1);
    S_en[2] = analogRead(A4);
    S_en[3] = analogRead(A3);
    S_en[4] = analogRead(A2);
    S_en[1] = analogRead(A5);
   //for(int i =0; i <6; i ++){
   //  Serial.print(S_en[i]);
    //Serial.print(" ");}
    //Serial.println();
    przelicz();
    uchyb(sum());
    odleglosc = okresl_czas();
    if (on) {
    jedz();
    //delay(100);
    // Serial.println(U);
  } else {
    digitalWrite(5, LOW);
    digitalWrite(6, LOW);
  }
  if (stringComplete){
    if (inputString[0] == 'o'){
      on = true;
    }
    else if (inputString[0] == 'f'){
      on = false;
    }
    else if (inputString[0] == 'k'){
      Kp = inputString.substring(2).toInt();
    }
    else if (inputString[0] == 'i'){
      Ti = inputString.substring(2).toInt();
    }
    else if (inputString[0] == 'd'){
      Td = inputString.substring(2).toInt();
    }
    else if (inputString[0] == 'v'){
      V = inputString.substring(2).toInt();
    }
    stringComplete = false;
    inputString = "";
  }
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}

