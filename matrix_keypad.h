/* =============================================================
 * File   : matrix_keypad.h
 * Author : Satheeswaran M
 * Project: Microwave Oven Controller (PIC16F877A)
 * Brief  : Public interface for the 4×3 matrix keypad driver.
 *          Defines GPIO mappings, logic levels, scan modes,
 *          and the ALL_RELEASED sentinel value.
 * ============================================================= */

#ifndef MATRIX_KEYPAD_H
#define MATRIX_KEYPAD_H

/* ---- GPIO direction registers ------------------------------ */
#define MATRIX_KEYPAD_COLUMN_PORT_DDR   TRISB  /* RB0–RB2 → inputs  */
#define MATRIX_KEYPAD_ROW_PORT_DDR      TRISD  /* RD0–RD3 → outputs */

/* ---- Row output pins (driven LOW one at a time to scan) ---- */
#define ROW1    RD3
#define ROW2    RD2
#define ROW3    RD1
#define ROW4    RD0

/* ---- Column input pins (HIGH via pull-up when no key pressed) */
#define COL1    RB0
#define COL2    RB1
#define COL3    RB2

/* ---- Logic levels ------------------------------------------ */
#define HI      1
#define LOW     0

/* ---- Scan mode selectors ----------------------------------- */
#define STATE   1   /* Edge-detect: report a key only once per press */
#define LEVEL   0   /* Raw level:   report key state every call       */

/* ---- Sentinel return value --------------------------------- */
#define ALL_RELEASED    0xFF    /* Returned when no key is pressed */

/* ---- Public API -------------------------------------------- */
void          init_matrix_keypad(void);
unsigned char read_matrix_keypad(unsigned char mode);

#endif /* MATRIX_KEYPAD_H */
