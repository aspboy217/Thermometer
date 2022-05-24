#ifndef METHODS_H
#define METHODS_H

#include "GLOBAL.h"

void pin_config() {
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
  delay(200);
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
double calculateTemp(uint16_t tmp) {
  return (double)MAXTEMP - (double)TMP_PER_STEP * (double)(tmp - MINCOUNT);
}




#endif
