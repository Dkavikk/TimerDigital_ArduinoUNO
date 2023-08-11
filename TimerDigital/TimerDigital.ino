#include "Types.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2); // set the LCD address to 0x27 for a 16 chars and 2 line display


// Definición de pines para los botones
const int buttonConfigPin = 2;
const int buttonTimePin = 3;

const int buttonRestartLCDPin = 5;

// Definición de pines para salida
const int lightOutPin = 8;

unsigned long buttonPressStartTime = 0; // Tiempo en que se inicia la presión del botón
bool buttonPressed = false; // Variable para saber si el botón está presionado

// Variables para almacenar la hora y los minutos
int currentHour = 0;
int currentMinute = 0;
int ONHour = 0;
int ONMinute = 0;
int OFFHour = 0;
int OFFMinute = 0;
unsigned long previousMillis = 0;
// DUMMY - para probar funcionalidades mas rapido con 1 segundos
const long interval = 1000; // Intervalo de tiempo en milisegundos (1 minuto = 60000 milisegundos)

TimerState_t timerState; // Estado del Timer
ConfigState_t configState; // Estado de Configuracion del timer

bool isOn = false;

void setup() 
{
  timerState = STARTUP;

  lcd.init(); // inicializar la pantalla lcd
  lcd.backlight();
  lcd.setCursor(5.5,0);
  lcd.print("Timer");

  Serial.begin(9600); // Iniciar comunicación serial a 9600 baudios

  pinMode(buttonConfigPin, INPUT_PULLUP);
  pinMode(buttonTimePin, INPUT_PULLUP);
  pinMode(buttonRestartLCDPin, INPUT_PULLUP);

  pinMode(lightOutPin, OUTPUT);

  delay(1500);
  timerState = LOOP;
}

void loop() 
{
  unsigned long currentMillis = millis(); // Obtener el tiempo actual en milisegundos

  if (timerState == LOOP)
  {
    // Actualizar el tiempo transcurrido
    if (currentMillis - previousMillis >= interval) 
    {
      previousMillis = currentMillis;
      updateClock();
    }
    if (digitalRead(buttonConfigPin) == LOW) 
    {
      delay(150);
      if (digitalRead(buttonConfigPin) == LOW) 
      {
        timerState = CONFIG;
        updateLCD();
        updateSerial();
      }
    }

    if (isOn)
    {
      digitalWrite(lightOutPin, HIGH);
    }
    else
    {
      digitalWrite(lightOutPin, LOW);
    }
  }
  else if (timerState == CONFIG)
  {
    if (digitalRead(buttonTimePin) == LOW) 
    {
      delay(150);
      if (digitalRead(buttonTimePin) == LOW) 
      {
        if (configState == CURRENT_HOUR)
        {
          currentHour = (currentHour + 1) % 24; // Incrementar la hora y mantenerla en el rango de 0 a 23
        }
        else if (configState == CURRENT_MINUTE)
        {
          currentMinute = (currentMinute + 1) % 60; // Incrementar los minutos y mantenerlos en el rango de 0 a 59
        }
        else if (configState == ON_HOUR)
        {
          ONHour = (ONHour + 1) % 24;
        }
        else if (configState == ON_MINUTE)
        {
          ONMinute = (ONMinute + 1) % 60;
        }
        else if (configState == OFF_HOUR)
        {
          OFFHour = (OFFHour + 1) % 24;
        }
        else if (configState == OFF_MINUTE)
        {
          OFFMinute = (OFFMinute + 1) % 60;
        }
        updateLCD();
        updateSerial();
      }
    }

    if (digitalRead(buttonConfigPin) == LOW && !buttonPressed) 
    {
      delay(150);
      if (digitalRead(buttonConfigPin) == LOW && !buttonPressed) 
      {
        buttonPressed = true;
        buttonPressStartTime = currentMillis;
      }
    }

    if (digitalRead(buttonConfigPin) == HIGH && buttonPressed) 
    {
        buttonPressed = false;
        unsigned long buttonPressDuration = currentMillis - buttonPressStartTime;
        
        if (buttonPressDuration < 999) 
        {
          configState = static_cast<ConfigState_t>((configState + 1) % (OFF_MINUTE + 1));
          updateLCD();
          updateSerial();
        }
        else if (buttonPressDuration >= 1000) 
        {
          timerState = LOOP;
          configState = CURRENT_HOUR;
          updateLCD();
          updateSerial();
        }
    }
  }

  // Reinicia el LCD cuando se presionen el botone
  if (digitalRead(buttonRestartLCDPin) == LOW) 
  {
    delay(150); // Esperar un breve tiempo para evitar lecturas erróneas debido al rebote del botón
    if (digitalRead(buttonRestartLCDPin) == LOW) 
    {
      restartLCD();
    }
  }
}

