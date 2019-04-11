/*
 * Timing.c
 *
 *  Created on: Feb 15, 2017
 *      Author: Kevin Mahlie
 */

/* DriverLib Includes */
#include "driverlib/MSP432P4xx/driverlib.h"

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>

#include "Timing.h"
#include "System_Control.h"
#include "Math.h"
#include "driverlib/MSP432P4xx/driverlib.h"
#include "Inputs.h"

int Input_Capt_ISR_Status = 0;
int delay_counter = 0;
int Read_Count = 0;
uint16_t RPM_Capt_Values[20] = {0};
uint16_t RPM_Capt_Pointer = 0;
uint16_t VSS_Capt_Values[20] = {0};
uint16_t VSS_Capt_Pointer = 0;
float Pulse_Count_RPM = 0;
float Pulse_Count_VSS = 0;
extern int car_status;
extern int shifter_counter;
extern int clutch_counter;
extern int clutch_flag;
extern int shift_flag;
extern int delay_flag;
extern int actual_gear;


/*********************************
 * Function:Clock_Init
 * This function intializes MSP to use
 * offboard crystals for fast speed.
 * Sets
 * MCLK= HFXT 48Mhz
 * SMCLK = DCO 1Mhz
 * ACLK = REFO = 1kHz
 ********************************/
void Clock_Init(void) // sets Up clock Signals
{

    /*************************************************************************************/
    // sets the clock module to use the external 48 MHz crystal
    /* Configuring pins for peripheral/crystal usage */
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_PJ,
    GPIO_PIN3 | GPIO_PIN2, GPIO_PRIMARY_MODULE_FUNCTION);
    CS_setExternalClockSourceFrequency(32000, 48000000); // enables getMCLK to LFXT (32kHz), getSMCLK to HFXT (48MHz)
    // HFXT = High Frequency Xternal crystal  ** Clock Source **
    // LFXT =  Low Freq  "                   "

    /* Starting HFXT in non-bypass mode without a timeout. Before we start
     * we have to change VCORE to 1 to support the 48MHz frequency */
    MAP_PCM_setCoreVoltageLevel(PCM_VCORE1);
    MAP_FlashCtl_setWaitState(FLASH_BANK0, 2);
    MAP_FlashCtl_setWaitState(FLASH_BANK1, 2);
    CS_startHFXT(false); // false means that there are no timeouts set, will return when stable

    /* Initializing MCLK to HFXT (effectively 48MHz) */
    //MCLK = Master Clock   ** Clock Signal **
    MAP_CS_initClockSignal(CS_MCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);
    /*****************************************************************************************/
    /* Initializing DCO to 1MHz */
    //DCO = Digital Crystal Oscillator (on-chip) ** Clock Source **
    CS_setDCOFrequency(10000000);
    /* Initializing SMCLK to DCO source of 1Mhz*/
    //SMCLK = Sub Master Clock ** Clock Signal **
    MAP_CS_initClockSignal(CS_SMCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);
    /*****************************************************************************************/
    /* Initializing REFO to 1MHz */
    // REFO = Reference Oscillator 32kHz max (on-chip) ** Clock Source **
    MAP_CS_setReferenceOscillatorFrequency(CS_REFO_32KHZ); //sets REF0 to 32KHZ

    /* Initializing ACLK to REFO of 32kHz */
    // ACLK = Alternaitve Clock? ** Clock Signal **
    MAP_CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_16); //sets ACLK to 2kHZ
   /*****************************************************************************************/
}
/*********************************
 * Function:Timer_Setup
 * This function intializes TimerA0
 * for input capture of RPM & VSS
 * digital waveforms. Also enables
 * TimerA1 =
 * TiemrA2=
 * TimerA3=
 ********************************/
