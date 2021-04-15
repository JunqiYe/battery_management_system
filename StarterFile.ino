#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "TimerOne.h"

#include "Measurement.h"
#include "Soc.h"
#include "Contact.h"
#include "Alarm.h"
#include "TaskControlBlock.h"
#include "Display.h"
#include "touchInput.h"
#include "DataLog.h"
#include "Comm.h"
#include "Accelerometer.h"

#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

#define DEBUG 0
#define PERIOD 0
#define DEBUG_EEPROM 0
#define DEBUG_ACCE 0

// tft driver
Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

//function declaration
void insertTask(TCB* node);
void deleteTask(TCB* node);
void scheduler();


//Task Control Blocks
TCB measurementTCB;         // Declare measurement TCB
TCB socTCB;                 // Declare SOC TCB
TCB contactTCB;             // Declare Contact TCB
TCB alarmTCB;               // Declare Alarm TCB
TCB displayTCB;
TCB touchInputTCB;
TCB DataLogTCB;
TCB CommTCB;
TCB accelerometerTCB;


TCB* head = NULL;
TCB* taskPtr = NULL;
TCB* tail = NULL;


// Measurement Data
measurementData measure;    // Declare measurement data structure - defined in Measurement.h
float hvCurrent     = 0;
float hvVoltage     = 0;
float temperature   = 0;
bool HVIL           = 0;
float hvCurrent_Max     = -1000;
float hvCurrent_Min     = 1000;
float hvVoltage_Max     = -1000;
float hvVoltage_Min     = 1000;
float temperature_Max   = -1000;
float temperature_Min   = 1000;
const byte hvilPin  = 21;
const byte tempPin  = A15;
const byte HVCPin   = A14;
const byte HVVPin   = A13;
int updateTempHistFlag = 0;
int updateCurrHistFlag = 0;
int updateVoltHistFlag = 0;
bool resetEEPROM = false;


// SOC Data
socTaskData socData;
float stateofCharge = 0;
int lut[4][5] = {{0, 10, 35, 100, 100},
                 {0, 0,  20, 80,  100},
                 {0, 0,  10, 60,  100},
                 {0, 0,   0, 50,  100}};
int temp_arr[4] = {-10, 0, 25, 45};
int volt_arr[5] = {200, 250, 300, 350, 400};

// contactor task
contactTaskData contactData;
volatile bool contactor      = 0;
const byte contactPin  = 23;

// alarm
alarmTaskData alarmData;
volatile int HVIL_alarm_state= 0;
int overcurrent_state= 0;
int HVOR_state      = 0;
bool activeAlarm = false;

//display
displayTaskData displayData;

//display_hist
displayHistory displayHist_Data;
int prev_dis = 100;
float prev_temp = 0;
float prev_HVC = 0;
float prev_HVV = 0;
int prev_HVIL = 0;
float prev_soc = 0;
int prev_hvia = 0;
int prev_overC = 0;
int prev_hvor = 0;
int prev_contactor = 0;
bool prev_prev = false;
bool prev_next = false;
bool button_not_drawn = true;
bool ack_button_drawn = false;
float prev_x_position = 0;
float prev_y_position = 0;
float prev_z_position = 0;
float prev_distance = 0;
float prev_x_angle = 0;
float prev_y_angle = 0;
float prev_z_angle = 0;

//touchscreen
touchInputData tiData;
int prev_pressed = 0;
int next_pressed = 0; 
bool contactor_flag = 0;
bool acknowledge_flag = 0;

// accelerometer
accelerometerData acData;
const byte x_pin = A10;
const byte y_pin = A11;
const byte z_pin = A12;
const byte sleep_pin = 22;
float x_a;
float y_a;
float z_a;
float x_position = 0;
float y_position = 0;
float z_position = 0;
float distance = 0;
float x_angle = 0;
float y_angle = 0;
float z_angle = 0;
unsigned long prev_time = 0.0;
float x_a_sum = 0;
float y_a_sum = 0;
float z_a_sum = 0;
bool full = false;
int a_FIFO_ptr = 0;
// .... other shared global data ....
int curr_display = 2;
float x_velocity = 0.0;
float y_velocity = 0.0;
float z_velocity = 0.0;

// timebaseflag
volatile int timeBaseFlag;
unsigned long timeIntv;

// global counter
int counter = 0;

