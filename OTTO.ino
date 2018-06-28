#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

#if defined(ARDUINO_AVR_MEGA2560) // MEGA2560 pin assignments

#define encoderA 2  // Вход энкодера A
#define encoderB 3  // Вход энкодера B
#define nizhniy 14  // Нижний датчик
#define relay1 35   // Реле сдува
#define rolik1 15   // Ролик 1
#define rolik2 16   // Ролик 2
#define alarm 30    // Реле авария

// Описание подключения клавиатуры
#define row1 11     // Строка 1
#define row2 10     // Строка 2
#define row3 9      // Строка 3
#define row4 8      // Строка 4

#define col1 7      // Колонка 1
#define col2 6      // Колонка 2
#define col3 5      // Колонка 3
#define col4 4      // Колонка 4


#elif defined(ARDUINO_AVR_PRO) // Pro Mini assignments

#define encoderA 10 // Вход энкодера A
#define encoderB 10 // Вход энкодера B
#define nizhniy 11 // Нижний датчик
#define relay1 12 // Реле сдува
#define rolik1 11 // Ролик 1
#define rolik2 11 // Ролик 2
#define alarm 12 // Реле авария

// Описание подключения клавиатуры
#define row1 2      // Строка 1
#define row2 3      // Строка 2
#define row3 4      // Строка 3
#define row4 5      // Строка 4

#define col1 6      // Колонка 1
#define col2 7      // Колонка 2
#define col3 8      // Колонка 3
#define col4 9      // Колонка 4

#else
#error Unsupported board selection.
#endif

LiquidCrystal_I2C _lcd1(0x27, 16, 2); // Подключаем LCD дисплей

byte enc; // Число импульсов энкодера
byte t_out; // Время от начала движения ленты до опроса датчиков (роликов) (Число циклов)
byte rotate; // Вращать/не вращать

const int row[] = { row1, row2, row3, row4 };
const int col[] = { col1, col2, col3, col4 };

int i;
int count = 0;

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT); // Иногда будем мигать светодиодом
  Wire.begin(); // Инициализация i2c
  delay(10); // Подумаем о вечном 10ms
  _lcd1.init(); // Инициализация LCD дисплея
  _lcd1.backlight(); // Видимо включаем подсветку

  // Настройка клавиатуры
  for ( i = 0 ; i < 4 ; i++ ) {
    pinMode(row[i], INPUT_PULLUP);
    pinMode(col[i], INPUT_PULLUP);
  }

  // Входы
  pinMode(encoderA, INPUT); // Вход энкодера зеленый
  pinMode(encoderB, INPUT);  // Вход энкодера белый
  pinMode(nizhniy, INPUT); // Сигнал приклеивания карты
  pinMode(rolik1, INPUT); // Ролик 1
  pinMode(rolik2, INPUT); // Ролик 2

  // Выходы
  pinMode(relay1, OUTPUT); // Реле сдува
  digitalWrite(relay1, HIGH); // Сразу выключаем
  pinMode(alarm, OUTPUT); // Реле авария
  digitalWrite(alarm, HIGH); // Сразу выключаем

  enc = EEPROM.read(0);
  if (enc > 99) enc = 20;
  t_out = EEPROM.read(1);
  if (t_out > 99) t_out = 70;
  rotate = EEPROM.read(2);
  if (rotate > 1) rotate = false;

  Serial.begin(9600);
  Serial.println("I am ready!");
  _lcd1.setCursor(0, 0);
  _lcd1.print("Ready           ");

  _lcd1.setCursor(0, 1);
  _lcd1.print("E=");
  _lcd1.print(enc);

  _lcd1.setCursor(5, 1);
  _lcd1.print("T=");
  _lcd1.print(t_out);

  _lcd1.setCursor(10, 1);
  _lcd1.print("R");
  if ( rotate ) _lcd1.print("on");
  else _lcd1.print("off");
}

void loop() {

  //  key_scan();
  if ( key(0, 3) ) { // Постановка на паузу
    _lcd1.setCursor(0, 0);
    _lcd1.print("Pause...        ");
    Serial.println("Pause...");

    while ( !key(1, 3) ) {} // Снятие с паузы
    _lcd1.setCursor(0, 0);
    _lcd1.print("Ready           ");
    Serial.println("Ready");
  }

  if ( key(0, 0) && enc < 99 ) { // Увеличение счётчика энкодера
    enc++;
    _lcd1.setCursor(15, 1);
    _lcd1.print("*");
    _lcd1.setCursor(0, 1);
    _lcd1.print("E=");
    _lcd1.print(enc);
    while ( key(0, 0) ) {}
    delay(200);
  }

  if ( key(1, 0) && enc > 0 ) { // Уменьшение счётчика энкодера
    enc--;
    _lcd1.setCursor(15, 1);
    _lcd1.print("*");
    _lcd1.setCursor(0, 1);
    _lcd1.print("E=");
    _lcd1.print(enc);
    _lcd1.print(" ");
    while ( key(1, 0) ) {}
    delay(200);
  }

  if ( key(0, 1) && t_out < 99 ) { // Увеличение задержки
    t_out++;
    _lcd1.setCursor(15, 1);
    _lcd1.print("*");
    _lcd1.setCursor(5, 1);
    _lcd1.print("T=");
    _lcd1.print(t_out);
    while ( key(0, 1) ) {}
    delay(200);
  }

  if ( key(1, 1) && t_out > 0 ) { // Уменьшение задержки
    t_out--;
    _lcd1.setCursor(15, 1);
    _lcd1.print("*");
    _lcd1.setCursor(5, 1);
    _lcd1.print("T=");
    _lcd1.print(t_out);
    _lcd1.print(" ");
    while ( key(1, 1) ) {}
    delay(200);
  }

  if ( key(2, 3) && !rotate ) { // Включение поворота
    rotate = true;
    _lcd1.setCursor(15, 1);
    _lcd1.print("*");
    _lcd1.setCursor(10, 1);
    _lcd1.print("Ron ");
    while ( key(2, 3) ) {}
    delay(500);
  }

  if ( key(3, 3) && rotate ) { // Выключение поворота
    rotate = false;
    _lcd1.setCursor(15, 1);
    _lcd1.print("*");
    _lcd1.setCursor(10, 1);
    _lcd1.print("Roff");
    while ( key(3, 3) ) {}
    delay(500);
  }

  if ( key(3, 0) ) { // Сохранение настроек

    EEPROM.write(0, enc);
    EEPROM.write(1, t_out);
    EEPROM.write(2, rotate);

    _lcd1.setCursor(15, 1);
    _lcd1.print(" ");
    while ( key(3, 0) ) {}
    delay(500);
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
      _lcd1.print("Error! Press [#]");

      while ( !key(3, 2) ) {}

      digitalWrite(alarm, HIGH);
      Serial.println("Alarm OFF");
      _lcd1.setCursor(0, 0);
      _lcd1.print("Ready           ");

    }
    digitalWrite(LED_BUILTIN, LOW);
  } //1

} //loop


bool key(int x, int y) {
  bool pressed;

  pinMode(row[x], OUTPUT);        // Перводим вывод на выход
  digitalWrite(row[x], LOW);      // Притягиваем к земле

  if ( digitalRead(col[y]) == LOW )
    pressed = true;
  else pressed = false;

  digitalWrite(row[x], HIGH);     // Возвращаем как было
  pinMode(row[x], INPUT_PULLUP);  // Перводим вывод на вход

  return pressed;
}

