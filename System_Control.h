/*
 * System_Control.h
 *
 *  Created on: Feb 26, 2017
 *      Author: Kevin Mahlie
 */

#include "driverlib/MSP432P4xx/driverlib.h"

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include "msp.h"

#include "Inputs.h"
#include "Outputs.h"
#include "Timing.h"
#include <Flash_custom.h>
#include "display.h"
#include "math.h"

#define UPPER 0.25
#define LOWER 0.25
#define TRUE 1
#define FALSE 0

typedef struct
{
    uint16_t clutchFlag;
    uint16_t shiftRequestAccepted;
    uint16_t printFlag;

} SystemFlags;

typedef struct
{
    uint32_t currentGear;
    int expectedNextGear;
    uint32_t CalculatedGear;
} Gears;

typedef struct
{
    uint16_t OilPressure;
    uint16_t NuetralLight;
    uint16_t button1;
    uint16_t button2;

} SystemInputs;

typedef struct
{
    float MPH_IN;
    float RPM_IN;
    float Pulse_Count_RPM;
    float Pulse_Count_VSS;
} InputCaptures;

typedef struct
{
    uint32_t MCLKfreq;
    uint32_t SMCLKfreq;
    uint32_t HSMCLKfreq;
    uint32_t ACLKfreq;

} SystemClocks;

Gears gearCheck;
SystemFlags controlFlags;
SystemInputs Sensors;
InputCaptures rpmVssICC;
SystemClocks clocks;

enum
{
    idle,
    tachTimeout,
    findingNuetral,
    displayingDataLogger,
    mphTimeout,
    gearChange

} carStatus;

int Compare_Gears();


