#include <Arduino_FreeRTOS.h>
// Почему бы не применить FreeRTOS  :-)

// Всего будет 3 задачи
void Task1( void *pvParameters );
void Task2( void *pvParameters );
void TaskBlink( void *pvParameters );  // Помигаем светодиодом, просто так

// Эти параметры можно менять по вкусу
#define pause 5000    // ms (Пауза между остановкой двигателя и реверсом)
#define mini_pause 500  // ms (Пол периода мигания светодиода)

// the setup function runs once when you press reset or power the board
// Инициализация, выполняется один раз после подачи питания или ресета
void setup() {
  pinMode(2, INPUT_PULLUP); // Первый концевик первого двигателя
  pinMode(3, INPUT_PULLUP); // Второй концевик первого двигателя
  pinMode(4, INPUT_PULLUP); // Первый концевик второго двигателя
  pinMode(5, INPUT_PULLUP); // Второй концевик второго двигателя
  pinMode(6, OUTPUT);       // Реле прямого направления первого двигателя
  digitalWrite(6, HIGH);    // Высокий уровень - реле выключено
  pinMode(7, OUTPUT);       // Реле обратного направления первого двигателя
  digitalWrite(7, HIGH);    // Высокий уровень - реле выключено
  pinMode(8, OUTPUT);       // Реле прямого направления второго двигателя
  digitalWrite(8, HIGH);    // Высокий уровень - реле выключено
  pinMode(9, OUTPUT);      // Реле обратного направления второго двигателя
  digitalWrite(9, HIGH);    // Высокий уровень - реле выключено
  pinMode(LED_BUILTIN, OUTPUT); // Просто светодиод

  if ( (digitalRead(2) == HIGH) && (digitalRead(3) == HIGH) )  {
    digitalWrite(6, LOW);    // Если находимся в промежуточном положении, начинаем движение
  }

  if ( (digitalRead(4) == HIGH) && (digitalRead(5) == HIGH) )  {
    digitalWrite(8, LOW);    // Если находимся в промежуточном положении, начинаем движение
  }

  // Создаём задачи, ничего интересного
  xTaskCreate(
    Task1
    ,  (const portCHAR *)"Tuda1"
    ,  128
    ,  NULL
    ,  3
    ,  NULL );

  xTaskCreate(
    Task2
    ,  (const portCHAR *)"Tuda2"
    ,  128
    ,  NULL
    ,  3
    ,  NULL );

  xTaskCreate(
    TaskBlink
    ,  (const portCHAR *) "Blink"
    ,  128
    ,  NULL
    ,  3
    ,  NULL );
}

void loop()
{
  // Empty. Things are done in Tasks.
  // Тут пусто, так надо
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

// Задачи, обработка. Все однотипные, комментарии только в первой задаче

void Task1(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
  for (;;) // Бесконечный цикл
  {
    // Если сработал нужный концевик и второй не сработал (на всякий случай)
    if ( ( digitalRead(2) == LOW ) && ( digitalRead(3) == HIGH ) ) {
      digitalWrite(7, HIGH); // Сразу выключаем двигатель
      digitalWrite(6, HIGH); // Для надёжности выключаем оба реле, на всякий случай
      vTaskDelay(pause / portTICK_PERIOD_MS); // Ждём 5 секунд
        digitalWrite(6, LOW); // Подключаем двигатель
        digitalWrite(7, HIGH);
      while ( digitalRead(2) == LOW ) {
        digitalWrite(6, LOW); // Подключаем двигатель
        digitalWrite(7, HIGH);
      }
      vTaskDelay(5);
    }
    if ( ( digitalRead(3) == LOW ) && ( digitalRead(2) == HIGH ) ) {
      digitalWrite(6, HIGH);
      digitalWrite(7, HIGH);
      vTaskDelay(pause / portTICK_PERIOD_MS);
        digitalWrite(7, LOW);
        digitalWrite(6, HIGH);
      while ( digitalRead(3) == LOW ) {
        digitalWrite(7, LOW);
        digitalWrite(6, HIGH);
      }
      vTaskDelay(5);
    }
  }
}

void Task2(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
  for (;;)
  {
    if ( (digitalRead(4) == LOW ) && ( digitalRead(5) == HIGH ) ) {
      digitalWrite(9, HIGH);
      digitalWrite(8, HIGH);
      vTaskDelay(pause / portTICK_PERIOD_MS);
        digitalWrite(8, LOW);
        digitalWrite(9, HIGH);
      while ( digitalRead(4) == LOW ) {
        digitalWrite(8, LOW);
        digitalWrite(9, HIGH);
      }
      vTaskDelay(5);
    }
    if ( ( digitalRead(5) == LOW ) && ( digitalRead(4) == HIGH ) ) {
      digitalWrite(8, HIGH);
      digitalWrite(9, HIGH);
      vTaskDelay(pause / portTICK_PERIOD_MS);
        digitalWrite(9, LOW);
        digitalWrite(8, HIGH);
      while ( digitalRead(5) == LOW ) {
        digitalWrite(9, LOW);
        digitalWrite(8, HIGH);
      }
      vTaskDelay(5);
    }
  }
}

void TaskBlink(void *pvParameters)
{
  (void) pvParameters;
  for (;;)
  {
    digitalWrite(LED_BUILTIN, HIGH); // светим
    vTaskDelay(mini_pause / portTICK_PERIOD_MS); // ждём
    digitalWrite(LED_BUILTIN, LOW); // не светим
    vTaskDelay(mini_pause / portTICK_PERIOD_MS); // ждём
  }
}

