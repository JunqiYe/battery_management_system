#include "Accelerometer.h"
#include <math.h>

#define DE 0
#define Data 0

/****************
* Function name: volt_to_acce
* Function inputs: volt
* Function outputs: acceleration value
* Function description:  convert voltage to acceleration, unit: m/s^2
*****************/
float volt_to_acce(float volt) {
    float g_value = (volt - ZERO_G) / SLOPE;
    return g_value * g;
}

/****************
* Function name: update_acceleration
* Function inputs: x, y, z, xpin, y_pin, zpin
* Function outputs: none
* Function description: read from analog pin and process the voltage, store x, y, z acceleration value
*****************/
void update_acceleration(float* x_a, float* y_a, float* z_a, const byte* x_pin, 
                         const byte* y_pin, const byte* z_pin) {
    int x_read = analogRead(*x_pin);
    int y_read = analogRead(*y_pin);
    int z_read = analogRead(*z_pin);

    // raw voltagedw
    float volt_res = 3.3 / (ANALOG_MAX - ANALOG_MIN);
    float x_volt = x_read * volt_res;
    float y_volt = y_read * volt_res;
    float z_volt = z_read * volt_res;

    // acceleration res = 2g / 2^8 = 1/512 g/bit = 9.8/512 (m/s^2)/bit
    
    // calibration
    x_volt -= ACC_X_OFFSET;
    y_volt -= ACC_Y_OFFSET;
    z_volt += ACC_Z_OFFSET;

    if(abs(x_volt-1.65) < 0.08) x_volt = 1.65;
    if(abs(y_volt-1.65) < 0.08) y_volt = 1.65;
    if(abs(z_volt-1.65) < 0.08) z_volt = 1.65;
    if(abs(z_volt-0.85) < 0.08) z_volt = 0.85;

    
    // convert voltage to acceleration
    *x_a = volt_to_acce(x_volt) * 100;
    *y_a = volt_to_acce(y_volt) * 100;
    *z_a = volt_to_acce(z_volt) * 100;

    return;
}

/****************
* Function name: update_angle
* Function inputs: x, y, z, x_angle, y_angle, z_angle
* Function outputs: none
* Function description: using x, y, z acceleration vector to calculate the angles.
*****************/
// update angle with respect to each axis
void update_angle(float* x_a, float* y_a, float* z_a, 
                  float* x_angle, float* y_angle, float* z_angle) {
    double vector_length = sqrt(square(*x_a) + square(*y_a) + square(*z_a));

    // e.g. cos(x_angle) = x_positon / vector_length
    // find x_angle
    double x_angle_rad = acos((*x_a) / vector_length);
    double y_angle_rad = acos((*y_a) / vector_length);
    double z_angle_rad = acos((*z_a) / vector_length);

    *x_angle = (*z_a > 0) ? -rad_to_deg(x_angle_rad) : rad_to_deg(x_angle_rad);
    *y_angle = (*z_a > 0) ? -rad_to_deg(y_angle_rad) : rad_to_deg(y_angle_rad);
    *z_angle = rad_to_deg(z_angle_rad);
}



/****************
* Function name: update_FIFO
* Function inputs: x, y, z, x_sum, y_sum, z_sum, full, fifo_pointer
* Function outputs: none
* Function description: circular fifo to reduce noise
*****************/
float a_FIFO[3][FIFO_SIZE] = {0}; // initialize all elements to be 0;
void update_FIFO(float* x_a, float* y_a, float* z_a, float* x_a_sum, float* y_a_sum, float* z_a_sum, bool* full, int* a_FIFO_ptr) {
    // store acceleration in FIFO
    float removed_value[3];
    if((*full)) {
        for (int i = 0; i < 3; i++) { // register removed values
            removed_value[i] = a_FIFO[i][*a_FIFO_ptr];  
        }
    }

    float z_compensation = 980;    

    float x = *x_a;
    float y = *y_a;
    float z = z_compensation - *z_a;

    a_FIFO[0][*a_FIFO_ptr] = x;
    a_FIFO[1][*a_FIFO_ptr] = y;
    a_FIFO[2][*a_FIFO_ptr] = z;

    // update FIFO ptr
    int prev_ptr = *a_FIFO_ptr;
    //prev_full = full; // used to detect rising edge of full
    if (*a_FIFO_ptr == FIFO_SIZE - 1) {
        *a_FIFO_ptr = 0;
        *full = true; // only useful when populating the FIFO
    } else (*a_FIFO_ptr) += 1;
        *x_a_sum = 0;
        *y_a_sum = 0;
        *z_a_sum = 0;
        for (int i = 0; i < FIFO_SIZE; i++) {
            *x_a_sum += a_FIFO[0][i];
            *y_a_sum += a_FIFO[1][i];
            *z_a_sum += a_FIFO[2][i];
        }
}


/****************
* Function name: update_position
* Function inputs: x, y, z, x,y,z posisiton, x,y,z sum, x,y,z velocity
* Function outputs: none
* Function description: calculate velocity and position using fifo sum
*****************/
void update_position(float* x_position, float* y_position, float* z_position, float* distance, 
                      unsigned long* prev_time, float* x_a_sum, float* y_a_sum, float* z_a_sum, float* x_velocity, float* y_velocity, float* z_velocity) {

    float prev_v_x = *x_velocity;
    float prev_v_y = *y_velocity;
    float prev_v_z = *z_velocity;

    unsigned long new_time = millis();
    double delta_t_sec = (double)(new_time - *prev_time) / 1000;
    *prev_time = new_time;
    
    *x_velocity = (*x_velocity) + *x_a_sum/FIFO_SIZE * delta_t_sec;
    *y_velocity = (*y_velocity) + *y_a_sum/FIFO_SIZE * delta_t_sec;         
    *z_velocity = (*z_velocity) + *z_a_sum/FIFO_SIZE * delta_t_sec;

    if(*x_velocity == prev_v_x && *y_velocity == prev_v_y) {
        if(count == 10) {
            count = 0;
            *x_velocity = 0;
            *y_velocity = 0;
            *z_velocity = 0;
        }
        count++;
    } else {
        count = 0;
    }
    
    float x_delta = (prev_v_x + *x_velocity)/2 * delta_t_sec;
    float y_delta = (prev_v_y + *y_velocity)/2 * delta_t_sec;
    float z_delta = (prev_v_z + *z_velocity)/2 * delta_t_sec;
    *x_position += x_delta;
    *y_position += y_delta;
    *z_position += z_delta;
    *distance += abs(x_delta);
    *distance += abs(y_delta);
    *distance += abs(z_delta);
}

// helper method
// square the number
int count = 0;
double square(float value) {
    return value * value;
}

// helper method
// convert radian to degree
double rad_to_deg(double rad) {
     return rad * 180 / M_PI;
}

void accelerometerTask(void* acData) {
    accelerometerData* data = (accelerometerData*) acData;

    // update accleration readings
    update_acceleration(data->x_a, data->y_a, data->z_a, 
                       data->x_pin, data->y_pin, data->z_pin);
    update_angle(data->x_a, data->y_a, data->z_a, data->x_angle, data->y_angle, data->z_angle);
    update_FIFO(data->x_a, data->y_a, data->z_a, data->x_a_sum, data->y_a_sum, data->z_a_sum, data->full, data->a_FIFO_ptr);
    update_position(data->x_position, data->y_position, data->z_position, data->distance, data->prev_time, data->x_a_sum, data-> y_a_sum, data->z_a_sum, data->x_velocity, data->y_velocity, data->z_velocity);

}
