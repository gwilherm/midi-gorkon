#ifndef EEPROM_INTERFACE_MOCK_H
#define EEPROM_INTERFACE_MOCK_H

#include "IEEPROMInterface.h"

#include <gmock/gmock.h>

class EEPROMInterfaceMock : public IEEPROMInterface {
  public:
    EEPROMInterfaceMock() = default;
    virtual ~EEPROMInterfaceMock() = default;

    MOCK_METHOD(uint8_t, read, (int));

    MOCK_METHOD(void, update, (int, uint8_t));
};

#endif