/*
 * Inputs.h
 *
 *  Created on: Feb 12, 2017
 *      Author: Kevin Mahlie
 */

/* Defines */
#define UPSHIFT_IN GPIO_PIN4
#define DOWNSHIFT_IN GPIO_PIN6
#define SHIFTER_PORT GPIO_PORT_P2
#define UPSHIFT_READ   BIT4
#define DOWNSHIFT_READ BIT6

#define FIND_NUETRAL_IN GPIO_PIN2
#define FIND_NUETRAL_READ BIT2
#define NUETRAL_PORT GPIO_PORT_P3
#define NUETRAL_IN GPIO_PIN0
#define NUETRAL_READ BIT0

#define OIL_PORT GPIO_PORT_P1
#define OIL_IN GPIO_PIN7
#define OIL_READ BIT7


void Shifter_Input_Setup(void);
void PortInit ();
void Nuetral_Inputs_Setup();
uint16_t ReadBtnUpShift(void);
void DebounceUpShift(void);
uint16_t ReadBtnDownShift(void);
void DebounceDownShift(void);
uint16_t ReadBtnFindN(void);
void DebounceFindNuetral(void);
uint16_t ReadBtnNuetralIn(void);
void DebounceNuetralIn(void);

void RPM_VSS_Input_Setup();
void CheckSystemInputs();
void OIL_Pressure_Setup(void);
