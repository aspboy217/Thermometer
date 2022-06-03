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
void OLED_print(options opt = NORMAL, double temp = 0, bool format = true);
void oled_print(String message);

bool hold = false;

/* isContact : measuring contact (active low)
 *  return true  - contact measured
 *  return false - no contact
 */
bool isContact() {
  if(analogRead(CONTACT) < CONTACT_THRESHOLD){
    return true;
  }
  else{
    return false;
  }
}

void setup() {
  pin_config();

  OLED_setup();

  OLED_print(NORMAL, 0, format);

  uint8_t i = 0;
  while (!calibrate()) {
    i++;
    oled_print("calibration fail");
    oled_print("fail " + String(i,DEC));
  }
  
  oled_print("Ready to use");
  
  //mario_sing(1);
  //mario_sing(2);
}

void loop() {
  if(isButtonPressed()) {
    tone(SPEAKER, 500, 500);
    while(isButtonPressed()) {} // just busy wait
    format = !format;
  }

  if(isContact() && !printTemp){
    digitalWrite(LED, HIGH);
    OLED_print(MEASURING, 0, format);
    
    delay(HEATUP_DELAY);  // heating up the metal

    num_failure = -1;
    do {
      tone(SPEAKER, 1000, 250);
      num_failure++;
      if(num_failure >= FAIL_LIMIT){
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
      oled_print("Valid Meas");
      printTemp = true;
      time_mark = millis();

      OLED_print(SHOWTEMP, temp, format);
      //mario_sing(1);
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
}







/* measureTemp : entire process of measuring temperature and return result in Celsius 
 *  return -1   - invalid measurement (due to upper 3 bit checkup - isValid)
 *  return tmp  - temperature in Celsius (double type - NOT one decimal)
 */
double measureTemp(){
  resetCounter();     // reset counter
  delay(1000);
  pulseTrigger();     // send one pulse to timer
  delay(1000);

  if(!isValid()){
    oled_print("valid bit check fail");
    return -1;
  }
  else {
    /*
    uint16_t reading = readCounter();
    
    if((reading < MINCOUNT) || (reading > MAXCOUNT)){
      oled_print("temp out of range");
      return -1;
    }

    */

    /*
    double result = readCounter();
    oled_print("count");
    oled_print(String(result,DEC));
    result = calculateTemp(result);
    oled_print("temp");
    oled_print(String(result,1));
    return result;
    */
    return calculateTemp(readCounter());
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
void OLED_print(options opt = NORMAL, double temp = 0, bool format = true){ // PRINT WITH ONE PRECISION
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE); 

  if(opt == SHOWTEMP) {
    display.setTextSize(3);
    display.setCursor(0,5);                         
    //display.println("Your temperature is:"); // might not be println - try print
    //display.setCursor(0,15);
    if(format){
      display.print(String(temp,1));
      display.setCursor(110, 5);
      display.println("C");
    }
    else{
      display.print(String(temp * 1.8 + 32,1));
      display.setCursor(110, 5);
      display.println("F");
    }
  }
  else if(opt == MEASURING){
    display.setCursor(0,0);
    display.println("Measuring...");
  }
  else if(opt == INVALID) {
    display.setCursor(0,0);
    display.println("Measuring invalid!");
  }
  else {
    display.setCursor(0, 0);
    display.println("Please contact");
    display.setCursor(0,15);
    display.println("to measure");
  }
  display.display();
  delay(3000);
}


/* for debugging purposes */
void oled_print(String message) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println(message);
  display.display();
  delay(3000);
}
