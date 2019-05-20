#define _IN_ 9  // Входной сигнал
#define _OUT_ 10 // Выходной сигнал
#define TimeOut 1200 // Таймаут (мс) между носителями 
#define Pulse 10 // Длительность импульса
#define Delay_1 0
#define Delay_2 18
#define Delay_3 0
#define Delay_4 21


unsigned long CurrentTime = 0; // В переменной хранится время начала импульса
int Step = 0; // Номер шага

void setup() {
  // Настройка портов
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(_OUT_, OUTPUT);
  pinMode(_IN_, INPUT);
  digitalWrite(_OUT_, LOW);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.begin(57600);
}

// Главный цикл
void loop() {
  Main();
  ResetMain();
}

// Сброс на начало при длительном отсутствии сигнала
void ResetMain() {
  if (CurrentTime < millis()) {
    digitalWrite(LED_BUILTIN, HIGH);
    Step = 0;
  }
}

// Главный цикл запуска импульсов
void Main() {
  if (digitalRead(_IN_) == LOW) {
    CurrentTime = millis() + TimeOut;
    switch (Step) {
      case 0:
        digitalWrite(LED_BUILTIN, LOW);
        Serial.println(1);
        delay(Delay_1);
        PulseStart();
        Step = 10;
        break;
      case 10:
        Serial.println(2);
        delay(Delay_2);
        PulseStart();
        Step = 20;
        break;
      case 20:
        Serial.println(3);
        delay(Delay_3);
        PulseStart();
        Step = 30;
        break;
      case 30:
        Serial.println(4);
        delay(Delay_4);
        PulseStart();
        digitalWrite(LED_BUILTIN, HIGH);
        Step = 0;
        break;
      default:
        Step = 0;
    }
  }
}

// Формирователь импульсов
void PulseStart()
{
  digitalWrite(_OUT_, HIGH);
  delay(Pulse);
  digitalWrite(_OUT_, LOW);
  while (digitalRead(_IN_) == LOW);
}
