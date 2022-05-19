#ifndef GLOBAL_H
#define GLOBAL_H

/* ========================= Pin definitions ========================= */
// OUTPUT
#define SPEAKER     3   // PD3
#define LED         A0  // PC0
#define nTRIGGER    4   // PD4  active low - start timer
#define nCLEAR      A2  // PC2  active low - clear timer
// INPUT
#define BUTTON      A1  // PC1  active high
#define CONTACT     A7  // ADC7
#define VALID_BIT   A3  // PC3  high = good measurement

#define CTR_PIN_NUM 9
const uint8_t CTR_PIN[CTR_PIN_NUM] = {1, 5, 6, 7, 8, 9, 10, 2, 0}; // MSB to LSB
/* 31:PD1    9: PD5   10:PD6  11:PD7
 * 12:PB0    13:PB1   14:PB2  32:PD2
 * 30:PD0
 */

/* ========================= OLED vars ========================= */
#define SCREEN_WIDTH  128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)

/* ========================= other vars ========================= */
enum options {NORMAL, SHOWTEMP, MEASURING, INVALID};  // used for OLED printing options
#define CONTACT_THRESHOLD   500
#define HEATUP_DELAY        1000    // 1 sec
#define PULSE_WIDTH         50 // in microsec
#define FAIL_LIMIT          10

// based on Celsius (CHOOSE THIS)
#define MINCOUNT      2248
#define MAXCOUNT      2402
#define MINTEMP       20
#define MAXTEMP       40
const double TMP_PER_STEP = double((MAXTEMP-MINTEMP))/(MAXCOUNT - MINCOUNT);

#endif
