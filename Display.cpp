#include "Display.h"

void displayMeasurement(Elegoo_TFTLCD tft,void* mData, void* sData, void* hData) {
    /****************
    * Function name: displayMeasurement
    * Function inputs: Elegoo_TFTLCD, measurementTaskData struct pointer, socTaskData struct pointer
    * Function outputs: Display output
    * Function description: Output measurement screen
    *****************/
    measurementData* data = (measurementData*) mData;
    socTaskData* data1 = (socTaskData*) sData;
    displayHistory* hist = (displayHistory*) hData;
    
    tft.fillScreen(BLACK);
    //unsigned long start = micros();
    tft.setCursor(0, 0);
    tft.setTextColor(RED);  tft.setTextSize(3);
    tft.println("Measurements");
    tft.setTextColor(WHITE);  tft.setTextSize(2);
    tft.println();
    tft.println();
    tft.println();
    tft.print("Temperature: ");
    tft.println(*(data->temperature));
    *(hist->prev_temp) = *(data->temperature);
    tft.print("HV Current: ");
    tft.println(*(data->hvCurrent));
    *(hist->prev_HVC) = *(data->hvCurrent);
    tft.print("HV Voltage: ");
    tft.println(*(data->hvVoltage));
    *(hist->prev_HVV) = *(data->hvVoltage);
    tft.print("HVIL stat: ");
    tft.println(*(data->hvilStatus));
    *(hist->prev_HVIL) = *(data->hvilStatus);
    tft.print("Soc: ");
    tft.println(*(data1->stateofCharge));
    *(hist->prev_soc) = *(data1->stateofCharge);
}

#define TEXT_H 16
#define FILLW 200

void update_displayMeasurement(Elegoo_TFTLCD tft, void* mData, void* sData, void* hData) {
    /****************
    * Function name: displayMeasurement
    * Function inputs: Elegoo_TFTLCD, measurementTaskData struct pointer, socTaskData struct pointer
    * Function outputs: Display output
    * Function description: Output measurement screen
    *****************/
    measurementData* data = (measurementData*) mData;
    socTaskData* data1 = (socTaskData*) sData;
    displayHistory* hist = (displayHistory*) hData;
    
    if (*(data->temperature) != *(hist->prev_temp)) {
        *(hist->prev_temp) = *(data->temperature);
        tft.fillRect(150, 70, 200, TEXT_H, BLACK); // temperture
        tft.setCursor(150, 72);
        tft.setTextColor(WHITE);  tft.setTextSize(2);
        tft.print(*(data->temperature));
    }

    if (*(data->hvCurrent) != *(hist->prev_HVC)) {
        *(hist->prev_HVC) = *(data->hvCurrent);
        tft.fillRect(130, 70 + TEXT_H, FILLW, TEXT_H, BLACK); // HVC
        tft.setCursor(130, 70 + TEXT_H +2);
        tft.setTextColor(WHITE);  tft.setTextSize(2);
        tft.print(*(data->hvCurrent));
    }

    if (*(data->hvVoltage) != *(hist->prev_HVV)) {
        *(hist->prev_HVV) = *(data->hvVoltage);
        tft.fillRect(130, 70 + 2*TEXT_H, FILLW, TEXT_H, BLACK); // HVV
        tft.setCursor(130, 70 + 2*TEXT_H +2);
        tft.setTextColor(WHITE);  tft.setTextSize(2);
        tft.print(*(data->hvVoltage));
    }

    
    if (*(data->hvilStatus) != *(hist->prev_HVIL)) {
        *(hist->prev_HVIL) = *(data->hvilStatus);
        tft.fillRect(130, 70 + 3*TEXT_H, FILLW, TEXT_H, BLACK); // HVIL
        tft.setCursor(130, 70 + 3*TEXT_H +2);
        tft.setTextColor(WHITE);  tft.setTextSize(2);
        tft.print(*(data->hvilStatus));
    }

    
    if (*(data1->stateofCharge) != *(hist->prev_soc)) {
        *(hist->prev_soc) = *(data1->stateofCharge);
        tft.fillRect(60, 70 + 4*TEXT_H, FILLW, TEXT_H, BLACK); // soc
        tft.setCursor(60, 70 + 4*TEXT_H +2);
        tft.setTextColor(WHITE);  tft.setTextSize(2);
        tft.print(*(data1->stateofCharge));
    }
    

}

