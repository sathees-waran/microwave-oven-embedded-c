/* =============================================================
 * File   : matrix_keypad.c
 * Author : Satheeswaran M
 * Project: Microwave Oven Controller (PIC16F877A)
 * Brief  : Driver for a 4x3 matrix keypad.
 *          Rows on RD3–RD0 (outputs), Columns on RB2–RB0 (inputs
 *          with pull-ups). Provides both LEVEL and STATE modes.
 * ============================================================= */

#include <xc.h>
#include "matrix_keypad.h"

/* ---------------------------------------------------------------
 * init_matrix_keypad()
 * Configures the GPIO directions for the 4x3 keypad:
 *   - Columns (RB2–RB0) → inputs
 *   - Rows    (RD3–RD0) → outputs
 * Enables PORTB internal pull-ups and drives all rows HIGH
 * so no key appears pressed at startup.
 * --------------------------------------------------------------- */
void init_matrix_keypad(void)
{
    /* Set columns (RB0–RB2) as inputs */
    MATRIX_KEYPAD_COLUMN_PORT_DDR = MATRIX_KEYPAD_COLUMN_PORT_DDR | 0x07;

    /* Set rows (RD0–RD3) as outputs (mask keeps upper nibble unchanged) */
    MATRIX_KEYPAD_ROW_PORT_DDR = MATRIX_KEYPAD_ROW_PORT_DDR & 0xF0;

    /* Enable PORTB weak pull-ups so columns read HIGH when no key pressed */
    nRBPU = 0;

    /* Drive all rows HIGH — idle / no-key-pressed state */
    ROW1 = HI;
    ROW2 = HI;
    ROW3 = HI;
    ROW4 = HI;
}

/* ---------------------------------------------------------------
 * scan_keypad()  [static — internal use only]
 * Activates each row in turn (drives it LOW), then samples
 * all three column lines. Returns the key value for the first
 * LOW column found, or ALL_RELEASED if no key is pressed.
 *
 * Key map:
 *          COL1  COL2  COL3
 *   ROW1:   1     2     3
 *   ROW2:   4     5     6
 *   ROW3:   7     8     9
 *   ROW4:   *     0     #
 *
 * The small delay loop after each row activation lets the line
 * settle before reading the column pins.
 * --------------------------------------------------------------- */
static unsigned char scan_keypad(void)
{
    int i;

    /* ---- Scan Row 1 ---- */
    ROW1 = LOW; ROW2 = HI; ROW3 = HI; ROW4 = HI;
    for (i = 100; i--; );           /* Settling delay */

    if      (COL1 == LOW) return 1;
    else if (COL2 == LOW) return 2;
    else if (COL3 == LOW) return 3;

    /* ---- Scan Row 2 ---- */
    ROW1 = HI; ROW2 = LOW; ROW3 = HI; ROW4 = HI;
    for (i = 100; i > 0; i--);

    if      (COL1 == LOW) return 4;
    else if (COL2 == LOW) return 5;
    else if (COL3 == LOW) return 6;

    /* ---- Scan Row 3 ---- */
    ROW1 = HI; ROW2 = HI; ROW3 = LOW; ROW4 = HI;
    for (i = 100; i > 0; i--);

    if      (COL1 == LOW) return 7;
    else if (COL2 == LOW) return 8;
    else if (COL3 == LOW) return 9;

    /* ---- Scan Row 4 ---- */
    ROW1 = HI; ROW2 = HI; ROW3 = HI; ROW4 = LOW;
    for (i = 100; i > 0; i--);

    if      (COL1 == LOW) return '*';
    else if (COL2 == LOW) return 0;
    else if (COL3 == LOW) return '#';

    return ALL_RELEASED;    /* No key detected */
}

/* ---------------------------------------------------------------
 * read_matrix_keypad(mode)
 * Public API — wraps scan_keypad() with optional debouncing.
 *
 *   LEVEL mode : returns the raw scan result every call.
 *                Useful for held-key detection.
 *
 *   STATE mode : returns the key value only once per physical
 *                press (rising-edge detection via the 'once'
 *                flag). Subsequent calls return ALL_RELEASED
 *                until the key is released and pressed again.
 *                Used by main.c for single-action inputs.
 * --------------------------------------------------------------- */
unsigned char read_matrix_keypad(unsigned char mode)
{
    static unsigned char once = 1;     /* 1 = ready to report a new press */
    unsigned char key;

    key = scan_keypad();

    if (mode == LEVEL)
    {
        /* Return raw state — no edge detection */
        return key;
    }
    else    /* STATE (edge-detect) mode */
    {
        if ((key != ALL_RELEASED) && once)
        {
            once = 0;           /* Lock until key is released */
            return key;
        }
        else if (key == ALL_RELEASED)
        {
            once = 1;           /* Key released — ready for next press */
        }
    }

    return ALL_RELEASED;
}
