// Initial Commit
//   Katie, Kelly, Colin

#include "UnoJoy.h"

int pinButtons[] = {9,8,7,6,5,4,3,2};

#define DEBOUNCE_TIME 10
byte  btRawBTNS;          // actual state of buttons
byte  btDebounceTimer[8]; // debounce timer for buttons
byte  btBTNS;             // debounced state of buttons

//**********************************************************************
//  Initialize Hardware
//
void setup() 
{
  for (int i=0; i<8; i++)
  {
    pinMode(pinButtons[i], INPUT_PULLUP);
  }
    
  Serial.begin(115200);
  Serial.println("Hello World");
//  setupUnoJoy();
}

//**********************************************************************
//  Main Processing Loop
//
void loop() 
{
  ScanIO();
  DebounceButtons();
  Serial.print(btRawBTNS, HEX);
  Serial.print(" ");
  Serial.println(btBTNS, HEX);
  UpdateUnoJoy();

  delay(500);
}



//**********************************************************************
//  Read pushbuttons and write LEDs
//
void ScanIO(void)
{
    btRawBTNS = 0;
    for (int i=0; i<8; i++)
    {
      btRawBTNS |= !digitalRead(pinButtons[i]) << i;
    }
}

//********************************************************************
// DebounceButtons
//
//  This routine eliminates spurious changes on a button input if the 
//  contact inside the button bounces before settling. A timer is started
//  when a button input first changes and any subsequent changes will be
//  ignored until the timer has timed out. Each button has its own timer.
//  The timeout value can be adjusted for noisy buttons, but most button
//  switches will settle within 10 mSec.
//
void DebounceButtons(void)
{
  static uint32_t lastMillis;
  static byte lastBTNS;

  // determine how many mSecs have passed since last call
  uint32_t currentMillis = millis();
  uint32_t deltaMillis = currentMillis - lastMillis;
  
  // return if delta < 1 mSec
  if (deltaMillis == 0)
    return;

  // determine the bits that have changed
  byte changed = btRawBTNS ^ btBTNS;

  // update debounce timeouts
  for (byte i=0; i<8; i++)
  {
    byte bitMask = 0x01 << i;
//    Serial.print(bitMask, HEX);
    
    // check next bit's debounceTimer
    if (btDebounceTimer[i] > deltaMillis)
    {
      // still active, update and continue
      btDebounceTimer[i] -= deltaMillis;
//      Serial.print(" debouncing ");
//      Serial.println(btDebounceTimer[i]);
      continue;
    }
    else if (btDebounceTimer[i] > 0)
    {
      // debounce timed out
      btDebounceTimer[i] = 0;
//      Serial.print(" zero timer");
      if (changed & bitMask)
      {
        if ((btRawBTNS & bitMask) == 0)
        {
//          Serial.print(" clear bit ");
          btBTNS &= ~bitMask;
        }
        else
        {
//          Serial.print(" set bit ");
          btBTNS |= bitMask;
        } 
      }
    }
    else
    {
      // start debounc timer
      if (changed & bitMask)
        btDebounceTimer[i] = DEBOUNCE_TIME;
    }  
  }
  lastBTNS = btBTNS;
}

//**********************************************************************
//  Process button presses
//
void UpdateUnoJoy()
{
    // Always be getting fresh data
  dataForController_t controllerData = getControllerData();
  setControllerData(controllerData);
 Serial.println(btBTNS, HEX);
  printControllerData(controllerData);
}  

dataForController_t getControllerData(void)
{
  
  // Set up a place for our controller data
  //  Use the getBlankDataForController() function, since
  //  just declaring a fresh dataForController_t tends
  //  to get you one filled with junk from other, random
  //  values that were in those memory locations before
  dataForController_t controllerData = getBlankDataForController();
  // Since our buttons are all held high and
  //  pulled low when pressed, we use the "!"
  //  operator to invert the readings from the pins
  controllerData.triangleOn  = btBTNS & 1;
  controllerData.circleOn    = btBTNS & 2;
  controllerData.squareOn    = btBTNS & 4;
  controllerData.crossOn     = btBTNS & 8;
  controllerData.dpadUpOn    = btBTNS & 16;
  controllerData.dpadDownOn  = btBTNS & 32;
  controllerData.dpadLeftOn  = btBTNS & 64;
  controllerData.dpadRightOn = btBTNS & 128;
  //controllerData.l1On = !digitalRead(10);
  //controllerData.r1On = !digitalRead(11);
  //controllerData.selectOn = !digitalRead(12);
  //controllerData.startOn = !digitalRead(A4);
  //controllerData.homeOn = !digitalRead(A5);
  
  // Set the analog sticks
  //  Since analogRead(pin) returns a 10 bit value,
  //  we need to perform a bit shift operation to 
  //  lose the 2 least significant bits and get an
  //  8 bit number that we can use  
  controllerData.leftStickX  = analogRead(A0) >> 2;
  controllerData.leftStickY  = analogRead(A1) >> 2;
  controllerData.rightStickX = analogRead(A2) >> 2;
  controllerData.rightStickY = analogRead(A3) >> 2;
  // And return the data!
  return controllerData;
}

void printControllerData(dataForController_t controllerData)
{
  Serial.print("Triangle  : ");
  Serial.println(controllerData.triangleOn);
  Serial.print("Circle    : ");
  Serial.println(controllerData.circleOn);
  Serial.print("Square    : ");
  Serial.println(controllerData.squareOn);
  Serial.print("Cross     : ");
  Serial.println(controllerData.crossOn);
  Serial.print("PadUp     : ");
  Serial.println(controllerData.dpadUpOn);
  Serial.print("PadDown   : ");
  Serial.println(controllerData.dpadDownOn);
  Serial.print("Pad Left  : ");
  Serial.println(controllerData.dpadLeftOn);
  Serial.print("Pad Right : ");
  Serial.println(controllerData.dpadRightOn);
 }  