void displayAlarm(Elegoo_TFTLCD tft, void* AState, void* hData) {
    /****************
    * Function name: displayAlarm
    * Function inputs: Elegoo_TFTLCD, AlarmTaskData struct pointer
    * Function outputs: Display output
    * Function description: Output Alarm screen
    *****************/
    alarmTaskData* state = (alarmTaskData*) AState;
    displayHistory* hist = (displayHistory*) hData;
    
    tft.fillScreen(BLACK);
    //unsigned long start = micros();
    tft.setCursor(0, 0);
    tft.setTextColor(RED);  tft.setTextSize(3);
    tft.println("Alarm");
    tft.setTextColor(WHITE);  tft.setTextSize(2);
    tft.println();

    // print high via states
    tft.setCursor(0, TITLE_OFFSET);
    tft.println("High VIA:");
    int via_state = *(state->HVIA_alarm_state);
    //tft.println(via_state);
    tft.println(alarmAcknowledgment(via_state));
    *(hist->prev_hvia) = via_state;
    tft.println();

    // print overcurrent states
    tft.setCursor(0, TITLE_OFFSET + WORD_OFFSET);
    tft.println("Overcurrent: ");
    int oc_state = *(state->overcurrent_state);
    //tft.println(oc_state);
    tft.println(alarmAcknowledgment(oc_state));
    *(hist->prev_overC) = oc_state;
    tft.println();

    // print hvor states
    tft.setCursor(0, TITLE_OFFSET + 2*WORD_OFFSET);
    tft.println("HVOR: ");
    int hvor_state = *(state->HVOR_state);
    //tft.println(hvor_state);
    tft.println(alarmAcknowledgment(hvor_state));
    *(hist->prev_hvor) = hvor_state;
    tft.println();

    // draw if one of the state is ACTIVE ALARM. dont draw is none of the state is ACTIVE ALARM
    if (!*(hist->ack_button_drawn) && (via_state == 1 || oc_state == 1 || hvor_state == 1)) {
        displayAckButton(tft, AState, hist);
        *(hist->button_not_drawn) = true;
        *(hist->ack_button_drawn) = true;  
    }
}


void update_displayAlarm(Elegoo_TFTLCD tft, void* AState, void* hData) {
    /****************
    * Function name: update_displayAlarm
    * Function inputs: Elegoo_TFTLCD, measurementTaskData struct pointer, socTaskData struct pointer
    * Function outputs: Display output
    * Function description: Output measurement screen
    *****************/
    alarmTaskData* state = (alarmTaskData*) AState;
    displayHistory* hist = (displayHistory*) hData;
    
    // erase ack button
    if (*(hist->ack_button_drawn) && *(state->overcurrent_state) != 1 && *(state->HVIA_alarm_state) != 1 && *(state->HVOR_state) != 1) {
        tft.fillRect((1*DISMAXX)/2-ButtonW/2-1, (DISMAXY/2)-ButtonH/2, ButtonW+1, ButtonH/2+30, BLACK);
        *(hist->ack_button_drawn) = false;

        // print HVOR message again
        tft.fillRect(0, TITLE_OFFSET + 2*WORD_OFFSET + TEXT_H, 250, TEXT_H*2, BLACK); // hvia
        tft.setCursor(0, TITLE_OFFSET + 2*WORD_OFFSET + TEXT_H);
        tft.setTextColor(WHITE);  tft.setTextSize(2);
        tft.print(alarmAcknowledgment(*(state->HVOR_state)));
    }
    
    if (*(state->HVIA_alarm_state) != *(hist->prev_hvia)) {
        *(hist->prev_hvia) = *(state->HVIA_alarm_state);
        tft.fillRect(0, TITLE_OFFSET + TEXT_H, 250, TEXT_H*2, BLACK); // hvia
        tft.setCursor(0, TITLE_OFFSET + TEXT_H);
        tft.setTextColor(WHITE);  tft.setTextSize(2);
        tft.print(alarmAcknowledgment(*(state->HVIA_alarm_state)));
    }
    

    if (*(state->overcurrent_state) != *(hist->prev_overC)) {
        *(hist->prev_overC) = *(state->overcurrent_state);
        tft.fillRect(0, TITLE_OFFSET + WORD_OFFSET + TEXT_H, 250, TEXT_H*2, BLACK); // hvia
        tft.setCursor(0, TITLE_OFFSET + WORD_OFFSET + TEXT_H);
        tft.setTextColor(WHITE);  tft.setTextSize(2);
        tft.print(alarmAcknowledgment(*(state->overcurrent_state)));
    }

    
    if (*(state->HVOR_state) != *(hist->prev_hvor)) {
        *(hist->prev_hvor) = *(state->HVOR_state);
        tft.fillRect(0, TITLE_OFFSET + 2*WORD_OFFSET + TEXT_H, 250, TEXT_H*2, BLACK); // hvia
        tft.setCursor(0, TITLE_OFFSET + 2*WORD_OFFSET + TEXT_H);
        tft.setTextColor(WHITE);  tft.setTextSize(2);
        tft.print(alarmAcknowledgment(*(state->HVOR_state)));
    }

    // if ack_button not drawn and one of the alarm is in ACTIVE ALARM state, draw ack_button
    if (!*(hist->ack_button_drawn) && (*(state->overcurrent_state) == 1 || *(state->HVIA_alarm_state) == 1 | *(state->HVOR_state) == 1)) {
        displayAckButton(tft, AState, hist);
        // *(hist->button_not_drawn) = true;
        *(hist->ack_button_drawn) = true;
    }

}