void loop() {
  /****************
    * Function name:    loop
    * Function inputs:  Sensor data, touch input
    * Function outputs: Display data and lights indicating alarm status, contactor status, sensor data, & state of charge
    * Function description: This is a round robin scheduler to run a series of tasks
    *                       that create a user interface to a battery management system
    * Author(s): Yijie, Junqi
    *****************/
    while(1){
        
        if (1 == timeBaseFlag) {
            if (PERIOD) timeIntv = millis();
            timeBaseFlag = 0;
            
            counter += 1;

            if (counter % 2 == 0) {
              insertTask(&accelerometerTCB);
              if (DEBUG_ACCE) debugACCE();  
            }
            
            if (counter % 100 == 0) {
                insertTask(&measurementTCB);
                insertTask(&socTCB);
                insertTask(&contactTCB);
                insertTask(&alarmTCB); 
            }
    

            if (counter % 1000 == 0) {
                insertTask(&touchInputTCB);
                insertTask(&displayTCB);
                insertTask(&CommTCB);
            }
          
            if (counter == 5000) {
                insertTask(&DataLogTCB);
                if (DEBUG_EEPROM) {
                    debugEEPROM();
                }
            }

            
            scheduler();
            if (counter % 2 == 0) {
                deleteTask(&accelerometerTCB);  
            }

            if (counter % 100 == 0) {
                deleteTask(&measurementTCB);
                deleteTask(&socTCB);
                deleteTask(&contactTCB);
                deleteTask(&alarmTCB); 
            }
            
            if (counter % 1000 == 0) {
                deleteTask(&touchInputTCB);
                deleteTask(&displayTCB);
                deleteTask(&CommTCB);
            }

            if (counter == 5000) {
                counter = 0;
                deleteTask(&DataLogTCB);
            }
            

            
            if (PERIOD) {
            unsigned long endtime = millis();
            Serial.println(endtime - timeIntv);
            }
        }
        
//        Serial.println(hvCurrent_Max);
//        Serial.println(hvCurrent_Min);
//        Serial.println("clr");
        if (DEBUG) {
            debugMessage();
        }

        
    }
}

void insertTask(TCB* node) {
    node -> next = NULL; // reset to NULL b/c we didnt reset when deleting
    node -> prev = NULL;
    if (NULL == head) { // If the head pointer is pointing to nothing
        head = node; // set the head and tail pointers to point to this node
        tail = node;
    } else { // otherwise, head is not NULL, add the node to the end of the list
        tail -> next = node;
        node -> prev = tail; // note that the tail pointer is still pointing
        // to the prior last node at this point
        tail = node; // update the tail pointer
        tail -> next = NULL;
    }
    return;
}

void deleteTask(TCB* node){
    if (NULL != head) {
        if (head == tail) { // TODO check to make sure only node is deleted
            head = NULL;
            tail = NULL;
        } else if (head == node) {
            head = head -> next;
            head -> prev = NULL;
        } else if (tail == node) {
            tail = tail -> prev;
            tail -> next = NULL;
        } else {
            //middle
            //TCB* temp = head;
            //while(head != node) 
            (node -> next) -> prev = node -> prev;
            (node -> prev) -> next = node -> next;
        }
    }
    return;
}


void scheduler() {

    while (taskPtr) {
        taskPtr->task(taskPtr->taskDataPtr);
        taskPtr = taskPtr->next;
    }
    
    taskPtr = head;
}


void printListLength(){
    TCB* tmp = head;
    int c = 0;
    while (tmp) {
        c++;
        tmp = tmp->next;
    }
    Serial.print("Task list length = ");
    Serial.println(c);
}

// message for debugging
void debugMessage(){
    Serial.println("---------------------");
    Serial.println("MEASUREMENT TASK");
    Serial.print("hvCurrent = ");
    Serial.print(hvCurrent);
    Serial.print(", hvVoltage = ");
    Serial.print(hvVoltage);
    Serial.print(", HVIL = ");
    Serial.print(HVIL);
    Serial.print(", temperature = ");
    Serial.print(temperature);
    Serial.print(", soc = ");
    Serial.println(stateofCharge);
    Serial.println();
    
    Serial.println("ALARM TASK");
    Serial.print("HVIL_alarm_state = "); 
    Serial.print(HVIL_alarm_state);
    Serial.print(", OC_state = ");
    Serial.print(overcurrent_state);
    Serial.print(", HVOR_state = ");
    Serial.println(HVOR_state);
    Serial.println();

    Serial.println("Touch Input Task");
    Serial.print("Prev_pressed = ");
    Serial.print(prev_pressed);
    Serial.print(", Next_pressed = ");
    Serial.println(next_pressed);
    Serial.println();

    Serial.println("CONTACTOR TASK");
    Serial.print("curr_display = ");
    Serial.print(curr_display);
    Serial.print(", contactor_on_off = ");
    Serial.println(contactor);
    Serial.print("acknowledge flag = ");
    Serial.println(acknowledge_flag);
    Serial.println("---------------------");
}

