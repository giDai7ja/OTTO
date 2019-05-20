#define In1 6
#define In2 7
#define In3 8
#define In4 9

#define SW0 5
#define SW1 4

#define pause 3000
#define step_pause 3

byte step = 0;
int timeout = 0;

void setup() {
  pinMode(In1, OUTPUT);
  digitalWrite(In1, LOW);
  pinMode(In2, OUTPUT);
  digitalWrite(In2, LOW);
  pinMode(In3, OUTPUT);
  digitalWrite(In3, LOW);
  pinMode(In4, OUTPUT);
  digitalWrite(In4, LOW);

  pinMode(SW0, INPUT_PULLUP);
  pinMode(SW1, INPUT_PULLUP);

  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  timeout = 0;
  while (digitalRead(SW0)) {
    NextStep(true);
    delay(step_pause);
    timeout++;
    if (timeout > 4000) Alarm();
  }
  Stop();
  delay(pause);

  timeout = 0;
  while (digitalRead(SW1)) {
    NextStep(false);
    delay(step_pause);
    timeout++;
    if (timeout > 4000) Alarm();
  }
  Stop();
  delay(pause);

}

void NextStep(bool clockwise) {
  switch (step) {
    case 0:
      if (clockwise) digitalWrite(In2, HIGH);
      else digitalWrite(In4, HIGH);
      break;
    case 1:
      if (clockwise) digitalWrite(In1, LOW);
      else digitalWrite(In2, LOW);
      break;
    case 2:
      if (clockwise) digitalWrite(In3, HIGH);
      else digitalWrite(In1, HIGH);
      break;
    case 3:
      if (clockwise) digitalWrite(In2, LOW);
      else digitalWrite(In3, LOW);
      break;
    case 4:
      if (clockwise) digitalWrite(In4, HIGH);
      else digitalWrite(In2, HIGH);
      break;
    case 5:
      if (clockwise) digitalWrite(In3, LOW);
      else digitalWrite(In4, LOW);
      break;
    case 6:
      if (clockwise) digitalWrite(In1, HIGH);
      else digitalWrite(In3, HIGH);
      break;
    case 7:
      if (clockwise) digitalWrite(In4, LOW);
      else digitalWrite(In1, LOW);
      break;
  }
  if (clockwise) step++;
  else step--;
  step = abs(step % 8);
}

void Stop() {
  digitalWrite(In1, LOW);
  digitalWrite(In2, LOW);
  digitalWrite(In3, LOW);
  digitalWrite(In4, LOW);
}

void Alarm() {
  Stop();
  while (1) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
  }
}

