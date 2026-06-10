/* =============================================================
 * File   : isr.c
 * Author : Satheeswaran M
 * Project: Microwave Oven Controller (PIC16F877A)
 * Brief  : Interrupt Service Routine for Timer2.
 *          Generates a 1-second tick and decrements the
 *          cooking countdown (min/sec) and pre-heat counter.
 *
 * Timer2 configuration (set in timers.c):
 *   Fosc       = 20 MHz  →  instruction clock = 5 MHz
 *   Prescaler  = 1:16    →  timer clock       = 312.5 kHz
 *   PR2        = 250     →  overflow period   = 800 µs
 *   1 second   = 1,250 overflows
 * ============================================================= */

#include <xc.h>

/* Countdown registers — owned by micro_oven.c */
extern unsigned char sec, min;

/* Pre-heat counter — decremented here, read in set_temp() */
extern unsigned char pre_heat;

/* ---------------------------------------------------------------
 * isr()
 * Fires on every Timer2 overflow (every 800 µs).
 * After 1,250 overflows (= 1 second):
 *   1. Decrements the MM:SS cooking countdown if non-zero.
 *   2. Decrements pre_heat if the oven is pre-heating.
 * Always clears TMR2IF to allow the next interrupt.
 * --------------------------------------------------------------- */
void __interrupt() isr(void)
{
    static unsigned int count = 0;   /* Overflow accumulator */

    if (TMR2IF == 1)
    {
        if (++count == 1250)         /* 1,250 × 800 µs = 1 second */
        {
            count = 0;

            /* Decrement MM:SS countdown only when time remains */
            if (min > 0 || sec > 0)
            {
                if (sec == 0)
                {
                    /* Borrow one minute when seconds hit zero */
                    min--;
                    sec = 59;
                }
                else
                {
                    sec--;
                }
            }

            /* Decrement pre-heat counter if still warming up */
            if (pre_heat > 0)
            {
                pre_heat--;
            }
        }

        TMR2IF = 0;     /* Clear interrupt flag — mandatory */
    }
}
