#include "MsgPack.h"
#include "Arduino_LED_Matrix.h"
#include "Arduino_RouterBridge.h"

#define LED_MAT_H 8  //correct value = 8
#define LED_MAT_W 15  //correct value = 13

ArduinoLEDMatrix matrix;
int col = 0;
byte full[LED_MAT_H][LED_MAT_W];

// ARDUINO Setup Function. Called ONCE.
void setup()
{
  Monitor.begin();
  Serial.begin(9600);
  matrix.begin();
}

// Arduino Loop. Will execute indefinitely
void loop()
{
  goForward();
  goBackwards();

  Serial.println("Restarting from Serial");
  Monitor.println("Restarting from Monitor");
}

void fillColumn(byte* ledMat, int col, int val)
{
  //Applies 1 to each cell in the row, column fixed
  for (int i=0; i<LED_MAT_H; i++)
  {
    ledMat[i*LED_MAT_W + col] = val;
  }

}

void goForward()
{
  for(int i=0; i<LED_MAT_W; i++)
  {
    fillColumn((byte*) full, i, 1);

    matrix.renderBitmap(full, LED_MAT_H, LED_MAT_W);

    delay(100);

    matrix.clear();
    memset(full, 0, sizeof(full));
  }
}

void goBackwards()
{
  for(int i=LED_MAT_W-1; i>=0; i--)
  {
    fillColumn((byte*) full, i, 1);

    matrix.renderBitmap(full, LED_MAT_H, LED_MAT_W);

    delay(100);

    matrix.clear();
    memset(full, 0, sizeof(full));
  }
}