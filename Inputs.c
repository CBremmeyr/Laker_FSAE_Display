/*
 * Inputs.c
 *
 *  Created on: Feb 12, 2017
 *      Author: Kevin Mahlie
 */

/* DriverLib Includes */
#include "driverlib/MSP432P4xx/driverlib.h"

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>

/* Local Includes */
#include "Inputs.h"
#include "Timing.h"
#include "System_Control.h"

//int Up_Shift = 0, Down_Shift = 0, prev_state = 0;
extern int car_status;
extern int expected_gear;
extern int shifting_flag;
extern int actual_gear;
int Shifter_Status[3];
int Nuetral_Flag = 0;
int Nuetral_IN = 0;
int Oil_IN = 0;
int Find_Nuetral_IN = 0;

uint32_t status_ISR;

void Nuetral_Inputs_Setup()
{
    MAP_GPIO_setAsInputPin(NUETRAL_PORT, NUETRAL_IN);
    MAP_GPIO_setAsInputPinWithPullUpResistor(NUETRAL_PORT, FIND_NUETRAL_IN);
    //  MAP_GPIO_enableInterrupt(NUETRAL_PORT, FIND_NUETRAL_IN);
    //  MAP_Interrupt_enableInterrupt(INT_PORT3); //enables GPIO interrupt on desired PORT
    //  GPIO_interruptEdgeSelect(NUETRAL_PORT,  FIND_NUETRAL_IN,
    //  GPIO_HIGH_TO_LOW_TRANSITION);
}

void OIL_Pressure_Setup(void){

    MAP_GPIO_setAsInputPin(OIL_PORT, OIL_IN);
       //MAP_GPIO_clearInterruptFlag(OIL_PORT, OIL_IN);
      // MAP_GPIO_enableInterrupt(OIL_PORT, OIL_IN);
      // MAP_Interrupt_enableInterrupt(INT_PORT1);
      // GPIO_interruptEdgeSelect(OIL_PORT,  OIL_IN,
       //    GPIO_HIGH_TO_LOW_TRANSITION);
}

/*********************************
 * Function:RPM_VSS_Input_Setup()
 * This function sets up TimerA0
 * pin 2.4 for RPM input capture &
 * pin 2.5 for VSS input capture
 *
 ********************************/
void RPM_VSS_Input_Setup() {

    P2->SEL0 |= BIT5;            // TA0.CCI2A input capture pin, second function
    /* Enables VSS Input Capture on Pin 2.5*/
    P5->SEL0 |= BIT6;   // TA0.CCI1A input capture pin, second function
   // P10->SEL0 |= BIT5;
}


/* GPIO ISR */
void PORT1_IRQHandler(void){

    Oil_IN = GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN6);

    if (Oil_IN == 0)
    {
        P2->OUT |= BIT0;
        GPIO_interruptEdgeSelect(GPIO_PORT_P1,  GPIO_PIN6,
            GPIO_LOW_TO_HIGH_TRANSITION);
    }
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN6); //clear current interrupt flag
}

void PORT2_IRQHandler(void) {

    status_ISR = MAP_GPIO_getEnabledInterruptStatus(SHIFTER_PORT); //determine which pin activated port interupt
    MAP_GPIO_clearInterruptFlag(SHIFTER_PORT, status_ISR); //clear current interrupt flag

    if (status_ISR & UPSHIFT_IN)
    {
        //carStatus = upShifting;
    }
    else if(status_ISR & DOWNSHIFT_IN)
    {
       // carStatus = downShifting;
    }
}

void PORT3_IRQHandler(void){

    Find_Nuetral_IN = GPIO_getInputPinValue(NUETRAL_PORT, FIND_NUETRAL_IN);
    Nuetral_IN = GPIO_getInputPinValue(NUETRAL_PORT, NUETRAL_IN);

     if ( Find_Nuetral_IN == 0 && Nuetral_IN == 1)
    {
        carStatus = findingNuetral;
    }
     else if ( Find_Nuetral_IN == 0 && Nuetral_IN == 0)
     {
         carStatus = displayingDataLogger;
     }
    MAP_GPIO_clearInterruptFlag(NUETRAL_PORT, FIND_NUETRAL_IN); //clear current interrupt flag
}



void DebounceFindNuetral(void) {
    static uint16_t state = 0;
    //MAP_GPIO_disableInterrupt(SHIFTER_PORT, UPSHIFT_IN | DOWNSHIFT_IN);
    //read upshift input @ 50ms debounce
    state = (state << 1) | ReadBtnFindN() | 0x0FFF;
    if (state <= 0x03FF) //check if first button was held
            {
        car_status = 3;
        //printf("2.6 press\n");
       // Delay_Time(10);
    }
}

uint16_t ReadBtnFindN(void) {
    return (P2->IN & FIND_NUETRAL_READ ) >> 3;
}

void DebounceNuetralIn(void) {
    static uint16_t state = 0;
    //MAP_GPIO_disableInterrupt(SHIFTER_PORT, UPSHIFT_IN | DOWNSHIFT_IN);
    //read upshift input @ 50ms debounce
    state = (state << 1) | ReadBtnNuetralIn() | 0x0FFF;
    if (state <= 0x03FF) //check if first button was held
            {
        Nuetral_Flag = 1;
        //printf("2.6 press\n");
       // Delay_Time(10);
    }
}

uint16_t ReadBtnNuetralIn(void) {
    return (P2->IN & NUETRAL_READ ) >> 4;
}

/*********************************
 * Function:PortInit()
 * This function intializes RGB LED
 ********************************/
void PortInit() {

    /*RGB Status LED */
    P2->DIR |= (BIT0 | BIT1 | BIT2 );     // configure RGB LED as output
    P2->OUT &= ~(BIT0 | BIT1 | BIT2 );     // initially turn off RGB LED
    P2->OUT |= BIT0;

}

void CheckSystemInputs(){
         Nuetral_IN = GPIO_getInputPinValue(NUETRAL_PORT, NUETRAL_IN);
         Oil_IN = GPIO_getInputPinValue(OIL_PORT, OIL_IN);
         Find_Nuetral_IN = GPIO_getInputPinValue(NUETRAL_PORT, FIND_NUETRAL_IN);
}
