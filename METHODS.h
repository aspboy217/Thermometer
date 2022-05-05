#ifndef METHODS_H
#define METHODS_H

#include "GLOBAL.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/* NOTE: SPECIFIC HEADER FILE HAS TO BE INCLUDED - CONFIRM WITH COMPILER
 * SREG: AVR (CPU) status register
 *    
 */

/* PORT B (p 91, 36)
 *  8-bit I/O (internal pull-up resistor)
 *  PB6: can be used as input 
 *    - to the inverting oscillator amp 
 *    - to the internal clock operating circuit
 *  PB7: can be used as output
 *    - from the inverting oscillator amp
 * 
 * PORT C 
 *  7-bit I/O (internal pull-up resistor)s
 * 
 * PORT D (p 97)
 *  8-bit I/O (internal pull-up resistor)
 */

/* Crystal oscillator: for ceramic resonators, the capacitor values given by the manufacturer should be used */
void atmega_setup() {
  /* full-swing crystal oscillator setting (p 40) */
  CKSEL = 0x6; // full-swing crystal oscillator 0.4-20MHz with 12-22pF
  SUT = 0x0;     // ceramic resonator - fast rising power

  /* System Clock Prescaler (p 46) */
  CLKPCE = 0x1;   // Clock Prescaler Change Enable (must be 1 to change CLKPS bits)
  CLKPS = 0x3; // Clock division by 8
}

void pin_config() {
  /* Configuring the Pin (p 85)
   *    - each port - DDxn    : direction         (1 = output, 0 = input)
   *                - PORTxn  : pullups for input (1 = enable, 0 = disable)
   *                - PINxn   : drive for outpus  (1 = HIGH,   0 = LOW) - toggle regardless of DDxn
   */ 

  pinMode(SPEAKER,  OUTPUT);
  pinMode(LED,      OUTPUT);
  pinMode(nTRIGGER, OUTPUT);
  pinMode(nCLEAR,   OUTPUT);
  pinMode(BUTTON,   INPUT);
  pinMode(CONTACT,  INPUT);
  pinMode(VALID_BIT,INPUT);
  for(int i = 0; i < CTR_PIN_NUM; i++) {
    pinMode(CTR_PIN[i], INPUT);
  }

  //output setup
  digitalWrite(nTRIGGER, HIGH); // trigger pin is active low
  digitalWrite(nCLEAR,   HIGH);
  digitalWrite(LED,      LOW);
}

void OLED_setup() {
  // OLED print 
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); //Start the OLED display
  display.display();
}

/* ========================= METHODS ========================= */
/* isContact : measuring contact (active low)
 *  return true  - contact measured
 *  return false - no contact
 */
bool isContact() {
  if(analogRead(CONTACT) < CONTACT_THRESHOLD)
    return true;
  return false;
}

/* isButtonPressed : measuring button press (active high)
 *  return true   - button pressed  
 *  return false  - button not pressed
 */
bool isButtonPressed() {
  if(analogRead(BUTTON) > 500)
    return true;
  return false;
}

/* resetCounter : initiate low pulse for counters (active low)
 */
void resetCounter() {
  digitalWrite(nCLEAR, LOW);
  delayMicroseconds(100);
  digitalWrite(nCLEAR, HIGH);
}

/* pulsetTrigger : initiate low pulse for timer circuit (active low)
 *    pulse width can be controlled with PULSE_WIDTH
 */
void pulseTrigger() {
  digitalWrite(nTRIGGER, LOW);
  delayMicroseconds(PULSE_WIDTH); // 1/20th of the shortest time should be good?
  digitalWrite(nTRIGGER, HIGH);
}

/* isValid : check upper 3 bits from counter (should be b'100)
 *  return true   - valid count
 *  return false  - invalid count
 */
bool isValid() {
  return digitalRead(VALID_BIT);
}

/* readCounter : read counter latched outputs and convert to 16bit unsigned int
 *  return total number of counts (including upper 3 bits b'100)
 */
uint16_t readCounter(){
  uint16_t result = 0;
  for(int i = 0; i < CTR_PIN_NUM; i++)
    result |= digitalRead(CTR_PIN[i]) << i;

  result |= 0x4 << CTR_PIN_NUM;
  return result + 0x1;          // check this
}

/* calculateTemp : change temperature from int count to double format
 *  return temperature in celsius
 */
double calculateTemp(uint16_t tmp)) {
  return (double)MINTEMP + (double)TMP_PER_STEP * (double)(tmp - MINCOUNT);
}

/* measureTemp : entire process of measuring temperature and return result in Celsius 
 *  return -1   - invalid measurement (due to upper 3 bit checkup - isValid)
 *  return tmp  - temperature in Celsius (double type - NOT one decimal)
 */
// return negative value = failed measurement
double measureTemp(){
  resetCounter();     // reset counter
  delayMicroseconds(100);
  pulseTrigger();     // send one pulse to timer
  
  delayMicroseconds(100); // 0.1 sec delay to have result

  if(!isValid())
    return -1;
  else {
    uint16_t reading = readCounter();
    
    if((reading < MINCOUNT) || (reading > MAXCOUNT))
      return -1;
    
    return calculateTemp(reading);
  }
}

/* OLED_print : print OLED depending on status of machine
 *  opt = DEFAULT , temp = NC     - print instruction to contact
 *  opt = SHOWTEMP, temp = value  - print temperature value in Celsius or Farenheit
 *  opt = MEASURING, temp = NC    - print that the measuring is in progress
 */
void OLED_print(options opt, double temp = 0, bool format = true){ // PRINT WITH ONE PRECISION
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE); 

  if(opt == SHOWTEMP) {
    display.setCursor(0,0);                         
    display.println("Your body temperature is:"); // might not be println - try print
    display.setCursor(0,15);
    if(format){
      display.println(temp);
      display.setCursor(0, 50);
      display.println("C");
    }
    else{
      display.println(temp * 1.4 + 32);
      display.setCursor(0, 50);
      display.println("F");
    }
  }
  else if(opt == MEASURING){
    display.setCursor(0,0);                         
    display.println("Measuring your temperature...");
  }
  else {
    display.setCursor(0, 0);
    display.println("Please contact your skin");
    display.setCursor(0,15);
    display.println("to measure your temperature");
  }
  display.display();
}

#endif