const char* alarmAcknowledgment(int state) {
    /****************
    * Function name: alarmAcknowledgment
    * Function inputs: state number
    * Function outputs: string with the alarm message
    * Function description: depending on the state number, output the alarm message.
    *****************/
    if (state == 0){
      return "NOT ACTIVE"; 
    } else if (state == 1) {
      return "ACTIVE, NOT ACKNOWLEDGED";  
    } else {
      return "ACTIVE, ACKNOWLEDGED";  
    }
}


void displayBattery(Elegoo_TFTLCD tft, void* socData, void* contactorTaskData) {
    /****************
    * Function name: displayBattery
    * Function inputs: Elegoo_TFTLCD, socTaskData struct pointer, contactTaskData struct pointer
    * Function outputs: Display output
    * Function description: Output battery screen
    *****************/
    socTaskData* data = (socTaskData*) socData;
    contactTaskData* data1 = (contactTaskData*) contactorTaskData;
    
    tft.fillScreen(BLACK);
    tft.setCursor(DISMAXX/2-ButtonW/3, DISMAXY/2-ButtonH/3);
    tft.setTextSize(2);
    if(*(data1->contact_on_off)){
        tft.fillRoundRect((1*DISMAXX)/2-ButtonW/2, (DISMAXY/2)-ButtonH/2, ButtonW, ButtonH, ButtonR, BLUE); //blue LED on
        tft.setTextColor(WHITE);
        tft.print("ON");
    } else {
        tft.fillRoundRect((1*DISMAXX)/2-ButtonW/2, (DISMAXY/2)-ButtonH/2, ButtonW, ButtonH, ButtonR, WHITE); //white LED off
        tft.setTextColor(BLUE);
        tft.print("OFF");
    }
    tft.setCursor(0, 0);
    tft.setTextColor(RED);  tft.setTextSize(3);
    tft.println("Battery");
    tft.println("on/off");
    tft.setTextColor(WHITE);  tft.setTextSize(2);
}


