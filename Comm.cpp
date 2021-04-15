#include "Comm.h"
float EEPROM_readFloat(int addr);

void communicationTask(void* mData) {
  /****************
      Function name: communication
      Function inputs: hData pointer
      Function outputs: none
      Function description: perform the contactor task to update contact_on_pff
    *****************/
    measurementData* data = (measurementData*) mData;

    // history addresses in EEPROM
    int tempMaxAddr = 0;
    int tempMinAddr = tempMaxAddr + sizeof(float);
    int currMaxAddr = tempMinAddr + sizeof(float);
    int currMinAddr = currMaxAddr + sizeof(float);
    int voltMaxAddr = currMinAddr + sizeof(float);
    int voltMinAddr = voltMaxAddr + sizeof(float);

    if (Serial.available() > 0) {
        int input = Serial.read();
        if (input != 10 && input != 32) {

            switch(asciiToNum(input)) {  //result case statement
                case -1:
                    Serial.print("invalid input: [");
                    Serial.print((char)input);
                    Serial.println("]");
                    break;
                case 1:
                    *(data->resetEEPROM) = true;
                    Serial.println("reset EEPROM");
                    break;
                case 2:
                    // retrieve data from EEPROM
                    float hvCurrent_Max, hvCurrent_Min;
                    /*EEPROM.get(currMaxAddr, hvCurrent_Max);
                    EEPROM.get(currMinAddr, hvCurrent_Min);*/
                    hvCurrent_Max = EEPROM_readFloat(currMaxAddr);
                    hvCurrent_Min = EEPROM_readFloat(currMinAddr);

                    // print out data
                    Serial.print("HV Current Range [");
                    Serial.print(hvCurrent_Max);
                    Serial.print(", ");
                    Serial.print(hvCurrent_Min);
                    Serial.println("]");
                    break;
                case 3:
                    // retrieve data from EEPROM
                    float hvVoltage_Max, hvVoltage_Min;
                    /*EEPROM.get(voltMaxAddr, hvVoltage_Max);
                    EEPROM.get(voltMinAddr, hvVoltage_Min);*/
                    hvVoltage_Max = EEPROM_readFloat(voltMaxAddr);
                    hvVoltage_Min = EEPROM_readFloat(voltMinAddr);

                    // print out data
                    Serial.print("HV Voltage Range [");
                    Serial.print(hvVoltage_Max);
                    Serial.print(", ");
                    Serial.print(hvVoltage_Min);
                    Serial.println("]");
                    break;
                case 4:
                    // retrieve data from EEPROM
                    float temp_Max, temp_Min;
                    /*EEPROM.get(tempMaxAddr, temp_Max);
                    EEPROM.get(tempMinAddr, temp_Min);*/
                    temp_Max = EEPROM_readFloat(tempMaxAddr);
                    temp_Min = EEPROM_readFloat(tempMinAddr);

                    Serial.print("Temperature Range [");
                    Serial.print(temp_Max);
                    Serial.print(", ");
                    Serial.print(temp_Min);
                    Serial.println("]");
                    break;
            }
        }
    }
}



//works for single digit
int asciiToNum(int ascii) {
    return (ascii>=49 && ascii <= 52)? ascii - 48: -1;
}

// read from EEPROM byte by byte
// atomic
float EEPROM_readFloat(int addr) {
    noInterrupts();
    float value = 0.0;
    byte* p = (byte*)(void*)&value;
    for (int i = 0; i < sizeof(value); i++)
        *p++ = EEPROM.read(addr++);

    interrupts();
    return value;
}
