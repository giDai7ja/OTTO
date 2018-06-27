#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

#if defined(ARDUINO_AVR_MEGA2560) // MEGA2560 pin assignments

#define encoderA 2 // Вход энкодера A
#define encoderB 3 // Вход энкодера B
#define nizhniy 14 // Нижний датчик
#define relay1 35 // Реле сдува
#define rolik1 15 // Ролик 1
#define rolik2 16 // Ролик 2
#define alarm 30 // Реле авария

#elif defined(ARDUINO_AVR_PRO) // Pro Mini assignments

#define encoderA 10 // Вход энкодера A
#define encoderB 10 // Вход энкодера B
#define nizhniy 11 // Нижний датчик
#define relay1 12 // Реле сдува
#define rolik1 11 // Ролик 1
#define rolik2 11 // Ролик 2
#define alarm 12 // Реле авария

#else
#error Unsupported board selection.
#endif

LiquidCrystal_I2C _lcd1(0x27, 16, 2); // Подключаем LCD дисплей

bool _mkb1C1xP1 = 0;
bool _mkb1C1xP2 = 0;
bool _mkb1C1xP3 = 0;
bool _mkb1C1xP4 = 0;
bool _mkb1C2xP1 = 0;
bool _mkb1C2xP2 = 0;
bool _mkb1C2xP3 = 0;
bool _mkb1C2xP4 = 0;
bool _mkb1C3xP1 = 0;
bool _mkb1C3xP2 = 0;
bool _mkb1C3xP3 = 0;
bool _mkb1C3xP4 = 0;
bool _mkb1C4xP1 = 0;
bool _mkb1C4xP2 = 0;
bool _mkb1C4xP3 = 0;
bool _mkb1C4xP4 = 0;

byte enc; // Число импульсов энкодера
byte t_out; // Время от начала движения ленты до опроса датчиков (роликов) (Число циклов)
byte rotate; // Вращать/не вращать

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT); // Иногда будем мигать светодиодом
  Wire.begin(); // Инициализация i2c
  delay(10); // Подумаем о вечном 10ms
  _lcd1.init(); // Инициализация LCD дисплея
  _lcd1.backlight(); // Видимо включаем подсветку

  pinMode(8, INPUT);
  digitalWrite(8, HIGH);
  pinMode(9, INPUT);
  digitalWrite(9, HIGH);
  pinMode(10, INPUT);
  digitalWrite(10, HIGH);
  pinMode(11, INPUT);
  digitalWrite(11, HIGH);
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH);
  pinMode(6, OUTPUT);
  digitalWrite(6, HIGH);
  pinMode(7, OUTPUT);
  digitalWrite(7, HIGH);

  pinMode(encoderA, INPUT); // Вход энкодера зеленый
  pinMode(encoderB, INPUT);  // Вход энкодера белый
  pinMode(nizhniy, INPUT); // Сигнал приклеивания карты
  pinMode(rolik1, INPUT); // Ролик 1
  pinMode(rolik2, INPUT); // Ролик 2

  pinMode(relay1, OUTPUT); // Реле сдува
  digitalWrite(relay1, HIGH); // Сразу выключаем
  pinMode(alarm, OUTPUT); // Реле авария
  digitalWrite(alarm, HIGH); // Сразу выключаем

  int count = 0;

  byte enc = EEPROM.read(0);
  byte t_out = EEPROM.read(1);

  Serial.begin(9600);
  Serial.println("I am ready!");
  _lcd1.setCursor(0, 0);
  _lcd1.print("Ready           ");

  _lcd1.setCursor(0, 1);
  _lcd1.print("E=");
  //  _lcd1.setCursor(2, 1);
  _lcd1.print(enc);

  _lcd1.setCursor(5, 1);
  _lcd1.print("T=");
  //  _lcd1.setCursor(7, 1);
  _lcd1.print(t_out);

}

void loop() {

  key_scan();
  if ( _mkb1C1xP4 == 1 ) {
    _lcd1.setCursor(0, 0);
    _lcd1.print("Pause...        ");
    Serial.println("Pause...");
    while ( _mkb1C1xP3 == 0 ) {
      key_scan();
    }
    _lcd1.setCursor(0, 0);
    _lcd1.print("Ready           ");
    Serial.println("Ready");
  }

  if ( digitalRead(nizhniy) == HIGH ) { //1

    digitalWrite(relay1, LOW); //Включение сдува
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("Sduv ON");

    bool temp_enc = digitalRead(encoderA);
    bool temp_enc1;

    for ( int i = 0 ; i < enc ; i++ ) {
      temp_enc = digitalRead(encoderA);
      while (temp_enc == digitalRead(encoderA)) {};
    }

    int i = 0;

    // Первичная проверка карты
    while ( ( digitalRead(15) == HIGH || digitalRead(16) == HIGH ) && i < t_out ) {
      delay(1);
      i++;
    }

    //delay(timeout_card); // Должно настраиваться через меню
    digitalWrite(relay1, HIGH); // Выключение сдува
    Serial.println("Sduv OFF");
    Serial.println(i);

    if ( i >= t_out )
    {
      digitalWrite(alarm, LOW);
      Serial.println("Alarm!!!");
      _lcd1.setCursor(0, 0);
      _lcd1.print("Error! Press [*]");

      while ( _mkb1C4xP1 == 0 ) {
        key_scan();
      }

      digitalWrite(alarm, HIGH);
      Serial.println("Alarm OFF");
      _lcd1.setCursor(0, 0);
      _lcd1.print("Ready           ");

    }
    digitalWrite(LED_BUILTIN, LOW);
  } //1

} //loop

// row column
void key_scan () {
  digitalWrite(4, 0);
  _mkb1C1xP1 = ! (digitalRead(8));
  _mkb1C1xP2 = ! (digitalRead(9));
  _mkb1C1xP3 = ! (digitalRead(10));
  _mkb1C1xP4 = ! (digitalRead(11));
  digitalWrite(4, 1);
  digitalWrite(5, 0);
  _mkb1C2xP1 = ! (digitalRead(8));
  _mkb1C2xP2 = ! (digitalRead(9));
  _mkb1C2xP3 = ! (digitalRead(10));
  _mkb1C2xP4 = ! (digitalRead(11));
  digitalWrite(5, 1);
  digitalWrite(6, 0);
  _mkb1C3xP1 = ! (digitalRead(8));
  _mkb1C3xP2 = ! (digitalRead(9));
  _mkb1C3xP3 = ! (digitalRead(10));
  _mkb1C3xP4 = ! (digitalRead(11));
  digitalWrite(6, 1);
  digitalWrite(7, 0);
  _mkb1C4xP1 = ! (digitalRead(8));
  _mkb1C4xP2 = ! (digitalRead(9));
  _mkb1C4xP3 = ! (digitalRead(10));
  _mkb1C4xP4 = ! (digitalRead(11));
  digitalWrite(7, 1);
}

