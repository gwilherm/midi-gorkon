#ifndef HARDWARE_CONFIG_H
#define HARDWARE_CONFIG_H

#include <Arduino.h>

// Number of encoders (XY = 2 encoders)
#define ENC_NB 8
// Number of buttons
#define BTN_NB 1

// Encoders pins
const uint8_t ENC_PINS[ENC_NB] = { A10, A9, A8, A7,
                                    A0, A1, A2, A3 };

// Buttons pins
const uint8_t BTN_PINS[BTN_NB] = { 5 };

// Piano RGB pin
#define PIANO_RGB_PIN 4

// Piano RGB LED amount
#define PIANO_LED_COUNT 4

#endif // HARDWARE_CONFIG_H