void Timer_Setup(void) {

    /* To change timer configuration adjust clock and prescalar, then adjust count value. */
    /*****************************************************************************************/
    /******TimerA0 Setup for Input Capture*****/

    NVIC->ISER[0] = 1 << ((TA0_N_IRQn) & 31);   //ISR enable for timer

   /* TIMER_A0->CCTL[2] = TIMER_A_CCTLN_CM_1 | // Capture rising edge,
            TIMER_A_CCTLN_CCIS_0 |          // Use CCI1A, RPM INPUT PIN, 2.4
            TIMER_A_CCTLN_CCIE |            // Enable capture interrupt
            TIMER_A_CCTLN_CAP |             // Enable capture mode,
            TIMER_A_CCTLN_SCS;   */           // Synchronous capture

            TIMER_A0->CCTL[2] = TIMER_A_CCTLN_CM_2 | // Capture rising edge,
            TIMER_A_CCTLN_CCIS_0 |          // Use CCI2A, VSS INPUT PIN, 2.5
            TIMER_A_CCTLN_CCIE |            // Enable capture interrupt
            TIMER_A_CCTLN_CAP |             // Enable capture mode,
            TIMER_A_CCTLN_SCS;

    TIMER_A0->CTL |= TIMER_A_CTL_SSEL__SMCLK | // Use SMCLK as clock source for both inputs
            TIMER_A_CTL_MC_2;              // Start timer in continuous mode

    /******** Timer A0 Setup Complete ********/
    /*****************************************************************************************/
    /*****************************************************************************************/
      /******TimerA2 Setup for Input Capture*****/

      NVIC->ISER[0] = 1 << ((TA2_N_IRQn) & 31);   //ISR enable for timer

    TIMER_A2->CCTL[1] = TIMER_A_CCTLN_CM_1 | // Capture rising edge,
              TIMER_A_CCTLN_CCIS_0 |          // Use CCI1A, RPM INPUT PIN, 5.6
              TIMER_A_CCTLN_CCIE |            // Enable capture interrupt
              TIMER_A_CCTLN_CAP |             // Enable capture mode,
              TIMER_A_CCTLN_SCS;              // Synchronous capture

     /* TIMER_A0->CCTL[2] = TIMER_A_CCTLN_CM_2 | // Capture rising edge,
              TIMER_A_CCTLN_CCIS_0 |          // Use CCI2A, VSS INPUT PIN, 2.5
              TIMER_A_CCTLN_CCIE |            // Enable capture interrupt
              TIMER_A_CCTLN_CAP |             // Enable capture mode,
              TIMER_A_CCTLN_SCS;*/

      TIMER_A2->CTL |= TIMER_A_CTL_SSEL__SMCLK | // Use SMCLK as clock source for both inputs
              TIMER_A_CTL_MC_2;              // Start timer in continuous mode

      /******** Timer A2 Setup Complete ********/
      /*****************************************************************************************/
    /******TimerA1 Setup for __ counter*****/

    /* Configures TA1 on a 30 second Timer          */
    NVIC->ISER[0] = 1 << ((TA1_0_IRQn) & 31);   //ISR enable for timer
    TIMER_A1->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG; //turns off capture/compare ISR flag
    TIMER_A1->CCTL[0] = TIMER_A_CCTLN_CCIE; // TACCR0 interrupt enabled
    //TIMER_A1->CCR[0] = 60000;
    TIMER_A1->CCR[0] = 2000; //Intially turns off timer. Set value here with enable function
    TIMER_A1->CTL = TIMER_A_CTL_SSEL__ACLK | // Set clock to ACLK, UP mode Count
            TIMER_A_CTL_MC__UP;


    /******** Timer A1 Setup Complete ********/
    /*****************************************************************************************/
    /******TimerA3 Setup for _ Counter*****/

    /* Configures TA2 on 1ms Timer*/
    NVIC->ISER[0] = 1 << ((TA3_0_IRQn) & 31);       //ISR enable for timer
    TIMER_A3->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG; //turns off capture/compare ISR flag
    TIMER_A3->CCTL[0] = TIMER_A_CCTLN_CCIE; // TACCR0 interrupt enabled
    TIMER_A3->CCR[0] = 500; //sets count value to 2
    TIMER_A3->CTL = TIMER_A_CTL_SSEL__ACLK | // Set clock to ACLK, UP mode Count
            TIMER_A_CTL_MC__UP;

    /******** Timer A3 Setup Complete ********/

    /*****************************************************************************************/
    /*Enable masrter interupts*/
    MAP_Interrupt_enableMaster(); //enable ISR
    __enable_irq(); // Enable global interrupt

}

