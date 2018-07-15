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

#define r_out 20  // Время от подъёма каретки до начала возврата из повёрнутого положения (ms)
#define key_tout 100 // Время для устранения дребезга контактов

LiquidCrystal_I2C _lcd1(0x27, 16, 2); // Подключаем LCD дисплей

byte enc;     // Число импульсов энкодера
byte t_out;   // Время от начала движения ленты до опроса датчиков (роликов) (ms)
byte rotate;  // Вращать/Не вращать

boolean pause = false;
boolean rt = false;

const int row[] = { row1, row2, row3, row4 }; // Массив строк
const int col[] = { col1, col2, col3, col4 }; // Массив колонок

int i;                // Счётчик циклов
int rstep = 0;        // Отслеживание состояния поворота
int sduv_step = 0;    // Шаг приклеивания карты
int key_step = 0;     // Шаг сканирования клавиатуры
int r_step = 0;
int l_step = 0;
unsigned long r_time; // Таймаут поворота
unsigned long x_time; // Таймаут роликов
unsigned long pressed_time = 0; // Таймаут кнопок
unsigned long start_time = 0; // Таймаут кнопок

unsigned int Rtime = 255; //
unsigned int Ltime = 255; //


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
  //  if (enc > 99) enc = 20;
  t_out = EEPROM.read(1);
  // if (t_out > 99) t_out = 70;
  rotate = EEPROM.read(2);
  if (rotate > 1) rotate = false;

  Serial.begin(115200);             // Настраиваем порт для вывода отладочной информации

  _lcd1.setCursor(0, 0);
  _lcd1.print("Ready           ");
  /*
    _lcd1.setCursor(0, 1);
    _lcd1.print("E=");
    _lcd1.print(enc);
  */
  _lcd1.setCursor(0, 1);
  _lcd1.print("T=");
  _lcd1.print(t_out);

  _lcd1.setCursor(9, 1);
  if ( rotate ) _lcd1.print("R=On ");
  else _lcd1.print("R=Off");

  Serial.println("I am ready!");  // Всё готово
}

void loop() {
  ScanKey();
  ScanRotate();
  ScanSduv();
  RScan();
  LScan();
} //loop


// Подпрограмма управления сдувом
void ScanSduv() {

  switch (sduv_step) {
    case 0:
      if ( digitalRead(nizhniy) == HIGH ) {
        rstep = 20;
        digitalWrite(relay1, LOW); //Включение сдува
        digitalWrite(LED_BUILTIN, HIGH);
        Serial.println("Sduv ON");
        sduv_step = 10;
      }
      break;

    case 10:
      if ( digitalRead(nizhniy) != HIGH ) {
        start_time = millis();
        x_time = start_time + 255;
        sduv_step = 20;
      }
      break;

    case 20:
      if ( millis() > x_time ) {
        digitalWrite(relay1, HIGH); // Выключение сдува
        Serial.println("Sduv OFF");
  //      Serial.print("Rtime=");
  //      Serial.println(Rtime);
  //      Serial.print("Ltime=");
  //      Serial.println(Ltime);

        if (Rtime > t_out || Ltime > t_out ) {

          digitalWrite(alarm, LOW);
          Serial.println("Alarm!!!");
    //      Serial.print("Rtime=");
   //       Serial.println(Rtime);
   //       Serial.print("Ltime=");
   //       Serial.println(Ltime);
          _lcd1.setCursor(0, 0);
          _lcd1.print("Error! ");
          _lcd1.print(Rtime);
          _lcd1.print("/");
          _lcd1.print(Ltime);
         
          while ( !key(3, 2) ) {};

          digitalWrite(alarm, HIGH);
          Serial.println("Alarm OFF");
          _lcd1.setCursor(0, 0);
          _lcd1.print("Ready           ");

        }
        sduv_step = 0;
        digitalWrite(LED_BUILTIN, LOW);
        Rtime = 255;
        Ltime = 255;
        r_step = 0;
        l_step = 0;
      }
      break;
  }
}

