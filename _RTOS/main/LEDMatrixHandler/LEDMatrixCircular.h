//
// Created by Tommaso F. Costantini on 18/03/26.
//

#ifndef REPO_LEDMATRIXCIRCULAR_H
#define REPO_LEDMATRIXCIRCULAR_H

#include <LEDMatrixHandler.h>

class LEDMatrixCircular: public LEDMatrixHandler
{
	public:
		void goForward(ArduinoLEDMatrix* matrix);
		void goBackwards(ArduinoLEDMatrix* matrix);
};

#endif //REPO_LEDMATRIXCIRCULAR_H