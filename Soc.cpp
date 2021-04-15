#include "Soc.h"

void socTask(void* sData) {
	/****************
    * Function name: socTask
    * Function inputs: sData pointer
    * Function outputs: none
    * Function description: perform the soc task by updating 2d Interpolation
    *****************/
  	socTaskData* data = (socTaskData*) sData;
    measurementData* measure = data -> mData;

    float hvv = *(measure->hvVoltage);
    float hvc = *(measure->hvCurrent);
    float V_oc = hvv + hvc * RBATT;


    if (V_oc <= 200) {
        *(data->stateofCharge) = 0.0;
    } else if (V_oc >= 400){
        *(data->stateofCharge) = 100.0;
    } else {
        int volt_index = 0;
        int temp_index = 0;
        float volt_perc = 0;
        float temp_perc = 0;
        for(int i = 0; i < sizeof(data->volt_arr)/sizeof(int)-1; i++) {
            if (V_oc > data->volt_arr[i] && V_oc <= data->volt_arr[i+1]) {
                volt_index = i;
                volt_perc = (V_oc - data->volt_arr[i]) / (data->volt_arr[i+1]-data->volt_arr[i]);
                break;
            }
        }
        
        for(int i = 0; i < sizeof(data->temp_arr)/sizeof(int)-1; i++) {
            if (*(measure->temperature) > data->temp_arr[i] && *(measure->temperature) <= data->temp_arr[i+1]) {
                temp_index = i;
                temp_perc = (*(measure->temperature) - data->temp_arr[i]) / (data->temp_arr[i+1]-data->temp_arr[i]);
                break;
            }
        }

        // four data points
        int charge1_1 = data->lut[temp_index][volt_index];
        int charge1_2 = data->lut[temp_index][volt_index+1];
        int charge2_1 = data->lut[temp_index+1][volt_index];
        int charge2_2 = data->lut[temp_index+1][volt_index+1];

        // after 1st interpolation, two data points
        float charge_1_aft_volt = (charge1_2 - charge1_1) * volt_perc + charge1_1;
        float charge_2_aft_volt = (charge2_2 - charge2_1) * volt_perc + charge2_1;

        // 2nd interpolation
        float charge_aft_temp = (charge_2_aft_volt - charge_1_aft_volt) * temp_perc + charge_1_aft_volt;

        *(data->stateofCharge) = charge_aft_temp;
    }
}