void update_displayBattery(Elegoo_TFTLCD tft, void* socData, void* contactorTaskData, void* hData) {
    /****************
    * Function name: update_displayAlarm
    * Function inputs: Elegoo_TFTLCD, measurementTaskData struct pointer, socTaskData struct pointer
    * Function outputs: Display output
    * Function description: Output measurement screen
    *****************/
    socTaskData* data = (socTaskData*) socData;
    contactTaskData* data1 = (contactTaskData*) contactorTaskData;
    displayHistory* hist = (displayHistory*) hData;

    if (*(data1->contact_on_off) != *(hist->prev_contactor)) {
        *(hist->prev_contactor) = *(data1->contact_on_off);
        tft.setCursor(DISMAXX/2-ButtonW/3, DISMAXY/2-ButtonH/3);
        tft.setTextSize(2);
        if(*(data1->contact_on_off)){
            tft.fillRoundRect((1*DISMAXX)/2-ButtonW/2, (DISMAXY/2)-ButtonH/2, ButtonW, ButtonH, ButtonR, BLUE); //blue LED on
            tft.setTextColor(WHITE);
            tft.print("ON");
        } else {
            tft.fillRoundRect((1*DISMAXX)/2-ButtonW/2, (DISMAXY/2)-ButtonH/2, ButtonW, ButtonH, ButtonR, WHITE); //white LED off
            tft.setTextColor(BLUE);
            tft.print("OFF");
        }
    }
}


void displayAckButton(Elegoo_TFTLCD tft, void* AState, void* hData) {
    alarmTaskData* state = (alarmTaskData*) AState;
    displayHistory* hist = (displayHistory*) hData;
    tft.setCursor(DISMAXX/2-ButtonW/3, DISMAXY/2-ButtonH/3);
    tft.setTextSize(2);
    if (1 == *(state->HVIA_alarm_state) || 1 == *(state->overcurrent_state) || 1 == *(state->HVOR_state)) {
          tft.fillRoundRect((1*DISMAXX)/2-ButtonW/2, (DISMAXY/2)-ButtonH/2, ButtonW, ButtonH, ButtonR, BLUE); //blue LED on
          tft.setTextColor(WHITE);
          tft.print("ACK");
    } else {
          displayAlarm(tft, AState, hist);
    }
}

void displayAccelerometer(Elegoo_TFTLCD tft, void* acData, void* hData) {
    accelerometerData* data = (accelerometerData*) acData;  
    displayHistory* hist = (displayHistory*) hData;
    tft.fillScreen(BLACK);
    
    tft.setCursor(0, 0);
    tft.setTextColor(RED);  tft.setTextSize(3);
    tft.println("Acceleration");
    tft.setTextColor(WHITE);  tft.setTextSize(2);
    tft.println();
    tft.println();
    tft.println();
    
    tft.print("x_pos: ");
    tft.print(*(data->x_position));
    tft.println("cm");
    *(hist->prev_x_position) = *(data->x_position);
    
    tft.print("y_pos: ");
    tft.print(*(data->y_position));
    tft.println("cm");
    *(hist->prev_y_position) = *(data->y_position); 
    
    tft.print("z_pos: ");
    tft.print(*(data->z_position));
    tft.println("cm");
    *(hist->prev_z_position) = *(data->z_position); 
    
    tft.print(" dist: ");
    tft.print(*(data->distance));
    tft.println("cm");
    *(hist->prev_distance) = *(data->distance); 
    
    tft.print("x_ang: ");
    tft.print(*(data->x_angle));
    tft.println("deg");
    *(hist->prev_x_angle) = *(data->x_angle);
    
    tft.print("y_ang: ");
    tft.print(*(data->y_angle));
    tft.println("deg");
    *(hist->prev_y_angle) = *(data->y_angle); 
    
    tft.print("z_ang: ");
    tft.print(*(data->z_angle));
    tft.println("deg");
    *(hist->prev_z_angle) = *(data->z_angle); 
}

#define X_OFFSET 84
#define Y_OFFSET 70
#define TEXT_HEIGHT 16
#define TEXT_WIDTH 95
#define CHAR_OFFSET 20
#define PRINT_TEXT_OFFSET 2

