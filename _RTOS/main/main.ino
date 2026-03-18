#include "MsgPack.h"
#include "Arduino_LED_Matrix.h"
#include "Arduino_RouterBridge.h"

#define LED_MAT_H 8  //correct value = 8
#define LED_MAT_W 14  //correct value = 13

ArduinoLEDMatrix matrix;
int col = 0;
byte full[LED_MAT_H][LED_MAT_W];

void setup() {
  Monitor.begin();
  Serial.begin(9600);
  matrix.begin();
}

void loop() {
  for(int i=0; i<LED_MAT_W; i++) {
    fillColumn((byte*) full, i, 1);

    matrix.renderBitmap(full, LED_MAT_H, LED_MAT_W);

    delay(100);

    matrix.clear();
    memset(full, 0, sizeof(full));
  }

  Serial.println("Restarting from Serial");
  Monitor.println("Restarting from Monitor");
}

void fillColumn(byte* ledMat, int col, int val) {

  //Applies 1 to each cell in the row, column fixed
  for (int i=0; i<LED_MAT_H; i++) {
    ledMat[i*LED_MAT_W + col] = val;
  }

}