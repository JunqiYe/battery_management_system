#ifdef __cplusplus  
extern "C" { 
#endif 

#ifndef ALARM_H_
#define ALARM_H_

#include <stdlib.h>
#include <Arduino.h>
#include "Measurement.h"

#define NOT_ACT 0
#define ACTIVE_NOT_ACK 1
#define ACTIVE_ACK  2

typedef struct alarmTaskData {
	volatile int* HVIA_alarm_state;
	int* overcurrent_state;
	int* HVOR_state;
  measurementData* measure;
  bool* activeAlarm;
  bool* acknowledge_flag;
} alarmTaskData;


void alarmTask (void*);


#endif

#ifdef __cplusplus 
} 
#endif 
