#define MAX_VAL 910
#define MIN_VAL 60
#define TILT_1_VAL 850
#define TILT_2_VAL 790
#define OPEN 8
#define TILT 9
#define SILNIK_PWM 5
#define SILNIK_K1 11
#define SILNIK_K2 13
#define POSITION A3
#define STACYJKA 7
int cur_val = 0;
int value;
int opened = 0;
int last_position = 1;
int memory = 0;
int moving = 1;

void setup() {
  // put your setup code here, to run once:
  //TCCR0B = TCCR0B & B11111000 | B00000100;    // set timer 0 divisor to   256 for PWM frequency of   244.14 Hz
  TCCR0B = TCCR0B & B11111000 | B00000001;    // set timer 0 divisor to     1 for PWM frequency of 62500.00 Hz
  Serial.begin(9600);
  pinMode(2,OUTPUT);
  pinMode(SILNIK_PWM,OUTPUT);
  pinMode(SILNIK_K2,OUTPUT);
  pinMode(SILNIK_K1,OUTPUT);
  pinMode(OPEN,INPUT);
  pinMode(TILT,INPUT);
  pinMode(STACYJKA,INPUT);
  digitalWrite(2, HIGH);
  pinMode(POSITION,INPUT);
  analogWrite(SILNIK_PWM, 0);
}

void close_nav(){ // ZAMYKANIE NAWIGACJI
  digitalWrite(SILNIK_K1, LOW);
  digitalWrite(SILNIK_K2, HIGH);
}

void open_nav(){  // OTWIERANIE NAWIGACJI
  digitalWrite(SILNIK_K1, HIGH);
  digitalWrite(SILNIK_K2, LOW);
}

void move_engine(int speed_val){
  analogWrite(SILNIK_PWM, speed_val);
  cur_val = analogRead(POSITION);
  while (cur_val < value - 30 || cur_val > value + 30){
    cur_val = analogRead(POSITION); // WCZYTANIE AKTUALNEJ POZYCJI WINDY
    //analogWrite(SILNIK_PWM, (abs(value - cur_val))/10 + 170);
  }
  analogWrite(SILNIK_PWM, 0);
}

void move_nav(){
  switch (opened) {
    case 1:
      value = MAX_VAL;
      break;
    case 2:
      value = TILT_1_VAL;
      break;
    case 3:
      value = TILT_2_VAL;
      break;
    case 0:
      value = MIN_VAL;
      break;
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  if (digitalRead(STACYJKA) == HIGH){ // SPRAWDZENIE CZY JEST ZAPLON
    if (last_position > 0 && opened == 0){  // POWROT DO OSTATNIEJ POZYCJI
      opened = last_position;
      open_nav();
      move_nav();
      move_engine(255);
    }
    if (opened){  // PRZYCISK ZMIANY NACHYLENIA
      if (digitalRead(TILT) == LOW){
        delay(25000);
        //delay(100);
        switch (opened){
        case 1:
          opened = 2;
          last_position = 2;
          close_nav();
          break;
        case 2:
          opened = 3;
          last_position = 3;
          close_nav();
          break;
        case 3:
          opened = 1;
          last_position = 1;
          open_nav();
          break;
        }
        move_nav();
        move_engine(150);
      }
      //Serial.println(analogRead(A3));
    }
    if (digitalRead(OPEN) == LOW){ // PRZYCISK ZAMYKANIA
      delay(25000);
      //delay(100);
      if (opened){
        memory = opened;
        opened = 0;
        last_position = 0;
        close_nav();
      } else {
        opened = memory;
        last_position = memory;
        open_nav();
      }
      move_nav();
      move_engine(255); // RUCH WINDY
    }
    //Serial.print(opened);
    //Serial.print("     ");
    //Serial.println(last_position);
  } else  // ZAMYKANIE PO STACYJCE
  {
    if (opened)
    {
      last_position = opened; // ZAPISYWANIE OSTATNIEJ POZYCJI
      delay(1250000);
      if (digitalRead(STACYJKA) == 0){
        close_nav();  // ZAMYKANIE NAWIGACJI
        while (opened > 0)
        {
          if (analogRead(POSITION) < MIN_VAL + 20) // SPRAWDZANIE CZY NAWIGACJA JEST JUŻ ZAMKNIĘTA
          {
            opened = 0;
            analogWrite(SILNIK_PWM, 0);
          } else {
            analogWrite(SILNIK_PWM, 255);
          }
        }
      }
      Serial.print(opened);
      Serial.print("     ");
      Serial.println(last_position);
    }
  }
  //Serial.println(analogRead(A3));
}
