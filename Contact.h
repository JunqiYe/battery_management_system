#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONTACT_H_
#define CONTACT_H_

#include <stdlib.h>
#include <stdbool.h>
#include <Arduino.h>
#include "Alarm.h"

#define CLOSE true
#define OPEN false

typedef struct contactTaskData {
    volatile bool* contact_on_off;
    const byte* contactPin;
    bool* contactor_flag;
    alarmTaskData* aData;
} contactTaskData;

void contactTask(void*);

#endif

#ifdef __cplusplus
}
#endif
