#include "VadersWrapper.h"

VadersWrapper vw;

void setup()
{
    Serial.begin(9600);
#ifdef FW_DEBUG
    while (!Serial);

    Serial.println("setup");
#endif
    vw.begin();
}

void loop()
{
#ifdef FW_DEBUG_VERBOSE
    Serial.println("loop");
#endif
    vw.update();

}
