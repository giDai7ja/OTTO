#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

#if defined(ARDUINO_AVR_MEGA2560) // MEGA2560 pin assignments

#define encoderA 2  // Вход энкодера A
#define encoderB 3  // Вход энкодера B
#define verhnij 18  // Верхний датчик
#define srednij 17  // Средний датчик
#define nizhniy 14  // Нижний датчик
#define rolik1 15   // Ролик 1
#define rolik2 16   // Ролик 2
#define relay1 35   // Реле сдува
#define alarm 30    // Реле авария
#define povorot 32  // Реле поворота

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
#define verhnij 11  // Верхний датчик
#define srednij 11  // Средний датчик
#define nizhniy 11  // Нижний датчик
#define rolik1 11   // Ролик 1
#define rolik2 11   // Ролик 2
#define relay1 12   // Реле сдува
#define alarm 12    // Реле авария
#define povorot 12  // Реле поворота

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

#define r_out 100  // Время от подъёма каретки до начала возврата из повёрнутого положения

LiquidCrystal_I2C _lcd1(0x27, 16, 2); // Подключаем LCD дисплей

byte enc;     // Число импульсов энкодера
byte t_out;   // Время от начала движения ленты до опроса датчиков (роликов) (Число циклов)
byte rotate;  // Вращать/Не вращать

const int row[] = { row1, row2, row3, row4 }; // Массив строк
const int col[] = { col1, col2, col3, col4 }; // Массив колонок

int i;                // Счётчик циклов
int rstep = 0;        // Отслеживание состояния поворота
unsigned long r_time; // Таймаут поворота

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT); // Иногда будем мигать светодиодом
  Wire.begin();                 // Инициализация i2c
  delay(10);                    // Подумаем о вечном 10ms
  _lcd1.init();                 // Инициализация LCD дисплея
  _lcd1.backlight();            // Видимо включаем подсветку

  // Настройка клавиатуры
  for ( i = 0 ; i < 4 ; i++ ) {
    pinMode(row[i], INPUT_PULLUP); // Все выводы настроены как входы
    pinMode(col[i], INPUT_PULLUP); // и притянуты к питанию
  }

  // Входы
  pinMode(encoderA, INPUT);    // Вход энкодера зеленый
  pinMode(encoderB, INPUT);    // Вход энкодера белый
  pinMode(verhnij, INPUT);     // Верхний датчик
  pinMode(srednij, INPUT);     // Средний датчик
  pinMode(nizhniy, INPUT);     // Сигнал приклеивания карты
  pinMode(rolik1, INPUT);      // Ролик 1
  pinMode(rolik2, INPUT);      // Ролик 2

  // Выходы
  pinMode(relay1, OUTPUT);     // Реле сдува
  digitalWrite(relay1, HIGH);  // Сразу выключаем
  pinMode(alarm, OUTPUT);      // Реле авария
  digitalWrite(alarm, HIGH);   // Сразу выключаем
  pinMode(povorot, OUTPUT);    // Реле поворота
  digitalWrite(povorot, HIGH); // Сразу выключаем

  enc = EEPROM.read(0);        // Считываем настройки из EEPROM
  if (enc > 99) enc = 20;
  t_out = EEPROM.read(1);
  if (t_out > 99) t_out = 70;
  rotate = EEPROM.read(2);
  if (rotate > 1) rotate = false;

  Serial.begin(9600);             // Настраиваем порт для вывода отладочной информации

  _lcd1.setCursor(0, 0);
  _lcd1.print("Ready           ");

  _lcd1.setCursor(0, 1);
  _lcd1.print("E=");
  _lcd1.print(enc);

  _lcd1.setCursor(5, 1);
  _lcd1.print("T=");
  _lcd1.print(t_out);

  _lcd1.setCursor(10, 1);
  if ( rotate ) _lcd1.print("Ron");
  else _lcd1.print("Roff");

  Serial.println("I am ready!");  // Всё готово
}