// Подпрограмма вращения
void ScanRotate() {
  if ( rotate ) {
    switch (rstep) {
      case 0:
        if (digitalRead(srednij) == HIGH) rstep = 10;
        break;

      case 10:
        if (digitalRead(verhnij) == HIGH) {
          rt = !rt;
          digitalWrite(povorot, rt); // Поворот
          Serial.println("Povorot");
          rstep = 0;
        }
        break;

      case 20:
        if (digitalRead(verhnij) == HIGH) rstep = 0;
        break;

    } // switch
  } // if
}

// Подпрограмма сканирования клавиатуры
void ScanKey() {

  if ( sduv_step == 0 && pressed_time < millis() ) {

    switch (key_step) {
      case 0:
        if ( key(0, 3) && !pause ) { // Постановка на паузу
          pause = true;
          pressed_time = millis() + key_tout;
          _lcd1.setCursor(0, 0);
          _lcd1.print("Pause...        ");
          Serial.println("Pause...");
        }
        key_step = 10;
        break;

      case 10:
        if ( key(1, 3) && pause ) { // Снятие с паузы
          pause = false;
          pressed_time = millis() + key_tout;
          _lcd1.setCursor(0, 0);
          _lcd1.print("Ready           ");
          Serial.println("Ready");
        }
        key_step = 20;
        break;

      case 20:
        if ( key(0, 1) && t_out < 255 ) { // Увеличение задержки
          t_out++;
          pressed_time = millis() + key_tout;
          _lcd1.setCursor(15, 1);
          _lcd1.print("*");
          _lcd1.setCursor(0, 1);
          _lcd1.print("T=");
          _lcd1.print(t_out);
          key_step = 25;
        }
        else key_step = 30;
        break;

      case 25:
        if ( !key(0, 1) ) key_step = 30;
        break;

      case 30:
        if ( key(1, 1) && t_out > 0 ) { // Уменьшение задержки
          t_out--;
          pressed_time = millis() + key_tout;
          _lcd1.setCursor(15, 1);
          _lcd1.print("*");
          _lcd1.setCursor(0, 1);
          _lcd1.print("T=");
          _lcd1.print(t_out);
          _lcd1.print(" ");
          key_step = 35;
        }
        else key_step = 40;
        break;

      case 35:
        if ( !key(1, 1) ) key_step = 40;
        break;

      case 40:
        if ( key(2, 3) && !rotate ) { // Включение поворота
          rotate = true;
          pressed_time = millis() + key_tout;
          _lcd1.setCursor(15, 1);
          _lcd1.print("*");
          _lcd1.setCursor(9, 1);
          _lcd1.print("R=On ");
          key_step = 45;
        }
        else key_step = 50;
        break;

      case 45:
        if ( !key(2, 3) ) key_step = 50;
        break;

      case 50:
        if ( key(3, 3) && rotate ) { // Выключение поворота
          rotate = false;
          pressed_time = millis() + key_tout;
          _lcd1.setCursor(15, 1);
          _lcd1.print("*");
          _lcd1.setCursor(9, 1);
          _lcd1.print("R=Off");
          key_step = 55;
        }
        else key_step = 60;
        break;

      case 55:
        if ( !key(3, 3) ) key_step = 60;
        break;

      case 60:
        if ( key(3, 0) ) { // Сохранение настроек
          pressed_time = millis() + key_tout;
          EEPROM.write(0, enc);
          EEPROM.write(1, t_out);
          EEPROM.write(2, rotate);
          _lcd1.setCursor(15, 1);
          _lcd1.print(" ");
          key_step = 65;
        }
        else key_step = 0;
        break;

      case 65:
        if ( !key(3, 0) ) key_step = 0;
        break;
    } // switch
  } // if
}

// Подпрограмма сканирования заданной кнопки
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

void RScan() {
  switch (r_step) {

    case 0:
      if ( (sduv_step != 0) && (digitalRead(15) == LOW) ) {
        Rtime = millis() - start_time;
        r_step = 10;
      }
      break;
      
    case 10:
      break;
  }
}

void LScan() {
  switch (l_step) {

    case 0:
      if ( (sduv_step != 0) && (digitalRead(16) == LOW) ) {
        Ltime = millis() - start_time;
        l_step = 10;
      }
      break;
      
    case 10:
      break;
  }
}

