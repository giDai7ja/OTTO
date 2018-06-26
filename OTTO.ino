#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

#if defined(ARDUINO_AVR_MEGA2560) // MEGA2560 pin assignments

#define nizhniy 14 // Нижний датчик
#define relay1 35 // Реле сдува
#define rolik1 15 //ролик 1
#define rolik2 16 //ролик 2
#define alarm 30 //реле авария
#define timeout_card 70 // время от начала движения ленты до опроса датчиков (роликов) (Число циклов)

#elif defined(ARDUINO_AVR_PRO) // Pro Mini assignments

#define nizhniy 14 // Нижний датчик
#define relay1 35 // Реле сдува
#define rolik1 15 //ролик 1
#define rolik2 16 //ролик 2
#define alarm 30 //реле авария
#define timeout_card 70 // время от начала движения ленты до опроса датчиков (роликов) (Число циклов)

#else
#error Unsupported board selection.
#endif

LiquidCrystal_I2C _lcd1(0x27, 16, 2);

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


void setup()
{
  Wire.begin();
  delay(10);
  _lcd1.init();
  _lcd1.backlight();
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

  pinMode(2, INPUT); //вход энкодера зеленый
  pinMode(3, INPUT);  //вход энкодера белый
  pinMode(nizhniy, INPUT); //сигнал приклеивания карты
  pinMode(rolik1, INPUT); //ролик 1
  pinMode(rolik2, INPUT); //ролик 2

  pinMode(relay1, OUTPUT); //реле обдува
  digitalWrite(relay1, HIGH);
  pinMode(alarm, OUTPUT); //реле авария
  digitalWrite(alarm, HIGH);

  int count = 0;

  byte hb = EEPROM.read(0);
  byte lb = EEPROM.read(1);

  Serial.begin(9600);
  Serial.println("I am ready!");
  _lcd1.setCursor(0, 0);
  _lcd1.print("Ready           ");
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
    Serial.println("Sduv ON");

    bool temp_enc = digitalRead(2);
    bool temp_enc1;

    for ( int i = 0 ; i < 20; i++ ) {
      temp_enc = digitalRead(2);
      while (temp_enc == digitalRead(2)) {};
    }

    int i = 0;

    // Первичная проверка карты
    while ( ( digitalRead(15) == HIGH || digitalRead(16) == HIGH ) && i < timeout_card ) {
      delay(1);
      i++;
    }

    //delay(timeout_card); // Должно настраиваться через меню
    digitalWrite(relay1, HIGH); // Выключение сдува
    Serial.println("Sduv OFF");
    Serial.println(i);

    if ( i >= timeout_card )
    {
      digitalWrite(alarm, LOW);
      Serial.println("Alarm!!!");
      _lcd1.setCursor(0, 0);
      _lcd1.print("Error! Press '*'");

      while ( _mkb1C4xP1 == 0 ) {
        key_scan();
      }

      digitalWrite(alarm, HIGH);
      Serial.println("Alarm OFF");
      _lcd1.setCursor(0, 0);
      _lcd1.print("Ready           ");

    }

  } //1

} //loop

///для записи уставки
/*void save() {
  byte hb = (_menuValueArray_int[1] >> 8) & 0xFF;
  EEPROM.write(0, hb);
  byte lb = _menuValueArray_int[1] & 0xFF;
  EEPROM.write(1, lb);
  }
*/
void key_scan () {
  //Плата:1
  //Наименование:Обработка Нажатий Клавиатуры
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

