// Author: Chris Pryslak
// Date: 2/10/2018

/* DriverLib Includes */
#include "driverlib/MSP432P4xx/driverlib.h"

/* Standard Includes */
#include <stdint.h>
#include <stdio.h>

/* GrLib Includes */
#include "GrLib/grlib/grlib.h"
#include "LcdDriver/kitronix320x240x16_ssd2119_spi.h"
#include "images/images.h"
#include "Inputs.h"
#include "display.h"

// Printing variables (inputs from main code)
int RPM = 12000;        // max RPM is around 10.5 thousand
int speed = 12;         // speed under 100
int8_t RPMc[6];         // rpm character form
int8_t Speedc[3];       // speed
int gear = 0;           // max gear is ______?
extern int Oil_IN;
extern int Nuetral_IN;
extern float MPH_IN;
extern float RPM_IN;
extern float Pulse_Count_RPM;
extern float Pulse_Count_VSS;
extern int printFlag;

// Graphic library context
Graphics_Context g_sContext;

// Function Call-Outs
void Delay(uint16_t msec);
void clockInit(void);           // used to initialize 48 MHz timer

void initSCREEN()
{
    // LCD setup using Graphics Library API calls
    Kitronix320x240x16_SSD2119Init();
    Graphics_initContext(&g_sContext, &g_sKitronix320x240x16_SSD2119);

    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_clearDisplay(&g_sContext);
    Graphics_setFont(&g_sContext, &g_sFontCm38b);

    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLUE);

    Graphics_drawStringCentered(&g_sContext, "RPM",
    AUTO_STRING_LENGTH,
                                60, 100,
                                TRANSPARENT_TEXT);

    Graphics_drawStringCentered(&g_sContext, "MPH",
    AUTO_STRING_LENGTH,
                                270, 100,
                                TRANSPARENT_TEXT);

    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);

    // following draws the line dividers
    Graphics_drawLine(&g_sContext, 0, 52, 320, 52);
    Graphics_drawLine(&g_sContext, 0, 192, 320, 192);
    Graphics_drawLine(&g_sContext, 129, 52, 129, 192);
    Graphics_drawLine(&g_sContext, 211, 52, 211, 192);

    Graphics_setFont(&g_sContext, &g_sFontCm20b);

    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLUE);

    Graphics_drawString(&g_sContext, "Oil Pressure",
    AUTO_STRING_LENGTH,
                        0, 195,
                        TRANSPARENT_TEXT);

    Graphics_drawString(&g_sContext, "Neutral Light",
    AUTO_STRING_LENGTH,
                        0, 220,
                        TRANSPARENT_TEXT);

    // Following section prints the initial neutral light
    Graphics_drawImage(&g_sContext, &N1BPP_UNCOMP, 130, 80);

}
void updateSCREEN()
{
    char RPM_display[6];
    char MPH_display[3];

    //Following section is used to update the RPM, calculated speed and current gear
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_Rectangle FillerRect1 = { 0, 135, 128, 190 }; // used to cover and change RPM when updating
    Graphics_Rectangle FillerRect2 = { 212, 135, 320, 190 }; // used to cover and change MPH when updating
    Graphics_fillRectangle(&g_sContext, &FillerRect1);
    Graphics_fillRectangle(&g_sContext, &FillerRect2);
    //////////////////////////////////////////////////////////////////////////////////////////
    // Following section is used to print the RPM and Speed

    sprintf(RPMc, "%5.0f", RPM_IN);
    sprintf(Speedc, "%2.0f", MPH_IN);

    if (RPMc[4] == '.')
    {
        RPM_display[0] = RPMc[0];
        RPM_display[1] = RPMc[1];
        RPM_display[2] = RPMc[2];
        RPM_display[3] = RPMc[3];
        RPM_display[4] = '\0';
        RPM_display[5] = '\0';
    }
    else
    {
        RPM_display[0] = RPMc[0];
        RPM_display[1] = RPMc[1];
        RPM_display[2] = RPMc[2];
        RPM_display[3] = RPMc[3];
        RPM_display[4] = RPMc[4];
        RPM_display[5] = '\0';
    }

      MPH_display[0] = Speedc[0];
      MPH_display[1]= Speedc[1];
      MPH_display[2] = '\0';
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    Graphics_setFont(&g_sContext, &g_sFontCmtt48);

    Graphics_drawStringCentered(&g_sContext, RPM_display,
    AUTO_STRING_LENGTH,
                                60, 150,
                                TRANSPARENT_TEXT);

     Graphics_drawStringCentered(&g_sContext, MPH_display,
                                 AUTO_STRING_LENGTH,
                                 270,
                                 150,
                                 TRANSPARENT_TEXT);
    //////////////////////////////////////////////////////////////////////////////////////////
    // Following is used to display the gear
    //Print_Gear(gear);
    //////////////////////////////////////////////////////////////////////////////////////////
    // Following section is used to update the status of the car

    /*if (Oil_IN == 1){
     Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
     Graphics_fillCircle(&g_sContext, 300, 205, 7);
     }else if(Oil_IN == 0){
     Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
     Graphics_fillCircle(&g_sContext, 300, 205, 7);
     }

     if (Neutral_Flag == 1){
     Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
     Graphics_fillCircle(&g_sContext, 300, 230, 7);
     }else if(Neutral_Flag == 0){
     Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
     Graphics_fillCircle(&g_sContext, 300, 230, 7);
     }*/
}

