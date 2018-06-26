#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
LiquidCrystal_I2C _lcd1(0x27, 16, 2);
int _dispTempLength1 = 0;
boolean _isNeedClearDisp1;

#define nizhniy 14 // Нижний датчик
#define relay1 35 // Реле сдува
#define rolik1 15 //ролик 1
#define rolik2 16 //ролик 2
#define alarm 30 //реле авария
#define timeout_card 70 // время от начала движения ленты до опроса датчиков (роликов) (Число циклов)

const byte _menuParametrsArray[]  PROGMEM =  {1, 0, 0, 5, 5, 0, 0, 0, 0, 0, 6, 0, 2, 0, 0, 6, 6, 0, 0, 0, 0, 0, 3, 0, 3, 0, 0, 7, 7, 0, 0, 0, 0, 0, 8, 0, 4, 0, 0, 8, 8, 0, 0, 0, 0, 0, 5, 0, 5, 3, 2, 0, 0, 1, 2, 3, 1, 4, 2, 0, 6, 3, 3, 0, 0, 2, 1, 3, 1, 4, 4, 0, 7, 3, 4, 0, 0, 3, 1, 3, 1, 4, 7, 0, 8, 3, 1, 0, 0, 4, 1, 3, 1, 4, 1, 0};
int _menuValueArray_int[4];
const int _menuConstantValuesArray_int[]  PROGMEM =  {1, 10000, 0};
const char _flprogMenuNameString1[] PROGMEM = "Ust povorot";
const char _flprogMenuNameString2[] PROGMEM = "Ust rastoynie";
const char _flprogMenuNameString3[] PROGMEM = "Rolik1";
const char _flprogMenuNameString4[] PROGMEM = "Ust Rolik1";
const char _flprogMenuNameString5[] PROGMEM = "Povorot";
const char _flprogMenuNameString6[] PROGMEM = "Enkod";
const char _flprogMenuNameString7[] PROGMEM = "Ust Rolik2";
const char _flprogMenuNameString8[] PROGMEM = "Rolik2";
const char* const _flprogMenuStringsArray[] PROGMEM = { _flprogMenuNameString1,  _flprogMenuNameString2,  _flprogMenuNameString3,  _flprogMenuNameString4,  _flprogMenuNameString5,  _flprogMenuNameString6,  _flprogMenuNameString7,  _flprogMenuNameString8};
struct _menuItemStricture {
  int startInArrayIndex;
};
struct _menuMainStricture {
  String tempString;
  byte startIndex;
  byte stopIndex;
  bool isSwitchMenuAroundRing;
  _menuItemStricture  currentItem;
};
_menuItemStricture _MenuItems[8];
_menuMainStricture _MainMenus[1];
bool _gtv15 = 0;
bool _gtv17 = 0;
bool _gtv18 = 0;
bool _gtv19 = 0;
bool _gtv20 = 0;
bool _gtv21 = 0;
bool _gtv22 = 0;
bool _gtv23 = 0;
bool _gtv24 = 0;
bool _gtv25 = 0;
bool _gtv26 = 0;
bool _gtv28;
bool _gtv29;
bool _gtv30;
bool _gtv31;
bool _gtv32;
bool _gtv33;
bool _gtv1;
bool _gtv2;
bool _gtv3;
int _gtv6; //impuls enkoder
bool _gtv8;
int _disp4oldLength = 0;
int _disp10oldLength = 0;
int _disp8oldLength = 0;
int _disp1oldLength = 0;
bool _trgs3 = 0;
int _disp14oldLength = 0;
bool _trgrt1 = 0;
bool _trgrt1I = 0;
int _disp2oldLength = 0;
int _disp6oldLength = 0;
bool _trgs2 = 0;
int _disp3oldLength = 0;
bool _MenuBlock_311041158_AMO_480178635 = 0;
bool _MenuBlock_311041158_AMO_119340698 = 0;
bool _MenuBlock_311041158_AMO_516050426 = 0;
bool _MenuBlock_311041158_AMO_414965755 = 0;
bool _MenuBlock_311041158_AMO_200092525 = 0;
bool _MenuBlock_311041158_AMO_287060417 = 0;
bool _MenuBlock_311041158_AMO_379829139 = 0;
bool _MenuBlock_311041158_AMO_9875913 = 0;
String _MenuBlock_311041158_MNO;
String _MenuBlock_311041158_VNO;
bool _MenuBlock_311041158_OEIS = 0;
bool _MenuBlock_311041158_ORIS = 0;
bool _MenuBlock_311041158_OMUIS = 0;
bool _MenuBlock_311041158_OMDIS = 0;
bool _MenuBlock_311041158_OEMIS = 0;
bool _MenuBlock_311041158_OExFMIS = 0;
bool _MenuBlock_311041158_OVUIS = 0;
bool _MenuBlock_311041158_OVDIS = 0;
bool _MenuBlock_311041158_IDI_0 = 0;
bool _MenuBlock_311041158_IDI_1 = 0;
bool _MenuBlock_311041158_IDI_2 = 0;
bool _MenuBlock_311041158_IDI_3 = 0;
bool _MenuBlock_311041158_IDI_4 = 0;
bool _MenuBlock_311041158_IDI_5 = 0;
bool _MenuBlock_311041158_IDI_6 = 0;
bool _MenuBlock_311041158_IDI_7 = 0;
bool _MenuBlock_311041158_IDI_8 = 0;
bool _MenuBlock_311041158_IDI_9 = 0;
bool _MenuBlock_311041158COMMAOIS = 0;
bool _MenuBlock_311041158MinusIOS = 0;
bool _MenuBlock_311041158_BackspaceIOS = 0;
bool _MenuBlock_311041158_AMMO = 0;
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
int _disp7oldLength = 0;
bool _trgs1 = 0;
bool _tim1I = 0;
bool _tim1O = 0;
unsigned long _tim1P = 0UL;
int _disp17oldLength = 0;
int _disp5oldLength = 0;
int _disp11oldLength = 0;
int _disp9oldLength = 0;
///
int count; //число энкодера
bool encEnable = false; //для счета
//int nizhniy = 14; //концевик нижний приклеивания карты
//int rolik1 = 15; //ролик 1
//int rolik2 = 16; //ролик 2
int mid_konc = 17;  //концевик средний
int hight_konc = 18;  //концевик верхний
int setpoint = 19; //заменить
int vih_produkt = 20; //выхода продукта
bool low_reached = false; //нижний
bool mid_reached = false; //средний
//int relay1 = 31; //реле обдува
//int alarm = 30; //реле авария
int povorot = 32; //реле поворота
bool writeFront = false;
bool alarm30 = false;
unsigned long t = millis(); //для поворота
///
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
  _MenuItems[0].startInArrayIndex = 0;
  _MenuItems[1].startInArrayIndex = 12;
  _MenuItems[2].startInArrayIndex = 24;
  _MenuItems[3].startInArrayIndex = 36;
  _MenuItems[4].startInArrayIndex = 48;
  _MenuItems[5].startInArrayIndex = 60;
  _MenuItems[6].startInArrayIndex = 72;
  _MenuItems[7].startInArrayIndex = 84;
  _MainMenus[0].tempString = "";
  _MainMenus[0].startIndex = 1;
  _MainMenus[0].isSwitchMenuAroundRing = 1;
  _MainMenus[0].stopIndex = 4;
  _MainMenus[0].currentItem = _MenuItems[0];
  ///
  pinMode(2, INPUT); //вход энкодера зеленый
  pinMode(3, INPUT);  //вход энкодера белый
  pinMode(nizhniy, INPUT); //сигнал приклеивания карты
  pinMode(rolik1, INPUT); //ролик 1
  pinMode(rolik2, INPUT); //ролик 2
  pinMode(mid_konc, INPUT);  //средний
  pinMode(hight_konc, INPUT); //верхний
  pinMode(setpoint, INPUT); //заменить
  pinMode(vih_produkt, INPUT); //выхода продукта
  pinMode(relay1, OUTPUT); //реле обдува
  digitalWrite(relay1, HIGH);
  pinMode(alarm, OUTPUT); //реле авария
  digitalWrite(alarm, HIGH);
  pinMode(povorot, OUTPUT); //реле поворота
  digitalWrite(povorot, HIGH);

  //  Serial.begin(115200);
  count = 0;
  //  attachInterrupt(0, readEncoder, RISING);
  byte hb = EEPROM.read(0);
  byte lb = EEPROM.read(1);
  _menuValueArray_int[1] = (hb << 8) + lb;
  ///
  Serial.begin(9600);
  Serial.println("I am ready!");
  _lcd1.setCursor(0, 1);
  _lcd1.print("Ready            ");
}
void loop() {
  //  digitalWrite(alarm, LOW); // Сброс алярма
  key_scan();
  if ( _mkb1C1xP4 == 1 ) {
    _lcd1.setCursor(0, 1);
    _lcd1.print("Pause...         ");
    Serial.println("Pause...");
    while ( _mkb1C1xP3 == 0 ) {
      key_scan();
    }
    _lcd1.setCursor(0, 1);
    _lcd1.print("Ready            ");
    Serial.println("Ready");
  }

  if ( digitalRead(nizhniy) == HIGH ) { //1
    //    delay(50);
    //    if ( digitalRead(nizhniy) == HIGH ) { //2
    digitalWrite(relay1, LOW); //Включение сдува
    Serial.println("Sduv ON");

    //int i = 0;
    bool temp_enc = digitalRead(2);
    bool temp_enc1;
    //    while (temp_enc == digitalRead(2)) {};

    //    while ( digitalRead(15) == HIGH && digitalRead(16) == HIGH )
    //    {
    //      temp_enc1 = digitalRead(2);
    //      if ( temp_enc != temp_enc1 ) {
    //        temp_enc = temp_enc1;
    //        i++;
    //      }
    //    }

    for ( int i = 0 ; i < 20; i++ ) {
      temp_enc = digitalRead(2);
      while (temp_enc == digitalRead(2)) {};
    }

    int i = 0;
//    while ( digitalRead(15) == HIGH && digitalRead(16) == HIGH && i < timeout_card ) {
    while ( ( digitalRead(15) == HIGH || digitalRead(16) == HIGH ) && i < timeout_card ) {
      delay(1);
      i++;
    }

    //delay(timeout_card); // Должно настраиваться через меню
    digitalWrite(relay1, HIGH); // Выключение сдува
    Serial.println("Sduv OFF");
    //  Serial.print("Delay = ");
    Serial.println(i);
    // Serial.println(" ms");
    //      delay(10);

    // Первичная проверка карты

    // if ( digitalRead(15) == HIGH || digitalRead(16) == HIGH )
    if ( i >= timeout_card )
    {
      digitalWrite(alarm, LOW);
      Serial.println("Alyarma!!!");
      _lcd1.setCursor(0, 1);
      _lcd1.print("Error! Press '*'");

      while ( _mkb1C4xP1 == 0 ) {
        key_scan();
      }

      digitalWrite(alarm, HIGH);
      Serial.println("Alyarma OFF!!!");
      _lcd1.setCursor(0, 1);
      _lcd1.print("Ready            ");

    }



    /*  int count = 0;
      int zaderzhka = 10;
      while ( digitalRead(15) == HIGH && digitalRead(16) == HIGH && count < 100 )
      {
        count += zaderzhka;
        delay(zaderzhka);
      }

      if ( count >= 100 ) {
        digitalWrite(alarm, HIGH);
      }
      else {
        delay(10);
        if ( digitalRead(15) == HIGH || digitalRead(16) == HIGH ) {
          digitalWrite(alarm, HIGH);
        }
      }
    */


    //    } //2





  } //1



  // delay (2000);


} //loop
//функция энкодера
void readEncoder() {
  if (encEnable)
    count += digitalRead(3) ? 1 : -1;
}
///для записи уставки
void save() {
  byte hb = (_menuValueArray_int[1] >> 8) & 0xFF;
  EEPROM.write(0, hb);
  byte lb = _menuValueArray_int[1] & 0xFF;
  EEPROM.write(1, lb);
}

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

