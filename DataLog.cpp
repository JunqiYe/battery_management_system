#include "DataLog.h"
void EEPROM_writeFloat(int addr, float value);

void DataLogTask (void* mData) {
  /****************
      Function name: DataLogTask
      Function inputs: mData pointer
      Function outputs: none
      Function description: update the EEPROM using the update flag
    *****************/
    measurementData* data = (measurementData*) mData;
    int* updateTempFlag = (data -> update_hist)[0];
    int* updateCurrFlag = (data -> update_hist)[1];
    int* updateVoltFlag = (data -> update_hist)[2];
    bool* reset = data->resetEEPROM;

    // history addresses in EEPROM
    int tempMaxAddr = 0;
    int tempMinAddr = tempMaxAddr + sizeof(float);
    int currMaxAddr = tempMinAddr + sizeof(float);
    int currMinAddr = currMaxAddr + sizeof(float);
    int voltMaxAddr = currMinAddr + sizeof(float);
    int voltMinAddr = voltMaxAddr + sizeof(float);

    if (*reset) {
        *reset = false;
        *(data->temperature_Max) = -1000;
        *(data->temperature_Min) = 1000;
        *(data->hvCurrent_Max) = -1000;
        *(data->hvCurrent_Min) = 1000;
        *(data->hvVoltage_Max) = -1000;
        *(data->hvVoltage_Min) = 1000;
        /*EEPROM.put(tempMaxAddr, 0.0f);
        EEPROM.put(tempMinAddr, 0.0f);
        EEPROM.put(currMaxAddr, 0.0f);
        EEPROM.put(currMinAddr, 0.0f);
        EEPROM.put(voltMaxAddr, -1.0f);
        EEPROM.put(voltMinAddr, -1.0f);*/
        EEPROM_writeFloat(tempMaxAddr, 0.0f);
        EEPROM_writeFloat(tempMinAddr, 0.0f);
        EEPROM_writeFloat(currMaxAddr, 0.0f);
        EEPROM_writeFloat(currMinAddr, 0.0f);
        EEPROM_writeFloat(voltMaxAddr, -1.0f);
        EEPROM_writeFloat(voltMinAddr, -1.0f);
    } else {
        // update temp history
        if (*updateTempFlag == 1) {
            *updateTempFlag = 0;
            EEPROM_writeFloat(tempMaxAddr, *(data->temperature_Max));
        } 
        if (*updateTempFlag == -1) {
            *updateTempFlag = 0;  
            EEPROM_writeFloat(tempMinAddr, *(data->temperature_Min));
        } 
    
        // update current history
        if (*updateCurrFlag == 1) {
            *updateCurrFlag = 0;
            EEPROM_writeFloat(currMaxAddr, *(data->hvCurrent_Max));  
        } 
        if (*updateCurrFlag == -1) {
            *updateCurrFlag = 0;
            EEPROM_writeFloat(currMinAddr, *(data->hvCurrent_Min));  
        }
    
        // update voltage history
        if (*updateVoltFlag == 1) {
            *updateVoltFlag = 0;
            EEPROM_writeFloat(voltMaxAddr, *(data->hvVoltage_Max));  
        } 
        if (*updateVoltFlag == -1) {
            *updateVoltFlag = 0;
            EEPROM_writeFloat(voltMinAddr, *(data->hvVoltage_Min));  
        }
    }

}

// write to EEPROM
// Atomic
void EEPROM_writeFloat(int addr, float value) {
    noInterrupts();
    byte* p = (byte*)(void*)&value;
    for (int i = 0; i < sizeof(value); i++)
        EEPROM.write(addr++, *p++);
    interrupts();
}