void TA1_0_IRQHandler(void) {
    // Clear the compare interrupt flag

   // Read_Count++;
    carStatus = mphTimeout;
    TIMER_A1->R = 0;
    TIMER_A1->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;

}

void TA3_0_IRQHandler(void) {
    // Clear the compare interrupt flag


        if(carStatus != mphTimeout)
        {
            carStatus = tachTimeout;
        }



   // car_status = 4;
  //  RPM_Calculation();
    //VSS_Calculation(actual_gear);

    TIMER_A3->R = 0;
    TIMER_A3->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
}

void TA2_N_IRQHandler(void) {
 //Clear the compare interrupt flag
 TIMER_A2->CCTL[1] &= ~(TIMER_A_CCTLN_CCIFG);
 Pulse_Count_VSS++;

 // Clear the interrupt flag
 // TIMER_A0->CCTL[2] &= ~(TIMER_A_CCTLN_CCIFG);
 //add counter here

 TIMER_A2->R = 0;
 }

// Timer A0 ISR for input Capture of RPM & VSS
void TA0_N_IRQHandler(void) {
    volatile uint32_t i;
   // Input_Capt_ISR_Status = TIMER_A0->IV;
    TIMER_A0->CCTL[2] &= ~(TIMER_A_CCTLN_CCIFG);
      // TIMER_A0->CCTL[2] &= ~(TIMER_A_CCTLN_CCIFG);

   // if (Input_Capt_ISR_Status == 4) {

        //RPM_Capt_Values[RPM_Capt_Pointer] = TIMER_A0->CCR[1];
        //RPM_Capt_Pointer++;
        Pulse_Count_RPM++;

       /* if (RPM_Capt_Pointer == 2) {
            car_status = 4;
            RPM_Capt_Pointer = 0;
        }*/
     /*else if (Input_Capt_ISR_Status == 4) {
        VSS_Capt_Values[VSS_Capt_Pointer] = TIMER_A0->CCR[2];
        VSS_Capt_Pointer++;
        Pulse_Count_VSS++;

        /*if (VSS_Capt_Pointer == 2) {
            //calculate vss
            car_status = 5;
            VSS_Capt_Pointer = 0;*/
      //  }


    // Clear the interrupt flag
    TIMER_A0->R = 0;

    Input_Capt_ISR_Status = 0;
}

/*Self Explanitory*/
void Enable_A1_Timeout() {
    TIMER_A1->CCR[0] = 2;

}
void Disable_A1_Timeout() {
    TIMER_A1->CCR[0] = 0;
    //shifter_counter=0;
}
void Enable_A2_Timeout() {
    TIMER_A2->CCR[0] = 2;
}
void Disable_A2_Timeout() {
    TIMER_A2->CCR[0] = 0;
}
void Clear_A1_Timeout() {
    TIMER_A1->R = 0;
    //counter = 0;
}
void Clear_A2_Timeout() {
    TIMER_A2->R = 0;
}


void Enable_A3_Timeout() {
    TIMER_A3->CCR[0] = 1000;
}
void Disable_A3_Timeout() {
    TIMER_A3->CCR[0] = 0;
}
void Clear_A3_Timeout() {
    TIMER_A3->R = 0;
    //counter = 0;
}



void Delay_Time(int milliseconds)
{   delay_flag = 1;

    while (delay_counter < milliseconds)
    {

    }
        delay_counter = 0;
        delay_flag=0;
}

