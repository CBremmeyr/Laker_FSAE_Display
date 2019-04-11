/*
 * Flash.c
 *
 *  Created on: Apr 16, 2017
 *      Author: Kevin Mahlie
 */
#include "msp.h"
#include "driverlib/MSP432P4xx/driverlib.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
//#include "ST7735_MOD.h"
#include "System_Control.h"
#include "Flash_custom.h"
#include "Math.h"

#define ATTEMPT_LOG_ADDR  0x0003F000
#define SCHEDULE_ADDR     0x0001f000
#define PIN_START_ADDR    0x0003F090
#define FIRST_RUN_ADDR    0x0003F0A0
#define CALIBRATION_START 0x000200058
#define TRIP_LOG_ADDR     0x0003E000

char currentPin[4];
int* addr_pointer;
volatile int8_t shift_data[7];
extern int actual_gear;
uint32_t read_shift_data[10][5];
extern int expected_gear;
extern float RPM_IN;
extern float MPH_IN;
extern int shift_type_flag;
extern int sucessful_shift_flag;
extern int up_shift_counter;
extern int down_shift_counter;
extern int shift_RPM;
extern int shift_MPH;
int up_count_data = 0;
int down_count_data=0;
// First time setup detection:
// Description: If running first time ever (ie. first run after reprogram) set-up flag
// In: N/A
// Out: Flag for Setup Run
void first_time_run_check(void) {

    char defaultPin[4] = { '1', '1', '1', '1' };

    int16_t status[4];
    *addr_pointer = FIRST_RUN_ADDR;

    int j;

    for (j = 0; j <= 3; j++) {
        status[j] = *addr_pointer++;
    }

    // If first time run is detected, set default pin to 1111
    if (status[0] == -1) {
        // 1st run detect -- set to 1111
        for (j = 0; j < 4; j++) {
            currentPin[j] = '1';
        }

        clear_log_flash_on_first_startup();
        //Commit_Gear_To_Flash(actual_gear);
    }

    write_first_setup_bit();

}

void write_first_setup_bit(void) {

    int8_t firstRun = 10;

    MAP_FlashCtl_unprotectSector(FLASH_MAIN_MEMORY_SPACE_BANK1, FLASH_SECTOR31);

    if (!MAP_FlashCtl_programMemory(firstRun, (void*) FIRST_RUN_ADDR, 4))// change: size 4096 to 4..

        while (1)
            ;

    __no_operation();

    MAP_FlashCtl_protectSector(FLASH_MAIN_MEMORY_SPACE_BANK1, FLASH_SECTOR31);

}

void clear_log_flash_on_first_startup(void) {

    int j;

    int flash_shift_temp[4096]; // temp array for printing in order.. for j..for i.. ect

    // Get current values for each one
    for (j = 0; j < 40; j++) {
        flash_shift_temp[j] = 0;
    }

    MAP_FlashCtl_unprotectSector(FLASH_MAIN_MEMORY_SPACE_BANK1, FLASH_SECTOR31);

    if (!MAP_FlashCtl_programMemory(flash_shift_temp, (void*) ATTEMPT_LOG_ADDR,
            4096))
        while (1)
            ;

    /* Setting the sector back to protected  */
    MAP_FlashCtl_protectSector(FLASH_MAIN_MEMORY_SPACE_BANK1, FLASH_SECTOR31);

    MAP_FlashCtl_unprotectSector(FLASH_MAIN_MEMORY_SPACE_BANK1, FLASH_SECTOR31);

    if (!MAP_FlashCtl_programMemory(flash_shift_temp, (void*) ATTEMPT_LOG_ADDR,
            4096))
        while (1)
            ;

    /* Setting the sector back to protected  */
    MAP_FlashCtl_protectSector(FLASH_MAIN_MEMORY_SPACE_BANK1, FLASH_SECTOR31);

    MAP_FlashCtl_unprotectSector(FLASH_MAIN_MEMORY_SPACE_BANK1, FLASH_SECTOR30);

    /* Trying to erase the sector. Within this function, the API will
     automatically try to erase the maximum number of tries. If it fails,
     trap in an infinite loop */

//  if (!MAP_FlashCtl_eraseSector(ATTEMPT_LOG_ADDR))
//      while (1)
    //      ;
    /* Trying to program the memory. Within this function, the API will
     automatically try to program the maximum number of tries. If it fails,
     trap inside an infinite loop */
    if (!MAP_FlashCtl_programMemory(flash_shift_temp, (void*) TRIP_LOG_ADDR,
            4096))
        while (1)
            ;
    __no_operation();
    /* Setting the sector back to protected  */
    MAP_FlashCtl_protectSector(FLASH_MAIN_MEMORY_SPACE_BANK1, FLASH_SECTOR30);

}

