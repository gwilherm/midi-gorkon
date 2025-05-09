#ifndef SERIAL_PRINTER_H
#define SERIAL_PRINTER_H

#include <cstddef>

typedef enum {
  DEC = 10,
  HEX = 16
} base_t;

class SerialPrinter {
  public:
    SerialPrinter() = default;
    virtual ~SerialPrinter() = default;

    size_t print(int, base_t base=DEC);
    size_t print(const char*);
    size_t println(void);
    size_t println(int);
    size_t println(const char*);

};

extern SerialPrinter Serial;

#endif