void updateDisplayAccelerometer(Elegoo_TFTLCD tft, void* acData, void* hData) {
    accelerometerData* data = (accelerometerData*)acData;
    displayHistory* hist = (displayHistory*)hData;

    // set print standard
    tft.setTextColor(WHITE);
    tft.setTextSize(2);
    
    // update x_position on screen
    if (*(data->x_position) != *(hist->prev_x_position)) {
        *(hist->prev_x_position) = *(data->x_position); // update hist x_pos
        tft.fillRect(X_OFFSET, Y_OFFSET, TEXT_WIDTH*2, TEXT_HEIGHT, BLACK);
        tft.setCursor(X_OFFSET, Y_OFFSET + PRINT_TEXT_OFFSET);
        tft.print(*(data->x_position));
        tft.print("cm");    
    }

    // update y_position on screen
    if (*(data->y_position) != *(hist->prev_y_position)) {
        *(hist->prev_y_position) = *(data->y_position); // update hist y_pos
        tft.fillRect(X_OFFSET, Y_OFFSET + TEXT_HEIGHT, TEXT_WIDTH*2, TEXT_HEIGHT, BLACK);
        tft.setCursor(X_OFFSET, Y_OFFSET + TEXT_HEIGHT + PRINT_TEXT_OFFSET);
        tft.print(*(data->y_position));
        tft.print("cm");    
    }

    // update z_position on screen
    if (*(data->z_position) != *(hist->prev_z_position)) {
        *(hist->prev_z_position) = *(data->z_position); // update hist z_pos
        tft.fillRect(X_OFFSET, Y_OFFSET + 2 * TEXT_HEIGHT, TEXT_WIDTH*2, TEXT_HEIGHT, BLACK);
        tft.setCursor(X_OFFSET, Y_OFFSET + 2 * TEXT_HEIGHT + PRINT_TEXT_OFFSET);
        tft.print(*(data->z_position));
        tft.print("cm");
    }

    // update dist on screen
    if (*(data->distance) != *(hist->prev_distance)) {
        *(hist->prev_distance) = *(data->distance); // update hist dist
        tft.fillRect(X_OFFSET, Y_OFFSET + 3 * TEXT_HEIGHT, TEXT_WIDTH*2, TEXT_HEIGHT, BLACK);
        tft.setCursor(X_OFFSET, Y_OFFSET + 3 * TEXT_HEIGHT + PRINT_TEXT_OFFSET);
        tft.print(*(data->distance));
        tft.print("cm");    
    }

    // update x_ang on screen
    if (*(data->x_angle) != *(hist->prev_x_angle)) {
        *(hist->prev_x_angle) = *(data->x_angle); // update hist dist
        tft.fillRect(X_OFFSET, Y_OFFSET + 4 * TEXT_HEIGHT, TEXT_WIDTH + CHAR_OFFSET + 4, TEXT_HEIGHT, BLACK);  
        tft.setCursor(X_OFFSET, Y_OFFSET + 4 * TEXT_HEIGHT + PRINT_TEXT_OFFSET);
        tft.print(*(data->x_angle));
        tft.print("deg");  
    }

    // update y_ang on screen
    if (*(data->y_angle) != *(hist->prev_y_angle)) {
        *(hist->prev_y_angle) = *(data->y_angle); // update hist dist
        tft.fillRect(X_OFFSET, Y_OFFSET + 5 * TEXT_HEIGHT, TEXT_WIDTH + CHAR_OFFSET + 4, TEXT_HEIGHT, BLACK);   
        tft.setCursor(X_OFFSET, Y_OFFSET + 5 * TEXT_HEIGHT + PRINT_TEXT_OFFSET);
        tft.print(*(data->y_angle));
        tft.print("deg"); 
    }

    // update z_ang on screen
    if (*(data->z_angle) != *(hist->prev_z_angle)) {
        *(hist->prev_z_angle) = *(data->z_angle); // update hist dist
        tft.fillRect(X_OFFSET, Y_OFFSET + 6 * TEXT_HEIGHT, TEXT_WIDTH + CHAR_OFFSET + 4, TEXT_HEIGHT + 2, BLACK);
        tft.setCursor(X_OFFSET, Y_OFFSET + 6 * TEXT_HEIGHT + PRINT_TEXT_OFFSET);
        tft.print(*(data->z_angle));
        tft.print("deg");    
    }
}