void Commit_Gear_To_Flash(int gear) {

    //char test[4] = {'1','2','3','4'};
    //strcpy(test,newpin);
//  int new_pin[4];
    //char commit_gear;
    uint32_t commit_gear = 0;
    //commit_gear = gear;
    commit_gear = (gear);


    // TODO: ADDRESS THIS!
    // change: size 4096 to 4..

    // Start at beginning of register to get recent 4 only...
    *addr_pointer = CALIBRATION_START;

    MAP_FlashCtl_unprotectSector(FLASH_INFO_MEMORY_SPACE_BANK0, FLASH_SECTOR0);

    //Trying to erase the sector. Within this function, the API will
    // automatically try to erase the maximum number of tries. If it fails,
    // trap in an infinite loop

    //while (!MAP_FlashCtl_eraseSector(CALIBRATION_START))
      //  ;

    if (!MAP_FlashCtl_eraseSector(ATTEMPT_LOG_ADDR))
      while (1);

    //Trying to program the memory. Within this function, the API will
    // automatically try to program the maximum number of tries. If it f.
    // ails,
    // trap inside an infinite loop

    while (!MAP_FlashCtl_programMemory(commit_gear, (void*) CALIBRATION_START, 1))
        ;
    // Note: leave first byte unprogrammed
    /* Setting the sector back to protected */
    __no_operation();
    // Setting the sector back to protected
    MAP_FlashCtl_protectSector(FLASH_INFO_MEMORY_SPACE_BANK0, FLASH_SECTOR0);

}

void Log_Counter_Data(void) {

    int commitarray[2];
    commitarray[0] = up_shift_counter;
    commitarray[1] = down_shift_counter;

    __no_operation();

    MAP_FlashCtl_unprotectSector(FLASH_MAIN_MEMORY_SPACE_BANK0, FLASH_SECTOR31);

    while (!MAP_FlashCtl_eraseSector(SCHEDULE_ADDR))
        ;

    while (!MAP_FlashCtl_programMemory(commitarray, (void*) SCHEDULE_ADDR, 32))
        ;

    MAP_FlashCtl_protectSector(FLASH_MAIN_MEMORY_SPACE_BANK0, FLASH_SECTOR31);

}

void recall_pin(void) {

    int32_t pin;
    addr_pointer = CALIBRATION_START;

    int j;

//  for (j = 0; j <= 3; j++) {
    pin = *addr_pointer;
    currentPin[3] = ((pin >> 24) & (0b1111)) + '0';
    currentPin[2] = (pin >> 16) & (0b1111) + '0';
    currentPin[1] = (pin >> 8) & (0b1111) + '0';
    currentPin[0] = (pin) & (0b1111) + '0';

}


void Log_Shift_Data(void) {

    int i;
    int j;

///    read_RTC(); // get exact current time
    int Shift_data[5];
    // Extract recent 4 reads, shift them down. add new one
    int flash_shift[10][5];
    int flash_shift2[10][5];
    int flash_shift_temp[4096]; // temp array for printing in order.. for j..for i.. ect

    //Read_Inputs();

    Shift_data[0] = shift_type_flag;
    Shift_data[1] = expected_gear;
    Shift_data[2] = shift_RPM;
    Shift_data[3] = shift_MPH;
    Shift_data[4] = sucessful_shift_flag;

    // Start at beginning of register to get recent 4 only...
    addr_pointer = ATTEMPT_LOG_ADDR;

    // Get current values for each one
    for (j = 0; j < 10; j++) {
        // Read from flash memory location
        for (i = 0; i < 5; i++) // read values in flash after programming
                {
            flash_shift[j][i] = *addr_pointer++;
        }
    }

    // Shift current indices up one.. start from bottom up
    for (j = 1; j < 10; j++) {
        // Read from flash memory location
        for (i = 0; i < 5; i++) // read values in flash after programming
                {
            flash_shift2[j][i] = flash_shift[j - 1][i];
        }
    }

    // Get current time.. set it to the existing 0 index
    for (i = 0; i < 5; i++) {
        // Global variable that is printed to flash, offset by number of logs present. Resets after 5 to begin writing at position 1
        flash_shift2[0][i] = Shift_data[i];
    }
    /*if (armed == 1)
    {
    flash_shift2[0][7] = 1;
    }
    if (armed == 0)
    {
        flash_shift2[0][7] = 2;
    }*/

    // Plug into a large array...
    int index = 0;

    for (j = 0; j < 10; j++) {
        for (i = 0; i < 5; i++) {
            flash_shift_temp[index] = flash_shift2[j][i];
            index++;
        }
    }

    MAP_FlashCtl_unprotectSector(FLASH_MAIN_MEMORY_SPACE_BANK1, FLASH_SECTOR31);

    /* Trying to erase the sector. Within this function, the API will
     automatically try to erase the maximum number of tries. If it fails,
     trap in an infinite loop */

    if (!MAP_FlashCtl_eraseSector(ATTEMPT_LOG_ADDR))
        while (1)
            ;

    /* Trying to program the memory. Within this function, the API will
     automatically try to program the maximum number of tries. If it fails,
     trap inside an infinite loop */
    if (!MAP_FlashCtl_programMemory(flash_shift_temp, (void*) ATTEMPT_LOG_ADDR,
            4096))
        while (1)
            ;
    __no_operation();
    /* Setting the sector back to protected  */
    MAP_FlashCtl_protectSector(FLASH_MAIN_MEMORY_SPACE_BANK1, FLASH_SECTOR31);

}

void Read_Shift_Data_From_Flash(void)
{
    int i,j;

    addr_pointer = ATTEMPT_LOG_ADDR;

    for (j=0; j <= 9; j++)
    {
        for (i=0; i<=4; i++)
        {
            read_shift_data[j][i]=*addr_pointer++;
        }
    }

}


void Read_Counter_Data(void)
{
    int i,j;

    addr_pointer = SCHEDULE_ADDR;


            up_count_data=*addr_pointer;
            down_count_data = *addr_pointer++;


}

