/*
 * Output.c
 *
 *  Created on: Feb 19, 2017
 *      Author: Kevin Mahlie
 */
/* DriverLib Includes */
#include "driverlib/MSP432P4xx/driverlib.h"

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>

/* Local Includes */
#include "Outputs.h"

#define PNEUM_PORT GPIO_PORT_P6
#define PNEUM_UP   GPIO_PIN7
#define PNEUM_DW   GPIO_PIN6
#define PNEUM_CL   GPIO_PIN7

#define FUEL_SWITCH_PORT GPIO_PORT_P3
#define FUEL_SWITCH_PIN  GPIO_PIN0


/*********************************
 * Function:Pnuematic_Setup
 * This function sets up GPIO Outputs
 * to send the drive signal to the
 * pnuematic relays
 * Note: Keep all Pnuem on the same
 * Pnuem port.
 ********************************/
void Pnuematic_Setup() {

    //Setup up-shift pnuematic
    MAP_GPIO_setAsOutputPin(PNEUM_PORT, PNEUM_UP);
    GPIO_setOutputLowOnPin(PNEUM_PORT, PNEUM_UP);

    //Setup down-shift pnuematic
    MAP_GPIO_setAsOutputPin(PNEUM_PORT, PNEUM_DW);
    GPIO_setOutputLowOnPin(PNEUM_PORT, PNEUM_DW);

    //Setup clutch-shift pnuematic
    //MAP_GPIO_setAsOutputPin(PNEUM_PORT, PNEUM_CL);
    //GPIO_setOutputLowOnPin(PNEUM_PORT, PNEUM_CL);

}
/*********************************
 * Function:Fuel_Swith_Setup
 * This function sets up GPIO Output
 * for the fuel switch kill signal.
 ********************************/
void Fuel_Switch_Setup() {

    MAP_GPIO_setAsOutputPin(FUEL_SWITCH_PORT, FUEL_SWITCH_PIN);
    GPIO_setOutputLowOnPin(FUEL_SWITCH_PORT, FUEL_SWITCH_PIN);
}

/*
void Tach_Display_Setup()
{
    // Configure GPIO as output compare
    P2->DIR |= (BIT4);
    P2->SEL0 |= (BIT4);
    P2->SEL1 &= ~(BIT4);

    TIMER_A0->CCR[0] = 33000 - 1; // PWM Period
    TIMER_A0->CCTL[1] = TIMER_A_CCTLN_OUTMOD_7; // CCR1 as reset/set mode
    TIMER_A0->CCR[1] = 0;  // PWM duty cycle
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__UP | TIMER_A_CTL_CLR; // SMCLK UP mode clear TAR
}
*/


/*Self Explanatory*/
void Pnuem_UP_Engage(){

    GPIO_setOutputHighOnPin(PNEUM_PORT, PNEUM_UP);
}

void Pnuem_UP_Disengage(){

    GPIO_setOutputLowOnPin(PNEUM_PORT, PNEUM_UP);
}

void Pnuem_DW_Engage(){

    GPIO_setOutputHighOnPin(PNEUM_PORT, PNEUM_DW);
}

void Pnuem_DW_Disengage(){

    GPIO_setOutputLowOnPin(PNEUM_PORT, PNEUM_DW);
}
void Pnuem_CL_Engage(){

    GPIO_setOutputHighOnPin(PNEUM_PORT, PNEUM_CL);
}

void Pnuem_CL_Disengage(){

    GPIO_setOutputLowOnPin(PNEUM_PORT, PNEUM_CL);
}

