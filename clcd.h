/* =============================================================
 * File   : clcd.h
 * Author : Satheeswaran M
 * Project: Microwave Oven Controller (PIC16F877A)
 * Brief  : Public interface for the Character LCD (CLCD) driver.
 *          Defines pin mappings, LCD command constants, and the
 *          LINE1–LINE4 address macros for convenient cursor
 *          positioning on a 20×4 HD44780-compatible display.
 * ============================================================= */

#ifndef CLCD_H
#define CLCD_H

/* ---- System clock (required by __delay_ms / __delay_us) ---- */
#define _XTAL_FREQ          20000000    /* 20 MHz crystal */

/* ---- Hardware pin mapping ----------------------------------- */
#define CLCD_DATA_PORT_DDR  TRISD       /* Data bus direction register  */
#define CLCD_RS_DDR         TRISE2      /* RS pin direction             */
#define CLCD_EN_DDR         TRISE1      /* EN pin direction             */
#define CLCD_DATA_PORT      PORTD       /* 4-bit data bus (D7–D4 used)  */
#define CLCD_RS             RE2         /* Register Select: 0=cmd 1=data*/
#define CLCD_EN             RE1         /* Enable pulse pin             */

/* ---- Logic levels ------------------------------------------ */
#define INST_MODE           0           /* RS = 0 → instruction write   */
#define DATA_MODE           1           /* RS = 1 → data write          */
#define HI                  1
#define LOW                 0

/* ---- DDRAM address macros ----------------------------------
 * LINE1–LINE4 return the DDRAM address for column x on each row.
 * Pass to clcd_putch() / clcd_print() as the 'addr' argument.
 * Bit 7 is set to signal a "Set DDRAM Address" command to the LCD.
 * ------------------------------------------------------------ */
#define LINE1(x)    (0x80 + (x))    /* Row 1: 0x80–0x8F */
#define LINE2(x)    (0xC0 + (x))    /* Row 2: 0xC0–0xCF */
#define LINE3(x)    (0x90 + (x))    /* Row 3: 0x90–0x9F */
#define LINE4(x)    (0xD0 + (x))    /* Row 4: 0xD0–0xDF */

/* ---- HD44780 initialisation command bytes ------------------ */
#define EIGHT_BIT_MODE              0x33  /* Sync sequence (8-bit mode)  */
#define FOUR_BIT_MODE               0x02  /* Switch to 4-bit interface   */
#define TWO_LINES_5x8_4_BIT_MODE    0x28  /* 2-line, 5×8 font, 4-bit    */
#define CLEAR_DISP_SCREEN           0x01  /* Clear display + home cursor */
#define DISP_ON_AND_CURSOR_OFF      0x0C  /* Display ON, cursor hidden   */

/* ---- Custom character code --------------------------------- */
#define BAR                         0xFF  /* Full solid block (■)        */

/* ---- Public API -------------------------------------------- */
void init_clcd(void);
void clcd_putch(const char data, unsigned char addr);
void clcd_print(const char *str,  unsigned char addr);
void clear_screen(void);

#endif /* CLCD_H */
