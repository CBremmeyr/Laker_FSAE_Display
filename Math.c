/*
 * Math.c
 *
 *  Created on: Apr 14, 2017
 *      Author: Kevin Mahlie
 */

#include "Timing.h"
#include "driverlib/MSP432P4xx/driverlib.h"

/* Standard Includes */
#include <stdint.h>

#include <stdbool.h>

#define NUMBER_SPROCKET_TEETH 47
#define NUMBER_ENGINE_TEETH 12
#define DIFF_RATIO 3.91667
#define METER_TO_MILE 0.000621371
#define MILLI_SEC_TO_HOUR 36000
#define REV_PER_MILE 978.17
#define VSS_GEAR_RATIO  28/23
#define PRIMARY_GEAR_RATIO 2.11
#define PULSES_PER_REV 8
#define UPPER 0.5
#define LOWER 0.5

int Active_Gear_Ratio[] = { 2.66, 1.933, 1.611, 1.409, 1.26 };


float VSS_Revolution = 0;
extern float Pulse_Count_RPM;
extern float Pulse_Count_VSS;
extern float MPH_IN;
extern float RPM_IN;
extern int Calc_Gear;
extern int Calc_RPM;
extern int Read_Count;
extern int printFlag;

float tempRPM;

void Read_Inputs() {

    VSS_Revolution = (Pulse_Count_VSS); // /28
    MPH_IN = VSS_Revolution / DIFF_RATIO / REV_PER_MILE * 1800;

    if(printFlag == 1){

        RPM_IN = tempRPM;
    }

    else
    {
        RPM_IN = ((Pulse_Count_RPM / PULSES_PER_REV) * 240);
    }
    tempRPM = RPM_IN;
    printFlag = 0;
    Pulse_Count_RPM = 0;
    Pulse_Count_VSS = 0;
}

int Calculate_VSS_TO_RPM(int gear) {

    Calc_RPM = MPH_IN/Active_Gear_Ratio[gear]/PRIMARY_GEAR_RATIO/REV_PER_MILE*60;
    return Calc_RPM;
}


int Calculate_Gear(int gear) {

    // calculated_gear_ratio =
    int Calc_Gear_Ratio = 0;
    Calc_Gear_Ratio = RPM_IN /MPH_IN/Active_Gear_Ratio[gear]/ PRIMARY_GEAR_RATIO/REV_PER_MILE*60;

    if (Calc_Gear_Ratio > (Active_Gear_Ratio[0] - LOWER) && Calc_Gear_Ratio > (Active_Gear_Ratio[0] + UPPER))
            {
                Calc_Gear = 1;
            }

    else if (Calc_Gear_Ratio > (Active_Gear_Ratio[1] - LOWER) && Calc_Gear_Ratio > (Active_Gear_Ratio[1] + UPPER))
                {
                    Calc_Gear = 2;
                }

    else if (Calc_Gear_Ratio > (Active_Gear_Ratio[2] - LOWER) && Calc_Gear_Ratio > (Active_Gear_Ratio[2] + UPPER))
                {
                    Calc_Gear = 3;
                }

    else if (Calc_Gear_Ratio > (Active_Gear_Ratio[3] - LOWER) && Calc_Gear_Ratio > (Active_Gear_Ratio[3] + UPPER))
                {
                    Calc_Gear = 4;
                }

    else if (Calc_Gear_Ratio > (Active_Gear_Ratio[4] - LOWER) && Calc_Gear_Ratio > (Active_Gear_Ratio[4] + UPPER))
                {
                    Calc_Gear = 5;
                }

    return Calc_Gear;
}
