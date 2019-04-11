/*
 * Timing.h
 *
 *  Created on: Feb 15, 2017
 *      Author: Kevin Mahlie
 */

void Clock_Init(void);
void Timer_Setup(void);

void Enable_A1_Timeout();
void Disable_A1_Timeout();
void Enable_A2_Timeout();
void Disable_A2_Timeout();
void Clear_A1_Timeout();
void Clear_A2_Timeout();

void Enable_A3_Timeout();
void Disable_A3_Timeout();
void Clear_A3_Timeout();
void Delay_Time(int milliseconds);

//#define NUMBER_TIMER_CAPTURES        20;


