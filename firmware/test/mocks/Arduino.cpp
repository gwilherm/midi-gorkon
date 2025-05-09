#include "Arduino.h"
#include "ArduinoMock.h"
#include "ArduinoTestFixture.h"

void pinMode(uint8_t pin, uint8_t mode)
{
    ArduinoTestFixture::_arduinoMock->pinMode(pin, mode);
}

int analogRead(uint8_t pin)
{
    return ArduinoTestFixture::_arduinoMock->analogRead(pin);
}

int digitalRead(uint8_t pin)
{
    return ArduinoTestFixture::_arduinoMock->digitalRead(pin);
}

unsigned long millis(void)
{
    return ArduinoTestFixture::_arduinoMock->millis();
}
