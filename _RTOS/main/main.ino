#include <Arduino_RouterBridge.h>
#include "LEDMatrixBounce.h"

ArduinoLEDMatrix matrix;
LEDMatrixHandler* ledMatrixHandler;

// ARDUINO Setup Function. Called ONCE.
void setup()
{
  Monitor.begin();
  Serial.begin(9600);
  matrix.begin();
  ledMatrixHandler = new LEDMatrixBounce();
}

// Arduino Loop. Will execute indefinitely
void loop()
{
  ledMatrixHandler->goForward(&matrix);
  ledMatrixHandler->goBackwards(&matrix);

  Serial.println("Restarting from Serial");
  Monitor.println("Restarting from Monitor");
}