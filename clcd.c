/* =============================================================
 * File   : clcd.c
 * Author : Satheeswaran M
 * Project: Microwave Oven Controller (PIC16F877A)
 * Brief  : Character LCD (HD44780-compatible) driver.
 *          Uses 4-bit data bus on PORTD upper nibble.
 *          RS → RE2,  EN → RE1.
 * ============================================================= */

#include <xc.h>
#include "clcd.h"

/* ---------------------------------------------------------------
 * clcd_write(byte, mode)  [static — internal use only]
 * Sends one byte to the LCD using the 4-bit interface:
 *   1. Set RS line according to mode (INST_MODE or DATA_MODE).
 *   2. Send the upper nibble (D7–D4) via PORTD, pulse EN.
 *   3. Send the lower nibble shifted into D7–D4, pulse EN.
 *   4. Wait 4.1 ms for the LCD controller to process the command.
 * --------------------------------------------------------------- */
static void clcd_write(unsigned char byte, unsigned char mode)
{
    CLCD_RS = (__bit)mode;    /* RS = 0 → instruction, RS = 1 → data */

    /* Upper nibble: bits 7–4 → PORTD bits 7–4 */
    CLCD_DATA_PORT = byte & 0xF0;
    CLCD_EN = HI;
    __delay_us(100);
    CLCD_EN = LOW;

    /* Lower nibble: bits 3–0 shifted up → PORTD bits 7–4 */
    CLCD_DATA_PORT = (unsigned char)((byte & 0x0F) << 4);
    CLCD_EN = HI;
    __delay_us(100);
    CLCD_EN = LOW;

    __delay_us(4100);         /* Busy time for most LCD commands */
}

/* ---------------------------------------------------------------
 * init_display_controller()  [static — internal use only]
 * Runs the HD44780 power-on initialisation sequence:
 *   - Three 8-bit mode writes to synchronise the controller.
 *   - Switch to 4-bit mode.
 *   - Configure 2-line display with 5×8 font.
 *   - Clear the screen.
 *   - Turn display on with cursor hidden.
 * --------------------------------------------------------------- */
static void init_display_controller(void)
{
    __delay_ms(30);     /* Allow LCD power supply to stabilise */

    /* HD44780 reset sequence — three writes in 8-bit mode */
    clcd_write(EIGHT_BIT_MODE, INST_MODE);
    __delay_us(4100);
    clcd_write(EIGHT_BIT_MODE, INST_MODE);
    __delay_us(100);
    clcd_write(EIGHT_BIT_MODE, INST_MODE);
    __delay_us(1);

    /* Switch controller to 4-bit bus mode */
    clcd_write(FOUR_BIT_MODE, INST_MODE);
    __delay_us(100);

    /* 2-line display, 5×8 character font, 4-bit interface */
    clcd_write(TWO_LINES_5x8_4_BIT_MODE, INST_MODE);
    __delay_us(100);

    /* Clear display (also homes the cursor) */
    clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
    __delay_us(500);

    /* Display ON, cursor hidden */
    clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
    __delay_us(100);
}

/* ---------------------------------------------------------------
 * init_clcd()
 * Public initialisation entry point.
 * Sets data and control pins as outputs, then calls the
 * HD44780 startup sequence.
 * --------------------------------------------------------------- */
void init_clcd(void)
{
    CLCD_DATA_PORT_DDR = 0x00;  /* PORTD all outputs (data bus) */
    CLCD_RS_DDR        = 0;     /* RS pin as output             */
    CLCD_EN_DDR        = 0;     /* EN pin as output             */

    init_display_controller();
}

/* ---------------------------------------------------------------
 * clcd_putch(data, addr)
 * Moves the cursor to DDRAM address 'addr', then writes a
 * single character. Use the LINE1(x)…LINE4(x) macros for addr.
 * --------------------------------------------------------------- */
void clcd_putch(const char data, unsigned char addr)
{
    clcd_write(addr, INST_MODE);    /* Set DDRAM address */
    clcd_write(data, DATA_MODE);    /* Write character   */
}

/* ---------------------------------------------------------------
 * clcd_print(str, addr)
 * Moves the cursor to 'addr', then writes a null-terminated
 * string character by character. Stops at '\0'.
 * --------------------------------------------------------------- */
void clcd_print(const char *str, unsigned char addr)
{
    clcd_write(addr, INST_MODE);    /* Set starting DDRAM address */

    while (*str != '\0')
    {
        clcd_write(*str, DATA_MODE);
        str++;
    }
}

/* ---------------------------------------------------------------
 * clear_screen()
 * Sends the "Clear Display" instruction and waits for it to
 * complete (execution time ~1.5 ms on most HD44780 clones).
 * --------------------------------------------------------------- */
void clear_screen(void)
{
    clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
    __delay_us(500);
}
