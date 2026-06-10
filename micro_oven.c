/* =============================================================
 * File   : micro_oven.c
 * Author : Satheeswaran M
 * Project: Microwave Oven Controller (PIC16F877A)
 * Brief  : Screen rendering and cooking-mode logic.
 *          Handles power screen, mode menu, time/temperature
 *          input, countdown display, and quick-start heating.
 * ============================================================= */

#include <xc.h>
#include "clcd.h"
#include "micro_oven.h"
#include "matrix_keypad.h"

/* Countdown registers — updated every second by the ISR */
unsigned char min, sec;

/* Temperature target for convection mode (°C) */
unsigned int temp;

/* Screen and reset control flags — set by main.c */
extern unsigned char screen_flag;
extern unsigned char reset_flag;

/* Pre-heat countdown (seconds) for convection warm-up */
unsigned char pre_heat;

/* ---------------------------------------------------------------
 * display_power_screen()
 * Animates a startup splash on all four CLCD lines:
 *   Line 1 & 4 : sweeping BAR characters (■■■■ ... ■■■■)
 *   Line 2      : "Powering ON"
 *   Line 3      : "Microwave Oven"
 * --------------------------------------------------------------- */
void display_power_screen(void)
{
    /* Sweep a solid-block character across Line 1 */
    for (unsigned char i = 0; i < 16; i++)
    {
        clcd_putch(BAR, LINE1(i));
        __delay_ms(100);
    }

    clcd_print("Powering ON",     LINE2(3));
    clcd_print("Microwave Oven",  LINE3(2));

    /* Sweep a solid-block character across Line 4 */
    for (unsigned char i = 0; i < 16; i++)
    {
        clcd_putch(BAR, LINE4(i));
        __delay_ms(100);
    }

    __delay_ms(1000);   /* Hold the splash for one second */
}

/* ---------------------------------------------------------------
 * display_modes()
 * Renders the four-option main menu on the CLCD:
 *   1 Micro  |  2 Grill  |  3 Convection  |  4 Start
 * --------------------------------------------------------------- */
void display_modes(void)
{
    clcd_print("1 Micro",      LINE1(3));
    clcd_print("2 Grill",      LINE2(3));
    clcd_print("3 Convection", LINE3(3));
    clcd_print("4 Start",      LINE4(3));
}

/* ---------------------------------------------------------------
 * set_time(key, reset_flag)
 * Interactive time-entry screen used by Micro and Grill modes.
 *
 *   Layout : "SET TIME (MM:SS)"   — Line 1
 *             "TIME: MM:SS"        — Line 2  (blinking field)
 *             "*:CLEAR  #:ENTER"   — Line 4
 *
 *   Input rules:
 *     Digits 0-9  → fill seconds first (up to 3 digits),
 *                   then minutes (up to 2 more digits).
 *     '*'         → clear the currently active field.
 *     '#'         → confirm, start fan + timer, go to DISPLAY_TIME.
 *
 *   Blinking: the active field blanks every ~10 polling cycles
 *   to give a cursor-like visual feedback.
 * --------------------------------------------------------------- */
void set_time(unsigned char key, unsigned char reset_flag)
{
    /* blink_pos : 0 = seconds field active, 1 = minutes field active */
    static unsigned char blink_pos    = 0;
    static unsigned char key_count    = 0;
    static unsigned char delay, display_blank;

    /* Reset all state when transitioning into this screen */
    if (reset_flag == RESET_FLAG)
    {
        sec       = 0;
        min       = 0;
        key       = 0;
        key_count = 0;
        blink_pos = 0;
    }

    /* Draw static labels */
    clcd_print("SET TIME (MM:SS)", LINE1(0));
    clcd_print("TIME:",            LINE2(0));
    clcd_putch(':',                LINE2(7));
    clcd_print("*:CLEAR #:ENTER",  LINE4(0));

    if (key != '*' && key != '#' && key != ALL_RELEASED)
    {
        key_count++;
        if (key_count <= 3)
        {
            /* First three digits go into seconds */
            sec       = sec * 10 + key;
            blink_pos = 0;
        }
        else if (key_count <= 5)
        {
            /* Next two digits go into minutes */
            min       = min * 10 + key;
            blink_pos = 1;
        }
    }
    else if (key == '*')
    {
        /* Clear the active field and step back key_count */
        if (blink_pos == 0)
        {
            sec       = 0;
            key_count = 0;
        }
        else if (blink_pos == 1)
        {
            min       = 0;
            key_count = 2;
        }
    }
    else if (key == '#')
    {
        /* Confirm time, start fan and Timer2, switch to countdown */
        clear_screen();
        FAN    = ON;
        TMR2ON = 1;
        screen_flag = DISPLAY_TIME;
    }

    /* Toggle blank flag every 10 cycles (≈ blink rate) */
    if (delay++ == 10)
    {
        delay        = 0;
        display_blank = !display_blank;
    }

    if (display_blank)
    {
        /* Blank out the active field to create blinking effect */
        if (blink_pos == 0)
        {
            clcd_putch(' ', LINE2(8));
            clcd_putch(' ', LINE2(9));
        }
        else
        {
            clcd_putch(' ', LINE2(5));
            clcd_putch(' ', LINE2(6));
        }
    }
    else
    {
        /* Render the current seconds and minutes values */
        clcd_putch(sec / 10 + '0', LINE2(8));
        clcd_putch(sec % 10 + '0', LINE2(9));
        clcd_putch(min / 10 + '0', LINE2(5));
        clcd_putch(min % 10 + '0', LINE2(6));
    }
}

