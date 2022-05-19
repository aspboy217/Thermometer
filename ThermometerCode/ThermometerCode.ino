/* Libraries */
#include "GLOBAL.h"
#include "METHODS.h"
#include "MARIO.h"
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
long time_mark = 0;
bool format = true;   // true for C, false for F
int8_t num_failure = -1;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); //Declaring the OLED name



void OLED_setup();
void OLED_print(options opt, double temp = 0, bool format = true);
void oled_print(String message);

void setup() {
  pin_config();

  OLED_setup();

  OLED_print(NORMAL, 0, format);

  tone(SPEAKER, 1000, 500);
  //mario_sing(1);
  //mario_sing(2);
}

void loop() {
  
  if(isButtonPressed()) {
    tone(SPEAKER, 500, 500);
    while(isButtonPressed()) {} // just busy wait
    format = !format;
  }

  if(isContact()){ 
    OLED_print(MEASURING, 0, format);
    
    digitalWrite(LED, HIGH);
    delay(HEATUP_DELAY);  // heating up the metal

    
    do {
      //1000 Hz, 500 ms
      tone(SPEAKER, 1000, 250);
      num_failure++;
      if(num_failure >= FAIL_LIMIT){
        num_failure = -1;
        invalidMeas = true;
        break;
      }
    } while((temp = measureTemp()) < 0);
    
    if(invalidMeas) {
      OLED_print(INVALID, 0, format);
      delay(3000);
      invalidMeas = false;
      printTemp = false;
    }
    else {
      printTemp = true;
      time_mark = millis();

      OLED_print(SHOWTEMP, temp, format);
      mario_sing(1);
      
      temp = 0;
    }
    digitalWrite(LED, LOW);
  }

  if(printTemp) {
    if((millis() - time_mark) > 5000)
      printTemp = false;
  } 
  else {
    OLED_print(NORMAL, 0, format);
  }
  
  // cooltime to shut off the thermometer
  // celsius vs farenheit (do something with button?) - do outside of calculateTemp (just convert from C -> F)
}







/* measureTemp : entire process of measuring temperature and return result in Celsius 
 *  return -1   - invalid measurement (due to upper 3 bit checkup - isValid)
 *  return tmp  - temperature in Celsius (double type - NOT one decimal)
 */
// return negative value = failed measurement
double measureTemp(){
  resetCounter();     // reset counter
  delay(1000);
  pulseTrigger();     // send one pulse to timer
  delay(1000);

  if(!isValid())
    return -1;
  else {
    uint16_t reading = readCounter();

    oled_print("valid meas");
    oled_print(String(reading,DEC));
    
    if((reading < MINCOUNT) || (reading > MAXCOUNT))
      return -1;
    
    return calculateTemp(reading);
  }
}














void OLED_setup() {
  // OLED print 
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); //Start the OLED display
  display.display();
}

/* OLED_print : print OLED depending on status of machine
 *  opt = NORMAL , temp = NC     - print instruction to contact
 *  opt = SHOWTEMP, temp = value  - print temperature value in Celsius or Farenheit
 *  opt = MEASURING, temp = NC    - print that the measuring is in progress
 */
void OLED_print(options opt, double temp = 0, bool format = true){ // PRINT WITH ONE PRECISION
  display.clearDisplay();
  display.setTextSize(1);
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
  else if(opt == INVALID) {
    display.setCursor(0,0);
    display.println("Measuring invalid!");
  }
  else {
    display.setCursor(0, 0);
    display.println("Please contact your");
    display.println("skin");
    display.setCursor(0,15);
    display.println("to measure your");
    display.println("temperature");
  }
  display.display();
}

/* for debugging purposes */
void oled_print(String message) {
  display.clearDisplay();
  display.setCursor(0,0);
  display.println(message);
  display.display();
  delay(3000);
}
