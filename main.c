#include "System_Control.h"

#include "stdint.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "math.h"

#include "msp.h"
#include "msp432p401r.h"

int car_status = 0;
int Calc_Gear = 0;
int Calc_RPM = 0;
int shifter_counter = 0;
int clutch_counter = 0;

//int Calc_Gear = 0;
int clutch_flag = 0;
int shift_flag = 0;
int delay_flag = 0;
int actual_gear = 0;
int shifting_flag = 0;
int print_nuetral_flag = 0;
int log_data_flag = 0;
int up_shift_counter = 0;
int down_shift_counter = 0;
int dont_error_check_flag = 0;
int expected_gear = 0;
int shift_request = 0;

int sucessful_shift_flag = 0;
int print_flag = 1;
int shift_type_flag = 0;

extern int Shifter_Status[3];
extern int Nuetral_IN;
extern int Read_Count;
extern int up_count_data;
extern int down_count_data;

/* variables for calculating input VSS and RPM*/

extern float Pulse_Count_RPM;
extern float Pulse_Count_VSS;
float MPH_IN = 0;
float RPM_IN = 0;
extern int Oil_IN;
extern int Find_Nuetral_IN;

/* variables for data storage*/
extern int up_count_data;
extern int down_count_data;
float shift_RPM = 0;
float shift_MPH = 0;

int printFlag = 0;
int secCounter = 0;

int printLogFlag = 0;

/*debugging variables*/
uint16_t state_Up = 0, state_d = 0;

int main(void)
{
    WDT_A_holdTimer(); //WDT not currently implemented
    FPU_enableModule();

    /***********************      Setup Car       *************************************/
    // first_time_run_check();
    /*Time Setup*/
    Clock_Init();
    Timer_Setup();
    //Enable_A3_Timeout();

    /*Reads clock values*/
    clocks.MCLKfreq = CS_getMCLK();
    clocks.SMCLKfreq = CS_getSMCLK();
    clocks.HSMCLKfreq = CS_getHSMCLK();
    clocks.ACLKfreq = CS_getACLK();

    /*Inputs Setup*/
    RPM_VSS_Input_Setup(); //setup input capture for RPM and VSS
    OIL_Pressure_Setup();
    Nuetral_Inputs_Setup();

    /*Outputs Setup*/
    initSCREEN();

    /*FLASH - DATA LOG - Remember Information about last start*/
    //Read_Shift_Data_From_Flash();
    //Read_Counter_Data();

    /* Check if car is in nuetral or not */
    CheckSystemInputs();

    if (Nuetral_IN == 0)
    {
        gearCheck.expectedNextGear = 0;
    }

    else if (Nuetral_IN == 1)
    {
        gearCheck.expectedNextGear = 1;
    }

    Print_Gear(gearCheck.expectedNextGear);
    displayCarSystemChecks();

    /*************************************************************************************/

    // Main Loop
    while (1)
    {

        /**************************  Debugging Purposes    ****************************************/
        CheckSystemInputs();
        /*************************************************************************************/

        /***********************       State of the Car       *************************************/
        switch (carStatus)
        {

        case idle:
            // IDLE case
            break;


        case tachTimeout:
            // Case to read in RPM data and update LCD tach bar
            Read_Inputs();
            updateTACH();
            carStatus = idle;
            break;

        case mphTimeout:
            Read_Inputs();
            printFlag = 1;
            updateSCREEN();
            secCounter++;

            if (secCounter == 10)
            {
                displayCarSystemChecks();

                if (Nuetral_IN == 0)
                {
                    gearCheck.expectedNextGear = 0;
                }

                else if (Nuetral_IN == 1)
                {
                    gearCheck.expectedNextGear = 1;
                }

                Print_Gear(gearCheck.expectedNextGear);
                secCounter = 0;
            }
            carStatus = idle;

            break;

        case displayingDataLogger:

            Find_Nuetral_IN = GPIO_getInputPinValue(NUETRAL_PORT,
                                                    FIND_NUETRAL_IN);
            Nuetral_IN = GPIO_getInputPinValue(NUETRAL_PORT, NUETRAL_IN);

            while (Find_Nuetral_IN == 0 && Nuetral_IN == 0)
            {
                Find_Nuetral_IN = GPIO_getInputPinValue(NUETRAL_PORT,
                                                        FIND_NUETRAL_IN);
                Nuetral_IN = GPIO_getInputPinValue(NUETRAL_PORT, NUETRAL_IN);

                if (printLogFlag == 0)
                {
                    printLOG();
                    printLogFlag = 1;
                }
            }
            printLogFlag = 0;
            carStatus = idle;

            break;

            /*************************************************************************************/

        }
    }

}

int Compare_Gears()
{
    Calc_Gear = expected_gear;

//Calculate_Gear();

    if (Calc_Gear == expected_gear)
    {
        actual_gear = Calc_Gear;
        Print_Gear(actual_gear);
        //Commit_Gear_To_Flash(actual_gear);
        //sucessful_shift_flag = 1;
        Log_Shift_Data();
        Log_Counter_Data();
        return 1;
    }

    if (Calc_Gear != expected_gear)
    {
        //Determine last known gear
        //TODO: retry shift
        // sucessful_shift_flag = 0;
        Log_Shift_Data();
        Log_Counter_Data();
        return 0;
    }
}
