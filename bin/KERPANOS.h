#ifndef KERPANOS_H
#define KERPANOS_H

#include "sheaders.h"

void NORETURN kpanic(uint32_t error_code, const char* description);

#endif
