/*
 * Flash.h
 *
 *  Created on: Apr 16, 2017
 *      Author: Kevin Mahlie
 */
void Log_Counter_Data(void);
void Read_Counter_Data(void);
void first_time_run_check(void);

void write_first_setup_bit(void);
void clear_log_flash_on_first_startup(void);

void Commit_Gear_To_Flash(int gear);

// Recall flash from memory:
// Description: Read the current pin state, return it for login attempts...
// In: N/A
// Out: Updates global "currentPin"
void recall_pin(void);
void Log_Shift_Data(void);
void Log_Shift_Data(void);
void Read_Shift_Data_From_Flash(void);
