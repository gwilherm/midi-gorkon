#include "SerialPrinter.h"

#include <iostream>
#include <iomanip>

size_t SerialPrinter::print(int v, base_t base)
{
    std::cout << std::setbase(base) << std::uppercase << v;
    return 0;
}

size_t SerialPrinter::print(const char* v)
{
    std::cout << v;
    return 0;
}

size_t SerialPrinter::println(void)
{
    std::cout << std::endl;
    return 0;
}

size_t SerialPrinter::println(int v)
{
    std::cout << v << std::endl;
    return 0;
}

size_t SerialPrinter::println(const char* v)
{
    std::cout << v << std::endl;
    return 0;
}