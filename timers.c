/* =============================================================
 * File   : timers.c
 * Author : Satheeswaran M
 * Project: Microwave Oven Controller (PIC16F877A)
 * Brief  : Timer2 initialisation for a 1-second ISR tick.
 *
 * Timing calculation:
 *   Fosc = 20 MHz  →  Tcy (instruction period) = 200 ns × 4 = 800 ns
 *   With 1:16 prescaler  →  timer increments every 800 ns × 16 = 12.8 µs
 *   PR2 = 250            →  overflow every 250 × 12.8 µs = 3.2 ms
 *   NOTE: XC8/PIC docs state Timer2 overflows when TMR2 = PR2,
 *         giving period = (PR2 + 1) × prescaler × Tcy
 *       = 251 × 16 × 200 ns ≈ 803.2 µs per overflow
 *   1 second ≈ 1,246 overflows  (isr.c uses 1,250 — close match)
 * ============================================================= */

#include <xc.h>

/* ---------------------------------------------------------------
 * init_timer2()
 * Sets up Timer2 to generate periodic interrupts:
 *   - Prescaler 1:16  (T2CKPS1:T2CKPS0 = 11)
 *   - Period register PR2 = 250
 *   - Timer2 interrupt enabled
 *   - Timer2 switched on
 * --------------------------------------------------------------- */
void init_timer2(void)
{
    /* Set prescaler to 1:16 (both prescaler bits = 1) */
    T2CKPS0 = 1;
    T2CKPS1 = 1;

    /* Load the period register — Timer2 resets to 0 on match */
    PR2 = 250;

    /* Enable Timer2 match interrupt */
    TMR2IE = 1;

    /* Start Timer2 */
    TMR2ON = 1;
}