// Función para imprimir la hora y los minutos en el Monitor Serie
void updateSerial() 
{
  if (timerState == LOOP)
  {
    Serial.print("Hora: ");
    if (currentHour < 10) 
    {
      Serial.print("0");
    }
    Serial.print(currentHour);
    Serial.print(":");

    if (currentMinute < 10) 
    {
      Serial.print("0");
    }
    Serial.print(currentMinute);

    Serial.print(" / ");
    if (isOn)
    {
      Serial.println("Encendido");
    }
    else
    {
      Serial.println("Apagado");
    }
  }
  else if (timerState == CONFIG)
  {
    if (configState == CURRENT_HOUR || configState == CURRENT_MINUTE)
    {
      Serial.print("Hora actual: ");
      if (currentHour < 10) 
      {
        Serial.print("0");
      }
      Serial.print(currentHour);
      Serial.print(":");

      if (currentMinute < 10) 
      {
        Serial.print("0");
      }
      Serial.print(currentMinute);
      if (configState == CURRENT_HOUR)
      {
        Serial.println(" H");
      }
      else if (configState == CURRENT_MINUTE) 
      {
        Serial.println(" M");
      }
    }
    else if (configState == ON_HOUR || configState == ON_MINUTE)
    {
      Serial.print("Hora de ecendido: ");
      if (ONHour < 10) 
      {
        Serial.print("0");
      }
      Serial.print(ONHour);
      Serial.print(":");

      if (ONMinute < 10) 
      {
        Serial.print("0");
      }
      Serial.print(ONMinute);
      if (configState == ON_HOUR)
      {
        Serial.println(" H");
      }
      else if (configState == ON_MINUTE) 
      {
        Serial.println(" M");
      }
    }
    else if (configState == OFF_HOUR || configState == OFF_MINUTE)
    {
      Serial.print("Hora de apagado: ");
      if (OFFHour < 10) 
      {
        Serial.print("0");
      }
      Serial.print(OFFHour);
      Serial.print(":");

      if (OFFMinute < 10) 
      {
        Serial.print("0");
      }
      Serial.print(OFFMinute);
      if (configState == OFF_HOUR)
      {
        Serial.println(" H");
      }
      else if (configState == OFF_MINUTE) 
      {
        Serial.println(" M");
      }
    }
  }
}

// Función para imprimir la hora y los minutos en el LCD
void updateLCD() 
{
  lcd.clear();

  if (timerState == LOOP)
  {
    lcd.print("Hora: ");
    if (currentHour < 10) 
    {
      lcd.print("0");
    }
    lcd.print(currentHour);
    lcd.print(":");
    if (currentMinute < 10) 
    {
      lcd.print("0");
    }
    lcd.print(currentMinute);

    lcd.setCursor(0, 1);
    if (isOn)
    {
      lcd.print("Encendido");
    }
    else
    {
      lcd.print("Apagado");
    }
  }
  else if (timerState == CONFIG)
  {
    if (configState == CURRENT_HOUR || configState == CURRENT_MINUTE)
    {
      lcd.print("Hora actual: ");
      lcd.setCursor(0, 1);

      if (currentHour < 10) 
      {
        lcd.print("0");
      }
      lcd.print(currentHour);
      lcd.print(":");
      if (currentMinute < 10) 
      {
        lcd.print("0");
      }
      lcd.print(currentMinute);
      if (configState == CURRENT_HOUR)
      {
        lcd.print(" H");
      }
      else if (configState == CURRENT_MINUTE) 
      {
        lcd.print(" M");
      }
    }
    else if (configState == ON_HOUR || configState == ON_MINUTE)
    {
      lcd.print("Hora de ON: ");
      lcd.setCursor(0, 1);

      if (ONHour < 10) 
      {
        lcd.print("0");
      }
      lcd.print(ONHour);
      lcd.print(":");
      if (ONMinute < 10) 
      {
        lcd.print("0");
      }
      lcd.print(ONMinute);
      if (configState == ON_HOUR)
      {
        lcd.print(" H");
      }
      else if (configState == ON_MINUTE) 
      {
        lcd.print(" M");
      }
    }
    else if (configState == OFF_HOUR || configState == OFF_MINUTE)
    {
      lcd.print("Hora de OFF: ");
      lcd.setCursor(0, 1);

      if (OFFHour < 10) 
      {
        lcd.print("0");
      }
      lcd.print(OFFHour);
      lcd.print(":");
      if (OFFMinute < 10) 
      {
        lcd.print("0");
      }
      lcd.print(OFFMinute);
      if (configState == OFF_HOUR)
      {
        lcd.print(" H");
      }
      else if (configState == OFF_MINUTE) 
      {
        lcd.print(" M");
      }
    }
  }
}

// Función para reiniciar el DisplayLCD 
// en caso de desconectar y volver a conctar el Display
void restartLCD()
{
  lcd.init();
  lcd.backlight();
  updateLCD();
}

// Función para actualizar el tiempo automáticamente
void updateClock() 
{
  currentMinute = (currentMinute + 1) % 60; // Incrementar los minutos y mantenerlos en el rango de 0 a 59

  if (currentMinute == 0) 
  {
    currentHour = (currentHour + 1) % 24; // Incrementar la hora y mantenerla en el rango de 0 a 23
  }

  // Rango de hora en las que trabajara
  if (currentHour >= ONHour && currentHour < OFFHour)
  {
    isOn = true;
  }
  else
  {
    isOn = false;
  }
  updateLCD();
  updateSerial();
}
