#ifndef DISPLAY_H_
#define DISPLAY_H_


#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define MINPRESSURE 10
#define MAXPRESSURE 1000

//Touch For New ILI9341 TP
#define TS_MINX 120
#define TS_MAXX 900

#define TS_MINY 70
#define TS_MAXY 920

#define DISMAXX 240 // display max x value
#define DISMAXY 320 // display max y value
#define ButtonW 75  // button width
#define ButtonH 50  // button height
#define ButtonR 8   // button corner radius

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

#define TITLE_OFFSET 35
#define WORD_OFFSET 60

#include <stdlib.h>
#include <stdbool.h>
#include <Arduino.h>
#include <Elegoo_TFTLCD.h>

#include "Measurement.h"
#include "Soc.h"
#include "Contact.h"
#include "Alarm.h"
#include "TaskControlBlock.h"
#include "Accelerometer.h"

typedef struct displayHistory {
    int* prev_dis;
    float* prev_temp;
    float* prev_HVC;
    float* prev_HVV;
    int* prev_HVIL;
    float* prev_soc;
    int* prev_hvia;
    int* prev_overC;
    int* prev_hvor;
    int* prev_contactor;
    bool* prev_prev;
    bool* prev_next;
    bool* button_not_drawn;
    bool* ack_button_drawn;
    
    // accelerometer history
    float* prev_x_position;
    float* prev_y_position;
    float* prev_z_position;
    float* prev_distance;
    float* prev_x_angle;
    float* prev_y_angle;
    float* prev_z_angle;
} displayHistory;

typedef struct displayTaskData {
    Elegoo_TFTLCD* tft;
    socTaskData* socData;
    measurementData* measure;
    contactTaskData* contactData;
    alarmTaskData* alarmData;
    int* prev_pressed; // determine if prev button is pressed
    int* next_pressed; // determine if next button is pressed
    int* curr_display;// determine which screen
    displayHistory* history;
    accelerometerData* acData;
} displayTaskData;



void displayTask(void*);
void displayAckButton(Elegoo_TFTLCD tft, void* AState, void* hData);
const char* alarmAcknowledgment(int);

#endif
