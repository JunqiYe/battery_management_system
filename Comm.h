#ifndef COMM_H_
#define COMM_H_


#include <stdlib.h>
#include <Arduino.h>
#include <stdbool.h>
#include "Measurement.h"
#include <EEPROM.h>

void communicationTask(void* ptr);
int asciiToNum(int ascii);
float EEPROM_readFloat(int addr);

#endif