/* ---------------------------------------------------------------
 * set_temp(key, reset_flag)
 * Interactive temperature-entry screen for Convection mode.
 *
 *   Layout : "SET TEMP <*C>"     — Line 1
 *             "TIME=: XXX"        — Line 2  (blinking value)
 *             "*:CLEAR  #:ENTER"  — Line 4
 *
 *   After '#': clamps temp to 180 °C max, runs a 60-second
 *   pre-heat countdown on the CLCD, then jumps to MICRO_MODE.
 * --------------------------------------------------------------- */
void set_temp(unsigned char key, unsigned char reset_flag)
{
    static unsigned char key_count = 0;
    static unsigned char delay, display_blank;

    /* Reset state when entering this screen */
    if (reset_flag == RESET_FLAG)
    {
        temp      = 0;
        key       = 0;
        key_count = 0;
    }

    /* Toggle blink every 5 cycles (faster than set_time) */
    if (delay++ == 5)
    {
        delay        = 0;
        display_blank = !display_blank;
    }

    /* Draw static labels */
    clcd_print("SET TEMP <*C>",   LINE1(0));
    clcd_print("TIME=:",           LINE2(0));
    clcd_print("*:CLEAR #:ENTER",  LINE4(0));

    if (key != '*' && key != '#' && key != ALL_RELEASED)
    {
        key_count++;
        if (key_count <= 3)
        {
            /* Build up to a 3-digit temperature value */
            temp = temp * 10 + key;
        }
    }
    else if (key == '*')
    {
        /* Clear temperature and restart digit count */
        temp      = 0;
        key_count = 0;
    }
    else if (key == '#')
    {
        /* Clamp maximum temperature to 180 °C */
        if (temp > 180)
        {
            temp = 180;
        }

        /* Run the pre-heat phase (60 seconds) before cooking starts */
        clear_screen();
        pre_heat = 60;
        TMR2ON   = 1;
        clcd_print("Pre-Heating",   LINE1(3));
        clcd_print("Time Rem:",     LINE3(0));
        clcd_putch('s',             LINE3(12));

        /* Busy-wait until ISR decrements pre_heat to zero */
        while (pre_heat)
        {
            clcd_putch(pre_heat / 100       + '0', LINE3(9));
            clcd_putch((pre_heat / 10) % 10 + '0', LINE3(10));
            clcd_putch((pre_heat % 10)      + '0', LINE3(11));
        }

        /* Pre-heat done — switch to microwave cooking screen */
        TMR2ON      = 0;
        clear_screen();
        screen_flag = MICRO_MODE;
        reset_flag  = RESET_FLAG;
    }

    /* Render or blank the 3-digit temperature value */
    if (display_blank)
    {
        clcd_putch(' ', LINE2(7));
        clcd_putch(' ', LINE2(8));
        clcd_putch(' ', LINE2(9));
    }
    else
    {
        clcd_putch(temp / 100       + '0', LINE2(7));
        clcd_putch((temp / 10) % 10 + '0', LINE2(8));
        clcd_putch((temp % 10)      + '0', LINE2(9));
    }
}

/* ---------------------------------------------------------------
 * display_time()
 * Shows the live countdown and action keys during cooking.
 *
 *   Line 1 : "TIME= MM:SS"
 *   Line 2 : "4.START/RESUME"
 *   Line 3 : "5.PAUSE"
 *   Line 4 : "6.STOP"
 *
 * When both min and sec reach 0, stops the timer/fan, shows
 * a "TIME UP!" message, then returns to the menu screen.
 * --------------------------------------------------------------- */
void display_time(void)
{
    clcd_print("TIME=", LINE1(1));

    /* Print MM:SS from the ISR-managed global variables */
    clcd_putch(min / 10 + '0', LINE1(9));
    clcd_putch(min % 10 + '0', LINE1(10));
    clcd_putch(':',             LINE1(11));
    clcd_putch(sec / 10 + '0', LINE1(12));
    clcd_putch(sec % 10 + '0', LINE1(13));

    clcd_print("4.START/RESUME", LINE2(2));
    clcd_print("5.PAUSE",        LINE3(2));
    clcd_print("6.STOP",         LINE4(2));

    /* Check if countdown has finished */
    if (min == 0 && sec == 0)
    {
        clear_screen();
        TMR2ON = 0;
        FAN    = OFF;

        clcd_print("TIME UP!!!", LINE2(4));
        clcd_print("ENJOY YOUR MEAL", LINE3(0));
        __delay_ms(2000);

        clear_screen();
        screen_flag = MENU_SCREEN;
    }
}

/* ---------------------------------------------------------------
 * heat_food()
 * Quick-start mode: sets a fixed 30-second cook time,
 * enables the fan, starts Timer2, and jumps to the
 * countdown display immediately — no user input needed.
 * --------------------------------------------------------------- */
void heat_food(void)
{
    sec         = 30;
    min         = 0;
    TMR2ON      = 1;
    FAN         = 1;
    screen_flag = DISPLAY_TIME;
}
