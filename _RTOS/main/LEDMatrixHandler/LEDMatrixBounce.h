//
// Created by Tommaso F. Costantini on 18/03/26.
//

#ifndef REPO_LEDMATRIXBOUNCE_H
#define REPO_LEDMATRIXBOUNCE_H

#include <LEDMatrixHandler.h>

class LEDMatrixBounce: public LEDMatrixHandler
{
	public:
		void goForward(ArduinoLEDMatrix* matrix);
		void goBackwards(ArduinoLEDMatrix* matrix);
};

#endif //REPO_LEDMATRIXBOUNCE_H