// message for debugging EEPROM
void debugEEPROM() {
    Serial.println("----------------");
    Serial.print("resetEEPROM: ");
    Serial.println(resetEEPROM ? "True" : "False");
    Serial.print("updateTempHistFlag: ");
    Serial.println(updateTempHistFlag);
    Serial.print("updateCurrHistFlag: ");
    Serial.println(updateCurrHistFlag);
    Serial.print("updateVoltHistFlag: ");
    Serial.println(updateVoltHistFlag);
    Serial.println("----------------");
    /*Serial.print("Voltage Max: ");
    Serial.println(hvVoltage_Max);
    Serial.print("Voltage Min: ");
    Serial.println(hvVoltage_Min);*/
}

// debug message for accelerometer
void debugACCE() {
  Serial.println("----------------");
  Serial.print("x_acce = ");
  Serial.println(x_a);
  Serial.print("y_acce = ");
  Serial.println(y_a);
  Serial.print("z_acce = ");
  Serial.println(z_a);
  Serial.println("----------------");
}

void timerISR() {
    timeBaseFlag = 1;
}


void HVIL_ISR() {
    HVIL_alarm_state = 1;
    contactor = false;
}

void setup() {  
  /****************
    * Function name: setup
    * Function inputs: None
    * Function outputs: None
    * Function description: sets up the scheduler, touchscreen, and display.
    * Author(s): Yijie, Junqi
    *****************/
    // setup pin
    pinMode(hvilPin, INPUT);
    pinMode(contactPin, OUTPUT);
    pinMode(tempPin, INPUT_PULLUP);
    pinMode(HVCPin, INPUT_PULLUP);
    pinMode(HVVPin, INPUT_PULLUP);

    // setup accelerometer pin
    pinMode(x_pin, INPUT);
    pinMode(y_pin, INPUT);
    pinMode(z_pin, INPUT);
    pinMode(sleep_pin, OUTPUT);
    digitalWrite(sleep_pin, HIGH);

    // setup interrupt
    Timer1.initialize(1000);
    Timer1.attachInterrupt(timerISR);

    //TODO EEPROM

    attachInterrupt(digitalPinToInterrupt(hvilPin), HVIL_ISR, RISING);
    //Timer1.start();

    // Initialize Touch Input
    tiData                      = {&ts, &prev_pressed, &next_pressed, &contactor_flag, &acknowledge_flag, &curr_display};
    touchInputTCB.task          = &tsInputTask;
    touchInputTCB.taskDataPtr   = &tiData;
    touchInputTCB.next          = NULL;
    touchInputTCB.prev          = NULL;
       
    // Initialize Measurement & Sensors
    measure                     = {&HVIL, &hvilPin, &tempPin, &HVCPin, &HVVPin, &temperature, &hvCurrent, &hvVoltage, &hvCurrent_Max,
                                   &hvCurrent_Min, &hvVoltage_Max, &hvVoltage_Min, &temperature_Max, &temperature_Min, {&updateTempHistFlag, &updateCurrHistFlag, &updateVoltHistFlag}, &resetEEPROM};
    measurementTCB.task         = &measurementTask;
    measurementTCB.taskDataPtr  = &measure;
    measurementTCB.next         = NULL;
    measurementTCB.prev         = NULL;

    // Initialize SOC
    socData                     = {&stateofCharge, 
                                  {{0, 10, 35, 100, 100},
                                  {0, 0,  20, 80,  100},
                                  {0, 0,  10, 60,  100},
                                  {0, 0,   0, 50,  100}},
                                  {-10, 0, 25, 45}, 
                                  {200, 250, 300, 350, 400}, 
                                  &measure}; // why does this work but not the other
    socTCB.task                 = &socTask;
    socTCB.taskDataPtr          = &socData;
    socTCB.next                 = NULL;
    socTCB.prev                 = NULL;

    // Initialize Contactor
    contactData                 = {&contactor, &contactPin, &contactor_flag, &alarmData};
    contactTCB.task             = &contactTask;
    contactTCB.taskDataPtr      = &contactData;
    contactTCB.next             = NULL;
    contactTCB.prev             = NULL;
    
    // Initialize Alarm
    alarmData                   = {&HVIL_alarm_state, &overcurrent_state, &HVOR_state, &measure, &activeAlarm, &acknowledge_flag};
    alarmTCB.task               = &alarmTask;
    alarmTCB.taskDataPtr        = &alarmData;
    alarmTCB.next               = NULL;
    alarmTCB.prev               = NULL;
    
    // Initialize display
    displayHist_Data            = {&prev_dis, &prev_temp, &prev_HVC, &prev_HVV, &prev_HVIL, &prev_soc, &prev_hvia, &prev_overC, 
                                   &prev_hvor, &prev_contactor, &prev_prev, &prev_next, &button_not_drawn, &ack_button_drawn, 
                                   &prev_x_position, &prev_y_position, &prev_z_position, &prev_distance, &prev_x_angle, &prev_y_angle, &prev_z_angle};
    displayData                 = {&tft, &socData, &measure, &contactData, &alarmData, &prev_pressed, &next_pressed, &curr_display, &displayHist_Data, &acData};
    displayTCB.task             = &displayTask;
    displayTCB.taskDataPtr      = &displayData;
    displayTCB.next             = NULL;
    displayTCB.prev             = NULL;

    // Initialize Datalog
    DataLogTCB.task             = &DataLogTask;
    DataLogTCB.taskDataPtr      = &measure;
    DataLogTCB.next             = NULL;
    DataLogTCB.prev             = NULL;

    // Initialize communication
    CommTCB.task               = &communicationTask;
    CommTCB.taskDataPtr        = &measure;
    CommTCB.next               = NULL;
    CommTCB.prev               = NULL;

    // Initialize accelerometer
    acData                     = {&x_pin, &y_pin, &z_pin, &x_a, &y_a, &z_a, &x_position, &y_position, &z_position, &distance, 
                                  &x_angle, &y_angle, &z_angle, &prev_time, &x_a_sum, &y_a_sum, &z_a_sum, &full, &a_FIFO_ptr, &x_velocity, &y_velocity, &z_velocity};
    accelerometerTCB.task      = &accelerometerTask;
    accelerometerTCB.taskDataPtr = &acData;
    accelerometerTCB.next      = NULL;
    accelerometerTCB.prev      = NULL;
    
    // ..... Initialize other tasks and task data ..... 
    // Initialize Display
    tft.reset();
  
    uint16_t identifier = tft.readID();
    if(identifier == 0x9325) {
        Serial.println(F("Found ILI9325 LCD driver"));
    } else if(identifier == 0x9328) {
        Serial.println(F("Found ILI9328 LCD driver"));
    } else if(identifier == 0x4535) {
        Serial.println(F("Found LGDP4535 LCD driver"));
    }else if(identifier == 0x7575) {
        Serial.println(F("Found HX8347G LCD driver"));
    } else if(identifier == 0x9341) {
        Serial.println(F("Found ILI9341 LCD driver"));
    } else if(identifier == 0x8357) {
        Serial.println(F("Found HX8357D LCD driver"));
    } else if(identifier==0x0101)
    {     
        identifier=0x9341;
        Serial.println(F("Found 0x9341 LCD driver"));
    }else {
        Serial.print(F("Unknown LCD driver chip: "));
        Serial.println(identifier, HEX);
        Serial.println(F("If using the Elegoo 2.8\" TFT Arduino shield, the line:"));
        Serial.println(F("  #define USE_Elegoo_SHIELD_PINOUT"));
        Serial.println(F("should appear in the library header (Elegoo_TFT.h)."));
        Serial.println(F("If using the breakout board, it should NOT be #defined!"));
        Serial.println(F("Also if using the breakout, float-check that all wiring"));
        Serial.println(F("matches the tutorial."));
        identifier=0x9341;
     
    }
  
    tft.begin(identifier);

    
    // Initialize serial communication
    Serial.begin(9600);
    Serial1.begin(19200);

    //input instruction
    Serial.println("[1] Reset EEPROM");
    Serial.println("[2] HV Current Range [Hi, Lo]");
    Serial.println("[3] HV Voltage Range [Hi, Lo]");
    Serial.println("[4] Temperature Range [Hi, Lo]");
    Serial.println();
    Serial.println("Enter your menu choice [1-4]: <char>"); 
    Serial.println();
}
