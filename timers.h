/* =============================================================
 * File   : timers.h
 * Author : Satheeswaran M
 * Project: Microwave Oven Controller (PIC16F877A)
 * Brief  : Public interface for timer initialisation routines.
 *          Only init_timer2() is used in the current build;
 *          init_timer0() is declared here for future expansion.
 * ============================================================= */

#ifndef TIMERS_H
#define TIMERS_H

void init_timer0(void);   /* Reserved — not used in current build */
void init_timer2(void);   /* Initialises Timer2 for 1-second tick */

#endif /* TIMERS_H */