void displayCarSystemChecks()
{
    // Following section is used to update the status of the car

    Nuetral_IN = GPIO_getInputPinValue(NUETRAL_PORT, NUETRAL_IN);
    Oil_IN = GPIO_getInputPinValue(OIL_PORT, OIL_IN);
    if (Oil_IN == 1)
    {
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
        Graphics_fillCircle(&g_sContext, 300, 205, 7);
    }
    else if (Oil_IN == 0)
    {
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
        Graphics_fillCircle(&g_sContext, 300, 205, 7);
    }

    if (Nuetral_IN == 0)
    {
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
        Graphics_fillCircle(&g_sContext, 300, 230, 7);
    }
    else if (Nuetral_IN == 1)
    {
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
        Graphics_fillCircle(&g_sContext, 300, 230, 7);
    }
}
void updateTACH(void)
{
    // section for Tack Bar
    // green section
    Graphics_Rectangle myRectangle1 = { 0, 0, 23, 50 };
    Graphics_Rectangle myRectangle2 = { 27, 0, 50, 50 };
    Graphics_Rectangle myRectangle3 = { 54, 0, 77, 50 };
    // yellow section
    Graphics_Rectangle myRectangle4 = { 81, 0, 104, 50 };
    Graphics_Rectangle myRectangle5 = { 108, 0, 131, 50 };
    Graphics_Rectangle myRectangle6 = { 135, 0, 158, 50 };
    // orange section
    Graphics_Rectangle myRectangle7 = { 162, 0, 185, 50 };
    Graphics_Rectangle myRectangle8 = { 189, 0, 212, 50 };
    Graphics_Rectangle myRectangle9 = { 216, 0, 239, 50 };
    // red section
    Graphics_Rectangle myRectangle10 = { 243, 0, 266, 50 };
    Graphics_Rectangle myRectangle11 = { 270, 0, 293, 50 };
    Graphics_Rectangle myRectangle12 = { 297, 0, 320, 50 };

    // based on the RPM the TACH bar will update
    // 1000 RPM per bar
    if (RPM_IN >= 1000)
    {
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
        Graphics_fillRectangle(&g_sContext, &myRectangle1);
    }
    else
    {
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
        Graphics_fillRectangle(&g_sContext, &myRectangle1);
    }

    if (RPM_IN >= 2000)
    {
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
        Graphics_fillRectangle(&g_sContext, &myRectangle2);
    }
    else
    {
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
        Graphics_fillRectangle(&g_sContext, &myRectangle2);
    }

    if (RPM_IN >= 3000)
    {
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
        Graphics_fillRectangle(&g_sContext, &myRectangle3);
    }
    else
    {
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
        Graphics_fillRectangle(&g_sContext, &myRectangle3);
    }

    if (RPM_IN >= 4000)
    {
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_YELLOW);
        Graphics_fillRectangle(&g_sContext, &myRectangle4);
    }
    else
    {
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
        Graphics_fillRectangle(&g_sContext, &myRectangle4);
    }

    if (RPM_IN >= 5000)
    {
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_YELLOW);
        Graphics_fillRectangle(&g_sContext, &myRectangle5);
    }
    else
    {
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
        Graphics_fillRectangle(&g_sContext, &myRectangle5);
    }

    if (RPM_IN >= 6000)
    {
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_YELLOW);
        Graphics_fillRectangle(&g_sContext, &myRectangle6);
    }
    else
    {
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
        Graphics_fillRectangle(&g_sContext, &myRectangle6);
    }

    if (RPM_IN >= 7000)
    {
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_ORANGE);
        Graphics_fillRectangle(&g_sContext, &myRectangle7);
    }
    else
    {
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
        Graphics_fillRectangle(&g_sContext, &myRectangle7);
    }

    if (RPM_IN >= 8000)
    {
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_ORANGE);
        Graphics_fillRectangle(&g_sContext, &myRectangle8);
    }
    else
    {
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
        Graphics_fillRectangle(&g_sContext, &myRectangle8);
    }

    if (RPM_IN >= 9000)
    {
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_ORANGE);
        Graphics_fillRectangle(&g_sContext, &myRectangle9);
    }
    else
    {
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
        Graphics_fillRectangle(&g_sContext, &myRectangle9);
    }

    if (RPM_IN >= 10000)
    {
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
        Graphics_fillRectangle(&g_sContext, &myRectangle10);
    }
    else
    {
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
        Graphics_fillRectangle(&g_sContext, &myRectangle10);
    }

    if (RPM_IN >= 11000)
    {
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
        Graphics_fillRectangle(&g_sContext, &myRectangle11);
    }
    else
    {
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
        Graphics_fillRectangle(&g_sContext, &myRectangle11);
    }

    if (RPM_IN >= 12000)
    {
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
        Graphics_fillRectangle(&g_sContext, &myRectangle12);
    }
    else
    {
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
        Graphics_fillRectangle(&g_sContext, &myRectangle12);
    }

}

