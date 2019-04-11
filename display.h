/*
 * display.h
 *
 *  Created on: Apr 26, 2018
 *      Author: Chris Pryslak
 *      Description: Header file that accompanies the LCD display file
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_


void updateTACH();
// Function used to manage the tach bar

void initSCREEN();
// Function used to initiate the screen and print initial setup

void updateSCREEN();
// Function used to update the gear, RPM, speed and sensor outputs

void Print_Gear(int gear);
/* Function to update the gear on the screen
    -Input the current gear of the system
    -Possible inputs are N and 1 - 5
*/

void printLOG();
// Function used to print log data to screen

void displayCarSystemChecks();
/* Function display the engine system signals including:
    -Neutral Light: Active High
    -Oil Pressure Light: Active High
*/

#endif /* DISPLAY_H_ */
