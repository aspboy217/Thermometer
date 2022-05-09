/* Libraries */
#include "GLOBAL.h"
#include "METHODS.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

/*  LOGIC
 *  if contacted
 *    - buzz (tone method)
 *    - clear counter (clear pin)
 *    - initiate timer (trigger pin)
 *    - if valid bit is bad (low is bad)
 *      - go back to buzz step
 *    - read timer outputs
 *    - calculate to double temperature value
 *    - print to OLED
 *  if not contacted
 *    - OLED print ("contact" || "print temp")
 *    - timer for shutting down?
 */

double temp = 0;
bool printTemp = false;
bool invalidMeas = false;
long time = 0;
bool format = true;   // true for C, false for F
int8_t num_failure = -1;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); //Declaring the OLED name


void setup() {
  Serial.begin(9600); // for debugging

  atmega_setup();

  pin_config();

  OLED_setup();

  OLED_print(DEFAULT, 0, format);
}

void loop() {
  if(isButtonPressed()) {
    while(isButtonPressed()) {} // just busy wait
    format = !format;
  }

  if(isContact()){ 
    OLED_print(MEASURING, 0, format);

    digitalWrite(LED, HIGH);
    delay(HEATUP_DELAY);  // heating up the metal

    do {
      /*turn on speaker*/
      //built in function for 50% square wave
      /*!!!!!!!!!!!!!!!!!
      * THIS function uses 0-5V whereas the datasheet says 0-2.5V, but I feel like it doesn't make a difference since
      * the transistor can handle that much base voltage?*/
      //1000 Hz, 500 ms
      tone(SPEAKER, 1000, 500);
      num_failure++;
      if(num_failure >= FAIL_LIMIT){
        num_failure = -1;
        invalidMeas = true;
        break;
      }
    } while((temp = measureTemp()) < 0);
    
    if(invalidMeas) {
      OLED_print(INVALID, 0, format);
      delay(3);
      invalidMeas = false;
      printTemp = false;
    }
    else {
      // compare 2-3 results & average?
      printTemp = true;
      time = millis();
      OLED_print(SHOWTEMP, temp, format);
      
      temp = 0;
      digitalWrite(LED, LOW);
    }
  }

  if(printTemp) {
    if((millis() - time) > 5000)
      printTemp = false;
  } 
  else {
    OLED_print(DEFAULT, 0, format);
  }
  
  // cooltime to shut off the thermometer
  // celsius vs farenheit (do something with button?) - do outside of calculateTemp (just convert from C -> F)
}