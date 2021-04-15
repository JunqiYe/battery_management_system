#ifdef __cplusplus  
extern "C" { 
#endif 

#ifndef MEASUREMENT_H_
#define MEASUREMENT_H_


#define HVIL_OPEN   false
#define HVIL_CLOSED true
#define ANALOG_MAX 1023
#define ANALOG_MIN 0
#define TEMP_UPPER 45
#define TEMP_LOWER -10
#define HVV_UPPER 450
#define HVV_LOWER 0
#define HVC_UPPER 25
#define HVC_LOWER -25


#include <stdlib.h>
#include <stdbool.h>
#include <Arduino.h>


typedef struct measurementData {
    bool* hvilStatus;
    const byte* hvilPin;
    const byte* tempPin;
    const byte* HVCPin;
    const byte* HVVPin;
    float* temperature;
  	float* hvCurrent;
	  float* hvVoltage;
    float* hvCurrent_Max;
    float* hvCurrent_Min;
    float* hvVoltage_Max;
    float* hvVoltage_Min;
    float* temperature_Max;
    float* temperature_Min;
    // bool*  update_hist;
    // array of change max/min flags, 1 is change max, 0 is no-op, -1 is change min
    // update_hist[0] is temp, [1] is current, [2] is voltage
    int* update_hist[3]; 
    bool* resetEEPROM;
} measurementData;


void measurementTask (void*);


#endif

#ifdef __cplusplus 
} 
#endif 
