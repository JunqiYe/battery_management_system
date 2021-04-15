#include "Alarm.h"

void updateHVIL_Alarm(volatile int* HVIL_Alarm_State, bool* HVIL) {
  //, bool* acknowledge_flag
    /****************
    * Function name: updateHVIL_Alarm
    * Function inputs: pointer to HVIL_Alarm_State
    * Function outputs: None
    * Function description: recieves HVIL_Alarm_State pointer and updates its value
    *****************/
    if (*HVIL == false) {
        *HVIL_Alarm_State = NOT_ACT;
    }
  	return;
}

void updateOvercurrent_Alarm(int* Overcurrent_Alarm_State, float* hvCurrent) {
    /****************
    * Function name: updateOvercurrent_Alarm
    * Function inputs: Overcurrent_Alarm_State pointer
    * Function outputs: None
    * Function description: updated Overcurrent_Alarm_State pointer's values
    *****************/
    if (*hvCurrent >= -5 && *hvCurrent <= 20) {
        *Overcurrent_Alarm_State = NOT_ACT;
    } else if (*Overcurrent_Alarm_State != ACTIVE_ACK){
        *Overcurrent_Alarm_State = ACTIVE_NOT_ACK;
    }

    return;
}

void updateHVOR_Alarm(int* HVOR_Alarm_State, float* hvVoltage) {
    /****************
    * Function name: updateHVOR_Alarm
    * Function inputs: HVOR_Alarm_State pointer
    * Function outputs: None
    * Function description: updates HVOR_Alarm_State pointer's value
    *****************/
    if (*hvVoltage >= 280 && *hvVoltage <= 405) {
        *HVOR_Alarm_State = NOT_ACT;
    } else if (*HVOR_Alarm_State != ACTIVE_ACK){
        *HVOR_Alarm_State = ACTIVE_NOT_ACK;
    }
  	return;
}

// Disabled alarm
void current_Alarm(bool* activeAlarm, volatile int* HVIL_Alarm_State ,int* Overcurrent_Alarm_State, int* HVOR_Alarm_State) {
    if (*HVIL_Alarm_State == ACTIVE_NOT_ACK || *Overcurrent_Alarm_State == ACTIVE_NOT_ACK || *HVOR_Alarm_State == ACTIVE_NOT_ACK){
      //*activeAlarm = true;
        *activeAlarm = false;
    }

    else *activeAlarm = false;
}

void acknowledge(bool* acknowledge_flag, volatile int* HVIL_Alarm_State ,int* Overcurrent_Alarm_State, int* HVOR_Alarm_State) {
    /****************
    * Function name: acknowledge
    * Function inputs: alarm states
    * Function outputs: acknowledge flag
    * Function description: depending on the flag, ackownledge the alarm
    *****************/
    if (*acknowledge_flag == true) {
        *acknowledge_flag = false;
        if (*HVIL_Alarm_State == ACTIVE_NOT_ACK) {
            *HVIL_Alarm_State = ACTIVE_ACK;
        }
        if (*Overcurrent_Alarm_State == ACTIVE_NOT_ACK) {
            *Overcurrent_Alarm_State = ACTIVE_ACK;
        }
    
        if (*HVOR_Alarm_State == ACTIVE_NOT_ACK) {
            *HVOR_Alarm_State = ACTIVE_ACK;
        }
    }

}



void alarmTask(void* alData) {
    /****************
    * Function name: alarmTask
    * Function inputs: alarmTaskData struct
    * Function outputs: None
    * Function description: takes alarmTaskData, deference, and call sub-tasks
    *****************/
  	alarmTaskData* data = (alarmTaskData*) alData;
    measurementData* mdata = data->measure;

    updateHVIL_Alarm(data->HVIA_alarm_state, mdata->hvilStatus);
    updateOvercurrent_Alarm(data->overcurrent_state, mdata->hvCurrent);
    updateHVOR_Alarm(data->HVOR_state, mdata->hvVoltage);
    current_Alarm(data->activeAlarm, data->HVIA_alarm_state, data->overcurrent_state, data->HVOR_state);
    acknowledge(data->acknowledge_flag, data->HVIA_alarm_state, data->overcurrent_state, data->HVOR_state);
	return;
}
