/* =============================================================
 * File   : micro_oven.h
 * Author : Satheeswaran M
 * Project: Microwave Oven Controller (PIC16F877A)
 * Brief  : Public interface for the microwave oven logic layer.
 *          Defines the fan pin, state-machine screen IDs,
 *          reset flag constants, and the function prototypes
 *          implemented in micro_oven.c.
 * ============================================================= */

#ifndef MICRO_OVEN_H
#define MICRO_OVEN_H

/* ---- Fan control ------------------------------------------- */
#define FAN       RC2    /* Fan output pin                        */
#define FAN_DDR   TRISC2 /* Fan pin direction register            */
#define ON        1
#define OFF       0

/* ---- Screen / state IDs (screen_flag values) ---------------
 * Non-contiguous values chosen deliberately so any accidental
 * assignment of 0x00 never matches a valid screen.
 * ------------------------------------------------------------ */
#define POWER_SCREEN      0x10  /* Animated startup splash screen   */
#define MENU_SCREEN       0x20  /* Main mode-selection menu         */
#define MICRO_MODE        0x30  /* Microwave — set cooking time     */
#define GRILL_MODE        0x40  /* Grill     — set cooking time     */
#define CONVECTION_MODE   0x50  /* Convection — set temperature     */
#define START_MODE        0x60  /* Quick-start (fixed 30 s heat)    */
#define DISPLAY_TIME      0x70  /* Live countdown screen            */
#define PAUSE             0x80  /* Cooking paused by user           */
#define STOP              0x90  /* Cooking stopped, return to menu  */

/* ---- Reset flag constants ---------------------------------- */
#define RESET_FLAG      0x31  /* Passed to set_time/set_temp to    */
                              /* clear state on first entry         */
#define RESET_NOTHING   0xFF  /* Normal (no-reset) value           */

/* ---- Public API -------------------------------------------- */
void display_power_screen(void);
void display_modes(void);
void set_time(unsigned char key, unsigned char reset_flag);
void display_time(void);
void set_temp(unsigned char key, unsigned char reset_flag);
void heat_food(void);

#endif /* MICRO_OVEN_H */
