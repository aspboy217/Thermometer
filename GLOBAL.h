#ifndef GLOBAL_H
#define GLOBAL_H

/* ========================= Pin definitions ========================= */
// OUTPUT
#define SPEAKER     1   // PD3
#define LED         23  // PC0
#define nTRIGGER    2   // PD4  active low - start timer
#define nCLEAR      25  // PC2  active low - clear timer
// INPUT
#define BUTTON      24  // PC1  active high
#define CONTACT     22  // ADC7
#define VALID_BIT   26  // PC3  high = good measurement

#define CTR_PIN_NUM 9
const uint8_t CTR_PIN[CTR_PIN_NUM] = {31, 9, 10, 11, 12, 13, 14, 32, 30};
/* 31:PD1    9: PD5   10:PD6  11:PD7
 * 12:PB0    13:PB1   14:PB2  32:PD2
 * 30:PD0
 */

/* ========================= OLED vars ========================= */
#define SCREEN_WIDTH  128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)

/* ========================= other vars ========================= */
enum options {DEFAULT, SHOWTEMP, MEASURING};  // used for OLED printing options
#define CONTACT_THRESHOLD   500
#define HEATUP_DELAY        1000    // 1 sec
#define PULSE_WIDTH         1100/20 // in microsec

// based on Celsius (CHOOSE THIS)
#define MINCOUNT      1
#define MAXCOUNT      2
#define MINTEMP       20
#define MAXTEMP       40
#define STEPS         ((MAXCOUNT-MINCOUNT))
#define TMP_PER_STEP  (((MAXTEMP-MINTEMP)/STEPS))

#endif