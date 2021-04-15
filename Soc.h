#ifndef SOC_H_
#define SOC_H_

#include <stdlib.h>
#include <Arduino.h>
#include "Measurement.h"

#define RBATT (float)0.5
#define LUT_TEMP 4
#define LUT_VOC 5
#define LUT_VOC_LOWER 200
#define LUT_VOC_INTERVAL 50

typedef struct socTaskData {
    float* stateofCharge;
    int lut[4][5];
    int temp_arr[4];
    int volt_arr[5];
    measurementData* mData;
} socTaskData;

void socTask(void* sData);

#endif
