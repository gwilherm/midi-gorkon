#ifndef CHRONO_H
#define CHRONO_H

#include <Arduino.h>

class Chrono
{
public:
    Chrono(uint64_t intervalMs):
        _intervalMs(intervalMs)
    {};

    void lap() { _lastMs = millis(); };
    bool isElapsed() { return (millis() >= (_lastMs + _intervalMs)); };

protected:
    uint32_t _intervalMs;
    uint64_t _lastMs;

};

#endif // CHRONO_H