#include "touchInput.h"


void updateTouch(TouchScreen ts, int* prev, int* next, bool* contact_flag, int* curr_display, bool* acknowledge_flag) {
    /****************
    * Function name: updateTouch
    * Function inputs: TouchScreen object, prev pointer, next, pointer, contact_flag pointer, curr_display pointer
    * Function outputs: None
    * Function description: Depending on the display, detect if a touch occurs.
    *****************/
	  TSPoint p;
    bool curr_contact_flag = *contact_flag; // make sure contact_flag is only updated once
    // bool curr_acknowledge_flag = *acknowledge_flag; // make sure contact_flag is only updated once

    for (long i = 0; i < 100; i++) {
         p = ts.getPoint();
         // prev button
        if ((*prev == 0) && (p.z > MINPRESSURE && p.z < MAXPRESSURE)) {
            if(((long)p.x-MINX)*DISMAXX/(MAXX-MINX) > ((1*DISMAXX)/4-ButtonW/2) && ((long)p.x-MINX)*DISMAXX/(MAXX-MINX) < ((1*DISMAXX)/4-ButtonW/2) + ButtonW &&
            	 ((long)(MAXY-p.y))*DISMAXY/(MAXY-MINY) > ((3*DISMAXY)/4)           && ((long)(MAXY-p.y))*DISMAXY/(MAXY-MINY) < (3*DISMAXY)/4 + ButtonH) {
            	  *prev = 1;
            	 }
        }
        // next button
        if ((*next == 0) && (p.z > MINPRESSURE && p.z < MAXPRESSURE)) {
            if(((long)p.x-MINX)*DISMAXX/(MAXX-MINX) > ((3*DISMAXX)/4-ButtonW/2) && ((long)p.x-MINX)*DISMAXX/(MAXX-MINX) < ((3*DISMAXX)/4-ButtonW/2) + ButtonW &&
            	 ((long)(MAXY-p.y))*DISMAXY/(MAXY-MINY) > ((3*DISMAXY)/4)           && ((long)(MAXY-p.y))*DISMAXY/(MAXY-MINY) < (3*DISMAXY)/4 + ButtonH) {
            	  *next = 1;
            	 }
        }
        
        // battery screen on/off button
        if (*curr_display == 1) {
            if (*contact_flag != 1 && (p.z > MINPRESSURE && p.z < MAXPRESSURE)) {
                if(((long)p.x-MINX)*DISMAXX/(MAXX-MINX) > ((1*DISMAXX)/2-ButtonW/2) && ((long)p.x-MINX)*DISMAXX/(MAXX-MINX) < ((1*DISMAXX)/2+ButtonW/2) &&
                  ((long)(MAXY-p.y))*DISMAXY/(MAXY-MINY) > ((1*DISMAXY)/2-ButtonH/2)&& ((long)(MAXY-p.y))*DISMAXY/(MAXY-MINY) < (1*DISMAXY)/2+ButtonH/2) {
                    *contact_flag = 1;
                }
            }
        }

        // acknowledge alarm button
        if (*curr_display == 0) {
            if (*acknowledge_flag != 1 &&(p.z > MINPRESSURE && p.z < MAXPRESSURE)) {
                if(((long)p.x-MINX)*DISMAXX/(MAXX-MINX) > ((1*DISMAXX)/2-ButtonW/2) && ((long)p.x-MINX)*DISMAXX/(MAXX-MINX) < ((1*DISMAXX)/2+ButtonW/2) &&
                  ((long)(MAXY-p.y))*DISMAXY/(MAXY-MINY) > ((1*DISMAXY)/2-ButtonH/2)&& ((long)(MAXY-p.y))*DISMAXY/(MAXY-MINY) < (1*DISMAXY)/2+ButtonH/2) {
                      *acknowledge_flag = 1;
                }
            }
        }
    }
}

void tsInputTask (void* tData) {
      /****************
    * Function name: tsInputTask
    * Function inputs: touchInputData struct
    * Function outputs: None
    * Function description: call all sub-tasks
    *****************/
	touchInputData* data = (touchInputData*) tData;
	updateTouch(*(data->ts), data->prev, data->next, data->contactor_flag, data->curr_display, data->acknowledge_flag);
}
