#ifndef ARDUINO_MOCK_H
#define ARDUINO_MOCK_H

#include <gmock/gmock.h>

class ArduinoMock {
  public:
    ArduinoMock() = default;
    virtual ~ArduinoMock() = default;

    MOCK_METHOD(void, pinMode, (uint8_t, uint8_t));

    MOCK_METHOD(int, analogRead, (uint8_t));

    MOCK_METHOD(int, digitalRead, (uint8_t));

    MOCK_METHOD(unsigned long, millis, ());

};

#endif