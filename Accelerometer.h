
#ifndef ACCELEROMETER_H_
#define ACCELEROMETER_H_

#include <stdlib.h>
#include <Arduino.h>
#include <stdbool.h>

#define ANALOG_MAX 1023
#define ANALOG_MIN 0
#define M_PI 3.14

// calibration offset
#define ACC_X_OFFSET 0.07;
#define ACC_Y_OFFSET 0.24;
#define ACC_Z_OFFSET 0.26;

// used to convert voltage to acceleration
#define ZERO_G 1.65
#define ONE_G 2.45
#define SLOPE (1.65 - 2.45)
#define g 9.8

// FIFO to store 10 acceleration samples
#define FIFO_SIZE 10

typedef struct accelerometerData {
    const byte* x_pin;
    const byte* y_pin;
    const byte* z_pin;
    float* x_a;
    float* y_a;
    float* z_a;
    float* x_position;
    float* y_position;
    float* z_position;
    float* distance;
    float* x_angle;
    float* y_angle;
    float* z_angle;
    unsigned long* prev_time;
    float* x_a_sum;
    float* y_a_sum;
    float* z_a_sum;
    bool* full;
    int* a_FIFO_ptr;
    float* x_velocity;
    float* y_velocity;
    float* z_velocity;
} accelerometerData;


void accelerometerTask (void*);
double rad_to_deg(double rad);
void print_a_x_array();
double square(float value);
#endif 