void displayButton(Elegoo_TFTLCD tft, int* prev, int* next) {
    /****************
    * Function name: displayButton
    * Function inputs: Elegoo_TFTLCD, prev pointer, next pointer
    * Function outputs: Display output
    * Function description: Based on prev and next value, output cooreponding button.
    *****************/

    if(*prev == true){
        tft.fillRoundRect((1*DISMAXX)/4-ButtonW/2, (3*DISMAXY)/4, ButtonW, ButtonH, ButtonR, BLACK); //prssed, draw inverse
        *prev = false;
    } else {
        tft.fillRoundRect((1*DISMAXX)/4-ButtonW/2, (3*DISMAXY)/4, ButtonW, ButtonH, ButtonR, WHITE); //not prssed, draw normal
    }

    if(*next == true) {
        tft.fillRoundRect((3*DISMAXX)/4-ButtonW/2, (3*DISMAXY)/4, ButtonW, ButtonH, ButtonR, BLACK);
        *next = false;
    } else {
        tft.fillRoundRect((3*DISMAXX)/4-ButtonW/2, (3*DISMAXY)/4, ButtonW, ButtonH, ButtonR, WHITE);
    }

    tft.setCursor((1*DISMAXX)/4-ButtonW/3, (3*DISMAXY)/4+ButtonH/3);
    tft.setTextColor(RED);  tft.setTextSize(2);
    tft.print("PREV");
    
    tft.setCursor((3*DISMAXX)/4-ButtonW/3, (3*DISMAXY)/4+ButtonH/3);
    tft.setTextColor(RED);  tft.setTextSize(2);
    tft.print("NEXT");
}


void displayTask(void* dData) {
    /****************
    * Function name: displayTask
    * Function inputs: displayTaskData struct
    * Function outputs: None
    * Function description: takes displayTaskData struct and calls sub-tasks. Display output.
    *****************/
    
    displayTaskData* data = (displayTaskData*) dData;
    displayHistory* hist = data -> history;

    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    if (*(data->curr_display) > 2) *(data->curr_display) = -1;
    if (*(data->curr_display) < -1) *(data->curr_display) = 2;
    if (*(data->curr_display) == -1) { // measurement screen
        if (*(hist->prev_dis) != *(data->curr_display)) {
            displayMeasurement(*(data->tft), data->measure, data->socData, hist);
        } else {
            update_displayMeasurement(*(data->tft), data->measure, data->socData, hist);
        }
        
        *(hist->prev_dis) = -1;
    } else if (*(data->curr_display) == 0) { // alarm screen
        if (*(hist->prev_dis) != *(data->curr_display)) {
            displayAlarm(*(data->tft), data->alarmData, hist);
        } else {
            update_displayAlarm(*(data->tft), data->alarmData, hist);
        }
        *(hist->prev_dis) = 0;
    } else if (*(data->curr_display) == 1){ // battery screen
        if (*(hist->prev_dis) != *(data->curr_display)) {
            displayBattery(*(data->tft), data->socData, data->contactData);
        } else {
            update_displayBattery(*(data->tft), data->socData, data->contactData, hist);
        }
        *(hist->prev_dis) = 1;
    } else { // acclerometer screen
        if (*(hist->prev_dis) != *(data->curr_display)) {
            displayAccelerometer(*(data->tft), data->acData, hist);  
        } else {
            updateDisplayAccelerometer(*(data->tft), data->acData, hist);
        }
        *(hist->prev_dis) = 2;
    }

    alarmTaskData* AState = (alarmTaskData*) data->alarmData;
    if (!*(AState->activeAlarm)) {
        *(data->curr_display) += *(data->next_pressed);
        *(data->curr_display) -= *(data->prev_pressed);
    } else {
        // draw ack button if already in alarm screen
        *(data->curr_display) = 0;
        if (*(hist->prev_dis) != 0){
            displayButton(*(data->tft), data->prev_pressed, data->next_pressed);// after updating curr_display b/c resets next,prev value
            *(hist->button_not_drawn) = false;
            *(hist->ack_button_drawn) = false;
        }   
    }
  

    if (0 == *(data->next_pressed) && 0 == *(data->prev_pressed) && *(hist->button_not_drawn) == true) {
        displayButton(*(data->tft), data->prev_pressed, data->next_pressed);// after updating curr_display b/c resets next,prev value
        *(hist->button_not_drawn) = false;
    } else if (1 == *(data->next_pressed) || 1 == *(data->prev_pressed)) {
        displayButton(*(data->tft), data->prev_pressed, data->next_pressed);// after updating curr_display b/c resets next,prev value
        *(hist->button_not_drawn) = true;
    }

    
    
  return;
}
