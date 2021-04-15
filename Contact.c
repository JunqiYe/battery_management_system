#include "Contact.h"

void contactTask(void* mData) {
  /****************
      Function name: contactTask
      Function inputs: cData pointer
      Function outputs: none
      Function description: perform the contactor task to update contact_on_pff
    *****************/
    noInterrupts();
    contactTaskData* data = (contactTaskData*) mData;
    alarmTaskData* alarmStates = data->aData;

    if (1 == *(data->contactor_flag)) {
        *(data->contactor_flag) = 0; // reset flag
        if (1 == *(data->contact_on_off)){
            *(data->contact_on_off) = OPEN;
        } else {
            *(data->contact_on_off) = CLOSE;
        }
    }

    // HVIL has higher priority in controlling the BLUE LED
    if (*(alarmStates->HVIA_alarm_state)) {
        *(data->contact_on_off) = OPEN;
    } 
    
    if (*(alarmStates->HVIA_alarm_state) != NOT_ACT || 
        *(alarmStates->overcurrent_state) != NOT_ACT || 
        *(alarmStates->HVOR_state) != NOT_ACT) {
        *(data->contact_on_off) = OPEN;
    }
    digitalWrite(*(data->contactPin), *(data->contact_on_off));

    interrupts();
}
