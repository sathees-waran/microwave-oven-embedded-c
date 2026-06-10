/* =============================================================
 * File   : main.c
 * Author : Satheeswaran M
 * Project: Microwave Oven Controller (PIC16F877A)
 * Brief  : Entry point. Initializes peripherals and runs the
 *          main state-machine loop that drives all oven modes.
 * ============================================================= */

#include <xc.h>
#include "clcd.h"
#include "micro_oven.h"
#include "matrix_keypad.h"
#include "timers.h"

/* Disable Watchdog Timer — not needed for this project */
#pragma config WDTE = OFF

/* Global screen state — shared with micro_oven.c via extern */
unsigned char screen_flag = POWER_SCREEN;

/* Countdown values updated by Timer2 ISR */
extern unsigned char min, sec;

/* ---------------------------------------------------------------
 * init_config()
 * Initialises all peripherals used by the oven controller.
 * Call once at startup before the main loop.
 * --------------------------------------------------------------- */
static void init_config(void)
{
    init_clcd();            /* Set up 4-bit CLCD display          */
    init_matrix_keypad();   /* Configure 4x3 matrix keypad GPIOs  */
    init_timer2();          /* Timer2 for 1-second tick generation */

    FAN_DDR = 0;            /* Set fan control pin as output       */

    PEIE = 1;               /* Enable peripheral interrupts        */
    GIE  = 1;               /* Enable global interrupts            */
}

/* ---------------------------------------------------------------
 * main()
 * Runs the polling loop:
 *   1. Read keypad once per iteration.
 *   2. Handle mode-transition keys.
 *   3. Dispatch to the appropriate screen/mode handler via switch.
 * --------------------------------------------------------------- */
void main(void)
{
    init_config();

    unsigned char key;
    unsigned char reset_flag;

    while (1)
    {
        /* Read keypad in STATE mode — returns value only on new press */
        key = read_matrix_keypad(STATE);

        /* ---- Handle MENU_SCREEN key presses ---- */
        if (screen_flag == MENU_SCREEN)
        {
            if (key == 1)
            {
                /* Key 1 → Microwave mode — show power level briefly */
                screen_flag = MICRO_MODE;
                clear_screen();
                clcd_print("power = 900W", LINE2(2));
                __delay_ms(3000);
                clear_screen();
                reset_flag = RESET_FLAG;
            }
            else if (key == 2)
            {
                /* Key 2 → Grill mode */
                screen_flag = GRILL_MODE;
                reset_flag  = RESET_FLAG;
                clear_screen();
            }
            else if (key == 3)
            {
                /* Key 3 → Convection mode */
                screen_flag = CONVECTION_MODE;
                clear_screen();
            }
            else if (key == 4)
            {
                /* Key 4 → Quick-start (30-second heat) */
                screen_flag = START_MODE;
                reset_flag  = RESET_FLAG;
                clear_screen();
            }
        }
        /* ---- Handle DISPLAY_TIME key presses ---- */
        else if (screen_flag == DISPLAY_TIME)
        {
            if (key == 4)
            {
                /* Key 4 during countdown → add 30 seconds */
                sec = sec + 30;
                if (sec > 59)
                {
                    min++;
                    sec = sec - 60;
                }
            }
            if (key == 5)
            {
                screen_flag = PAUSE;        /* Key 5 → Pause         */
            }
            else if (key == 6)
            {
                screen_flag = STOP;         /* Key 6 → Stop & return */
            }
        }
        /* ---- Handle PAUSE key presses ---- */
        else if (screen_flag == PAUSE)
        {
            if (key == 4)
            {
                /* Key 4 while paused → resume countdown */
                TMR2ON      = 1;
                FAN         = 1;
                screen_flag = DISPLAY_TIME;
            }
        }

        /* ---- State-machine: dispatch to active screen handler ---- */
        switch (screen_flag)
        {
            case POWER_SCREEN:
                /* Show animated power-on splash, then go to menu */
                display_power_screen();
                clear_screen();
                screen_flag = MENU_SCREEN;
                break;

            case MENU_SCREEN:
                /* Show the four mode options */
                display_modes();
                break;

            case MICRO_MODE:
                /* Let the user set a cooking time for microwave mode */
                set_time(key, reset_flag);
                break;

            case DISPLAY_TIME:
                /* Show live countdown; detect when time reaches zero */
                display_time();
                break;

            case GRILL_MODE:
                /* Grill mode also uses the same time-setting screen */
                set_time(key, reset_flag);
                break;

            case CONVECTION_MODE:
                /* Convection needs a temperature target instead */
                set_temp(key, reset_flag);
                break;

            case START_MODE:
                /* Quick-start: 30-second heat with no user input */
                heat_food();
                break;

            case PAUSE:
                /* Timer and fan off while paused */
                TMR2ON = 0;
                FAN    = 0;
                break;

            case STOP:
                /* Stop everything and return to the main menu */
                TMR2ON      = 0;
                FAN         = 0;
                clear_screen();
                screen_flag = MENU_SCREEN;
                break;
        }

        /* Clear the reset flag after each iteration */
        reset_flag = RESET_NOTHING;
    }

    return;
}
