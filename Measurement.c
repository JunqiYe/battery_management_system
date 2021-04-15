#include "Measurement.h"

void updateHVIL(bool* hvilReading, const byte* pin) {
    /****************
    * Function name: updateHVIL
    * Function inputs: pin pointer, hvilReading pointer
    * Function outputs: hvilReading 
    * Function description: update hvil based on pin 23 reading 
    *****************/
    int val = digitalRead(*pin);
    if(val == 0) {
      *hvilReading = HVIL_OPEN;
    } else {
      *hvilReading = HVIL_CLOSED;
    }
    return;
}

void updateTemperature(float* temperatureReading, const byte* pin) {
    /****************
    * Function name: updateTemperature
    * Function inputs: temperatureReading pointer
    * Function outputs: temperatureReading
    * Function description: updates the temperature reading
    *****************/
    int input_V = analogRead(*pin);
    *temperatureReading = (float)input_V*(TEMP_UPPER - TEMP_LOWER)/(ANALOG_MAX)+TEMP_LOWER;

  	return;
}

void updateHvCurrent(float* currentReading, const byte* pin) {
    /****************
    * Function name: updateHvCurrent
    * Function inputs: currentReading pointer
    * Function outputs: currentReading
    * Function description: update the currentReading
    *****************/
    int input_V = analogRead(*pin);
    *currentReading = (float)input_V*(HVC_UPPER - HVC_LOWER)/(ANALOG_MAX)+HVC_LOWER;
    return;
}

void updateHvVoltage(float* voltageReading, const byte* pin) {
    /****************
    * Function name: updateHvVoltage
    * Function inputs: voltageReading pointer
    * Function outputs: voltageReading
    * Function description: update voltageReading
    *****************/
    int input_V = analogRead(*pin);
    *voltageReading = (float)input_V*(HVV_UPPER - HVV_LOWER)/(ANALOG_MAX)+HVV_LOWER;
  	return;
}


void updateHistory(float* temperature, float* hvCurrent, float* hvVoltage,
                    float* hvCurrent_Max, float* hvCurrent_Min, float* hvVoltage_Max,
                    float* hvVoltage_Min, float* temperature_Max, float* temperature_Min,
                    int* update_hist[3]) {
    /****************
    * Function name: updateHistory
    * Function inputs: temp, HvC, HVV, max and mins
    * Function outputs: update the flag
    * Function description: update update_hist flag if a new max or min is found
    *****************/
    if (*temperature > *temperature_Max) {
        *update_hist[0] = 1;
        *temperature_Max = *temperature;
    } else if (*temperature < *temperature_Min) {
        *update_hist[0] = -1; 
        *temperature_Min = *temperature;
    }
    
    if (*hvCurrent > *hvCurrent_Max) {
        *update_hist[1] = 1;
        *hvCurrent_Max = *hvCurrent;
    } else if (*hvCurrent < *hvCurrent_Min) {
        *update_hist[1] = -1;
        *hvCurrent_Min = *hvCurrent;  
    }

    //|| *hvVoltage_Max == -1.0f
    if (*hvVoltage > *hvVoltage_Max) {
        *update_hist[2] = 1;
        *hvVoltage_Max = *hvVoltage;  
    } else if (*hvVoltage < *hvVoltage_Min) { // add special case after reset
        *update_hist[2] = -1;
        *hvVoltage_Min = *hvVoltage; 
    }
    return;
}

void measurementTask(void* mData) {
    /****************
    * Function name: measurementTask 
    * Function inputs: mData pointer
    * Function outputs: none
    * Function description: calls all functions in measurementTask to update data 
    *****************/
  	measurementData* data = (measurementData*) mData;
    
  	// Update all sensors
  	updateHVIL(data->hvilStatus, data->hvilPin);
  	updateTemperature(data->temperature, data->tempPin);
  	updateHvCurrent(data->hvCurrent, data->HVCPin);
  	updateHvVoltage(data->hvVoltage, data->HVVPin);
    updateHistory(data->temperature, data->hvCurrent, data->hvVoltage,
                  data->hvCurrent_Max, data->hvCurrent_Min, data->hvVoltage_Max,
                  data->hvVoltage_Min, data->temperature_Max, data->temperature_Min,
                  data->update_hist);
  	
	return;
}