void loop() {
  
  r_off();

  if ( key(0, 3) ) { // Постановка на паузу
    _lcd1.setCursor(0, 0);
    _lcd1.print("Pause...        ");
    Serial.println("Pause...");

    while ( !key(1, 3) ) {} // Снятие с паузы
    _lcd1.setCursor(0, 0);
    _lcd1.print("Ready           ");
    Serial.println("Ready");
  }

  if ( rotate ) {
    if ( (digitalRead(verhnij) == HIGH) && (rstep == 1) ) {
      rstep = 2;
      //           digitalWrite(povorot, LOW); // Включение поворота
      Serial.println("Povorot ON");
    }
    if ( (digitalRead(srednij) == HIGH) && (rstep == 0) ) {
      rstep = 1;
    }
  }

  if ( key(0, 0) && enc < 99 ) { // Увеличение счётчика энкодера
    enc++;
    _lcd1.setCursor(15, 1);
    _lcd1.print("*");
    _lcd1.setCursor(0, 1);
    _lcd1.print("E=");
    _lcd1.print(enc);
    delay(200);
    while ( key(0, 0) ) {}
  }

  if ( key(1, 0) && enc > 0 ) { // Уменьшение счётчика энкодера
    enc--;
    _lcd1.setCursor(15, 1);
    _lcd1.print("*");
    _lcd1.setCursor(0, 1);
    _lcd1.print("E=");
    _lcd1.print(enc);
    _lcd1.print(" ");
    delay(200);
    while ( key(1, 0) ) {}
  }

  if ( key(0, 1) && t_out < 99 ) { // Увеличение задержки
    t_out++;
    _lcd1.setCursor(15, 1);
    _lcd1.print("*");
    _lcd1.setCursor(5, 1);
    _lcd1.print("T=");
    _lcd1.print(t_out);
    delay(200);
    while ( key(0, 1) ) {}
  }

  if ( key(1, 1) && t_out > 0 ) { // Уменьшение задержки
    t_out--;
    _lcd1.setCursor(15, 1);
    _lcd1.print("*");
    _lcd1.setCursor(5, 1);
    _lcd1.print("T=");
    _lcd1.print(t_out);
    _lcd1.print(" ");
    delay(200);
    while ( key(1, 1) ) {}
  }

  if ( key(2, 3) && !rotate ) { // Включение поворота
    rotate = true;
    _lcd1.setCursor(15, 1);
    _lcd1.print("*");
    _lcd1.setCursor(10, 1);
    _lcd1.print("Ron ");
    delay(500);
    while ( key(2, 3) ) {}
  }

  if ( key(3, 3) && rotate ) { // Выключение поворота
    rotate = false;
    _lcd1.setCursor(15, 1);
    _lcd1.print("*");
    _lcd1.setCursor(10, 1);
    _lcd1.print("Roff");
    delay(500);
    while ( key(3, 3) ) {}
  }

  if ( key(3, 0) ) { // Сохранение настроек

    EEPROM.write(0, enc);
    EEPROM.write(1, t_out);
    EEPROM.write(2, rotate);

    _lcd1.setCursor(15, 1);
    _lcd1.print(" ");
    delay(500);
    while ( key(3, 0) ) {}
  }

  if ( digitalRead(nizhniy) == HIGH ) { //1

    digitalWrite(relay1, LOW); //Включение сдува
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("Sduv ON");
    if ( rstep == 2 ) rstep = 3;

    bool temp_enc = digitalRead(encoderA);

    for ( int i = 0 ; i < enc ; i++ ) {
      temp_enc = digitalRead(encoderA);
      while (temp_enc == digitalRead(encoderA)) r_off();
    }

    // Первичная проверка карты
    i = 0;
    while ( ( digitalRead(15) == HIGH || digitalRead(16) == HIGH ) && i < t_out ) {
      delay(1);
      i++;
      r_off();
    }

    digitalWrite(relay1, HIGH); // Выключение сдува
    Serial.println("Sduv OFF");
    Serial.println(i);

    if ( i >= t_out )
    {
      digitalWrite(alarm, LOW);
      Serial.println("Alarm!!!");
      _lcd1.setCursor(0, 0);
      _lcd1.print("Error! Press [#]");

      while ( !key(3, 2) ) r_off();

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

void r_off() {
  if ( (rstep == 3) && (digitalRead(verhnij) == HIGH) ) {
    rstep = 4;
    r_time = millis() + r_out;
  }
  if ( (rstep == 4) && (millis() >= r_time) ) {
    digitalWrite(povorot, HIGH); // Выключение поворота
    Serial.println("Povorot OFF");
    rstep = 0;
  }
}

