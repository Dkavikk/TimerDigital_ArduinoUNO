#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2); // set the LCD address to 0x27 for a 16 chars and 2 line display

// Definición de pines para los botones
const int buttonHourPin = 2;
const int buttonMinutePin = 3;
const int buttonRestartLCDPin = 4;

// Definición de pines para salida
const int lightOutPin = 8;

// Variables para almacenar la hora y los minutos
int currentHour = 0;
int currentMinute = 0;
unsigned long previousMillis = 0;
// DUMMY - para probar funcionalidades mas rapido con 5 segundos
const long interval = 5000; // Intervalo de tiempo en milisegundos (1 minuto = 60000 milisegundos)

bool isOn = false;

void setup() 
{
  lcd.init(); // inicializar la pantalla lcd
  lcd.backlight();
  lcd.setCursor(5.5,0);
  lcd.print("Timer");

  Serial.begin(9600); // Iniciar comunicación serial a 9600 baudios

  pinMode(buttonHourPin, INPUT);
  pinMode(buttonMinutePin, INPUT);
  pinMode(buttonRestartLCDPin, INPUT);

  pinMode(lightOutPin, OUTPUT);
}

void loop() 
{
  unsigned long currentMillis = millis(); // Obtener el tiempo actual en milisegundos

  // Actualizar el tiempo transcurrido
  if (currentMillis - previousMillis >= interval) 
  {
    previousMillis = currentMillis;
    updateClock();
  }

  // Ajustar la hora cuando se presionen el botone
  if (digitalRead(buttonHourPin) == LOW) 
  {
    delay(150); // Esperar un breve tiempo para evitar lecturas erróneas debido al rebote del botón
    if (digitalRead(buttonHourPin) == LOW) 
    {
      currentHour = (currentHour + 1) % 24; // Incrementar la hora y mantenerla en el rango de 0 a 23
      updateSerial();
      updateLCD();
    }
  }

  // Ajustar los minutos cuando se presionen el botone
  if (digitalRead(buttonMinutePin) == LOW)
  {
    delay(150); // Esperar un breve tiempo para evitar lecturas erróneas debido al rebote del botón
    if (digitalRead(buttonMinutePin) == LOW) 
    {
      currentMinute = (currentMinute + 1) % 60; // Incrementar los minutos y mantenerlos en el rango de 0 a 59
      updateSerial();
      updateLCD();
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

  if (isOn)
  {
    digitalWrite(lightOutPin, HIGH);
  }
  else
  {
    digitalWrite(lightOutPin, LOW);
  }
}

// Función para imprimir la hora y los minutos en el Monitor Serie
void updateSerial() 
{
  Serial.print("Time: ");
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

// Función para imprimir la hora y los minutos en el LCD
void updateLCD() 
{
  lcd.clear();
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

// Función para reiniciar el DisplayLCD 
// en caso de desconectar y volver a conctar el Display
void restartLCD()
{
  lcd.init(); // inicializar la pantalla lcd
  lcd.backlight();
  lcd.clear();
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

// Función para actualizar el tiempo automáticamente
void updateClock() 
{
  currentMinute = (currentMinute + 1) % 60; // Incrementar los minutos y mantenerlos en el rango de 0 a 59

  if (currentMinute == 0) 
  {
    currentHour = (currentHour + 1) % 24; // Incrementar la hora y mantenerla en el rango de 0 a 23
  }

  // Rango de hora en las que trabajara
  if (currentHour >= 20 || currentHour < 8)
  {
    isOn = true;
  }
  else
  {
    isOn = false;
  }
  updateSerial();
  updateLCD();
}