// Function used to update the gear on the screen
void Print_Gear(int gear)
{
    if (gear == 0)
    {
        Graphics_drawImage(&g_sContext, &N1BPP_UNCOMP, 130, 80);
    }

    else
    {

        Graphics_drawImage(&g_sContext, &Gear1BPP_UNCOMP, 130, 80);
    }
   /* else if (gear == 1)
    {
        Graphics_drawImage(&g_sContext, &one1BPP_UNCOMP, 130, 80);
    }
    else if (gear == 2)
    {
        Graphics_drawImage(&g_sContext, &two1BPP_UNCOMP, 130, 80);
    }
    else if (gear == 3)
    {
        Graphics_drawImage(&g_sContext, &three1BPP_UNCOMP, 130, 80);
    }
    else if (gear == 4)
    {
        Graphics_drawImage(&g_sContext, &four1BPP_UNCOMP, 130, 80);
    }
    else if (gear == 5)
    {
        Graphics_drawImage(&g_sContext, &five1BPP_UNCOMP, 130, 80);
    }*/

}
void printLOG()
{
    Graphics_clearDisplay(&g_sContext); // clears display

    /////////////////////////////////////////////////////////////////////////////////////
    // Following section sets up menu structure
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLUE);
    Graphics_setFont(&g_sContext, &g_sFontCm30b);
    Graphics_drawStringCentered(&g_sContext, "LOG MENU",
    AUTO_STRING_LENGTH,
                                160, 15,
                                TRANSPARENT_TEXT);
    Graphics_setFont(&g_sContext, &g_sFontCm22b);
    Graphics_drawStringCentered(&g_sContext, "Shift",
    AUTO_STRING_LENGTH,
                                32, 45,
                                TRANSPARENT_TEXT);
    Graphics_drawStringCentered(&g_sContext, "Gear",
    AUTO_STRING_LENGTH,
                                96, 45,
                                TRANSPARENT_TEXT);
    Graphics_drawStringCentered(&g_sContext, "RPM",
    AUTO_STRING_LENGTH,
                                160, 45,
                                TRANSPARENT_TEXT);
    Graphics_drawStringCentered(&g_sContext, "MPH",
    AUTO_STRING_LENGTH,
                                224, 45,
                                TRANSPARENT_TEXT);
    Graphics_drawStringCentered(&g_sContext, "State",
    AUTO_STRING_LENGTH,
                                288, 45,
                                TRANSPARENT_TEXT);

    // following draws the line dividers
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    Graphics_drawLine(&g_sContext, 0, 30, 320, 30);
    Graphics_drawLine(&g_sContext, 0, 60, 320, 60);
    Graphics_drawLine(&g_sContext, 0, 96, 320, 96);
    Graphics_drawLine(&g_sContext, 0, 132, 320, 132);
    Graphics_drawLine(&g_sContext, 0, 168, 320, 168);
    Graphics_drawLine(&g_sContext, 0, 204, 320, 204);
    Graphics_drawLine(&g_sContext, 0, 239, 320, 239);

    Graphics_drawLine(&g_sContext, 0, 30, 0, 240);
    Graphics_drawLine(&g_sContext, 64, 30, 64, 240);
    Graphics_drawLine(&g_sContext, 128, 30, 128, 240);
    Graphics_drawLine(&g_sContext, 192, 30, 192, 240);
    Graphics_drawLine(&g_sContext, 256, 30, 256, 240);
    Graphics_drawLine(&g_sContext, 319, 30, 319, 240);

    /////////////////////////////////////////////////////////////////////////////////////
    // Following section displays the last 5 logs
    int temp_data[5] = { 1, 4, 7550, 55, 1 };
    int read_shift_data[10][5];
    int8_t tempC[1];
    int i;
    int j;
    int y_dist = 78;

    for (i = 0; i <= 4; i++)
    {
        for (j = 0; j <= 4; j++)
        {
            read_shift_data[i][j] = temp_data[j];
        }
    }

    for (j = 0; j <= 4; j++)
    {
        for (i = 0; i <= 4; i++)
        {
            // Switch statement to deal with five individual sections
            switch (i)
            {

            // Case to deal with whether the gear was up shift or a downshift
            case 0:
                if (read_shift_data[j][0] == 0)
                {
                    Graphics_drawStringCentered(&g_sContext, "UP",
                    AUTO_STRING_LENGTH,
                                                32, y_dist,
                                                TRANSPARENT_TEXT);
                }

                else if (read_shift_data[j][0] == 1)
                {
                    Graphics_drawStringCentered(&g_sContext, "DW",
                    AUTO_STRING_LENGTH,
                                                32, y_dist,
                                                TRANSPARENT_TEXT);
                }
                break;
                // Case to deal with what dear was being shifted at
            case 1:
                if (read_shift_data[j][1] == 0)
                {
                    Graphics_drawStringCentered(&g_sContext, "N",
                    AUTO_STRING_LENGTH,
                                                96, y_dist,
                                                TRANSPARENT_TEXT);
                }
                else
                {
                    sprintf(tempC, "%d", read_shift_data[j][1]);
                    Graphics_drawStringCentered(&g_sContext, tempC,
                    AUTO_STRING_LENGTH,
                                                96, y_dist,
                                                TRANSPARENT_TEXT);
                }
                break;

                // Case to deal with the RPM value
            case 2:
                sprintf(tempC, "%d", read_shift_data[j][2]);
                Graphics_drawStringCentered(&g_sContext, tempC,
                AUTO_STRING_LENGTH,
                                            160, y_dist,
                                            TRANSPARENT_TEXT);
                break;

                // Case to deal with the speed (MPH) of the shift
            case 3:
                sprintf(tempC, "%d", read_shift_data[j][3]);
                Graphics_drawStringCentered(&g_sContext, tempC,
                AUTO_STRING_LENGTH,
                                            224, y_dist,
                                            TRANSPARENT_TEXT);
                break;

                // Case to deal with the status of the shift (whether it failed or was successful)
            case 4:
                if (read_shift_data[j][4] == 1)
                {
                    Graphics_drawStringCentered(&g_sContext, "Good",
                    AUTO_STRING_LENGTH,
                                                288, y_dist,
                                                TRANSPARENT_TEXT);
                }

                else if (read_shift_data[j][4] == 0)
                {
                    Graphics_drawStringCentered(&g_sContext, "Fail",
                    AUTO_STRING_LENGTH,
                                                288, y_dist,
                                                TRANSPARENT_TEXT);
                }

                break;
            }
        }
        y_dist = y_dist + 36;
    }
}

void clockInit(void)
{
    /* 2 flash wait states, VCORE = 1, running off DC-DC, 48 MHz */
    FlashCtl_setWaitState( FLASH_BANK0, 2);
    FlashCtl_setWaitState( FLASH_BANK1, 2);
    PCM_setPowerState( PCM_AM_DCDC_VCORE1);
    CS_setDCOCenteredFrequency( CS_DCO_FREQUENCY_48);
    CS_setDCOFrequency(48000000);
    CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, 1);
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, 1);
    CS_initClockSignal(CS_HSMCLK, CS_DCOCLK_SELECT, 1);

    return;
}

void Delay(uint16_t msec)
{
    uint32_t i = 0;
    uint32_t time = (msec / 1000) * (SYSTEM_CLOCK_SPEED / 15);

    for (i = 0; i < time; i++)
        ;
}
