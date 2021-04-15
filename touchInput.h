#ifndef TOUCH_H_
#define TOUCH_H_

#define MINX 120
#define MAXX 900

#define MINY 70
#define MAXY 920


#define MINPRESSURE 10
#define MAXPRESSURE 1000

#define BATTERYSCDELAY 250

#include <stdlib.h>
#include <Arduino.h>
#include <TouchScreen.h>
#include "Display.h"
#include <stdbool.h>

typedef struct touchInputData {
	TouchScreen* ts;
	int* prev;
	int* next;
  bool* contactor_flag;
  bool* acknowledge_flag;
  int* curr_display;

} touchInputData;


void tsInputTask (void*);


#endif
