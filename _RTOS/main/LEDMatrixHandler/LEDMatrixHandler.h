//
// Created by Tommaso F. Costantini on 18/03/26.
//

#ifndef REPO_LEDMATRIXHANDLER_H
#define REPO_LEDMATRIXHANDLER_H

#include <Arduino_LED_Matrix.h>

#define LED_MAT_H 8  //correct value = 8
#define LED_MAT_W 13  //correct value = 13

class LEDMatrixHandler
{
	public:
		virtual void goForward(ArduinoLEDMatrix* matrix) = 0;
		virtual void goBackwards(ArduinoLEDMatrix* matrix) = 0;

	protected:
		void fillColumn(byte* ledMat, int col, int val)
		{
  			//Applies 1 to each cell in the row, column fixed
  			for (int i=0; i<LED_MAT_H; i++)
  			{
    			ledMat[i*LED_MAT_W + col] = val;
  			}
		}
};

#endif //REPO_LEDMATRIXHANDLER_H