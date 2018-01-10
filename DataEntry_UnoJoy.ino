// Initial Commit
//   Katie, Keller, Colin

#include "UnoJoy.h"

int pinButtons[] = {9,8,7,6,5,4,3,2};
int pinID[]     = {12,11,10};

#define DEBOUNCE_TIME 10

byte  btID;
byte  btRawBTNS;          // actual state of buttons
byte  btDebounceTimer[8]; // debounce timer for buttons
byte  btBTNS;             // debounced state of buttons

int rawReads[8];
int newState[8];

//**********************************************************************
//  Initialize Hardware
//
void setup() 
{
  for (int i=2; i<12; i++)
  {
    pinMode(i, INPUT);
    digitalWrite(i, HIGH);
  }

//  btID = digitalRead(pinID[0]) << 0 |
//         digitalRead(pinID[1]) << 1 |
//         digitalRead(pinID[2]) << 2;
        
  setupUnoJoy();
   Serial.begin(38400);
}

//**********************************************************************
//  Main Processing Loop
//
void loop() 
{
  ScanIO();
  debounce2();
  //DebounceButtons();
//  Serial.print(btRawBTNS, HEX);
  UpdateUnoJoy();
}



//**********************************************************************
//  Read pushbuttons and write LEDs
//
void ScanIO(void)
{
  /*
    btRawBTNS = 0;
    for (int i=0; i<8; i++)
    {
      btRawBTNS |= !digitalRead(pinButtons[i]) << i;
    }
    */

    rawReads[0] = !digitalRead(2);
    rawReads[1] = !digitalRead(3);
    rawReads[2] = !digitalRead(4);
    rawReads[3] = !digitalRead(5);
    rawReads[4] = !digitalRead(6);
    rawReads[5] = !digitalRead(7);
    rawReads[6] = !digitalRead(8);
    rawReads[7] = !digitalRead(9);
    
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

void debounce2(void)
{
  int previous[8];
  static uint32_t lastTime;
  
  uint32_t currentTime = millis();
  uint32_t deltaTime = currentTime - lastTime;
  
  // return if delta < 1 mSec
  if (deltaTime == 0)
    return;

    for (int i=0; i<8; i++)
  {

    // check next bit's debounceTimer
    if (btDebounceTimer[i] > deltaTime)
    {
      // still active, update and continue
      btDebounceTimer[i] -= deltaTime;
//      Serial.print(" debouncing ");
//      Serial.println(btDebounceTimer[i]);
      continue;
    }
    else if (btDebounceTimer[i] > 0)
    {
      // debounce timed out
      btDebounceTimer[i] = 0;
//      Serial.print(" zero timer");
      if (previous[i] != rawReads[i])
      {
        if (rawReads[i] == 0)
        {
          newState[i] = 0;
        }
        else
        {
          newState[i] = 1;
        } 
      }
    }
    else
    {
      // start debounc timer
      if (previous[i] != rawReads[i])
        btDebounceTimer[i] = DEBOUNCE_TIME;
    }  
  }
  for(int i = 0; i < 8; i++)
  {
    previous[i] = newState[i];
  }
  
}
//**********************************************************************
//  Process button presses
//
void UpdateUnoJoy()
{
    // Always be getting fresh data
  dataForController_t controllerData = getControllerData();
  setControllerData(controllerData);
//  Serial.println(btBTNS, HEX);
 // printControllerData(controllerData);
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
  controllerData.triangleOn   = newState[0];
  controllerData.circleOn     = newState[1];
  controllerData.squareOn     = newState[2];
  controllerData.crossOn      = newState[3];
  controllerData.dpadUpOn     = newState[4];
  controllerData.dpadDownOn   = newState[5];
  controllerData.dpadLeftOn   = newState[6];
  controllerData.dpadRightOn  = newState[7];

  controllerData.l1On = 0;
  controllerData.r1On = 0;
  controllerData.selectOn = 0;
  controllerData.startOn = 0;
  controllerData.homeOn = 0;
  
  // Set the analog sticks
  //  Since analogRead(pin) returns a 10 bit value,
  //  we need to perform a bit shift operation to 
  //  lose the 2 least significant bits and get an
  //  8 bit number that we can use  
  controllerData.leftStickX  = 128;
  controllerData.leftStickY  = 128;
  controllerData.rightStickX = 128;
  controllerData.rightStickY = 128;
  // And return the data!
  return controllerData;
}

//void printControllerData(dataForController_t controllerData)
//{
//  byte btID = (controllerData.l1On << 0) +
//              (controllerData.r1On << 1) +
//              (controllerData.selectOn << 2);
//  Serial.print(" ID:");
//  Serial.print(btID, HEX);
//  Serial.print(" T:");
//  Serial.print(controllerData.triangleOn);
//  Serial.print(" C-");
//  Serial.print(controllerData.circleOn);
//  Serial.print(" S-");
//  Serial.print(controllerData.squareOn);
//  Serial.print(" C-");
//  Serial.print(controllerData.crossOn);
//  Serial.print(" U-");
//  Serial.print(controllerData.dpadUpOn);
//  Serial.print(" D-");
//  Serial.print(controllerData.dpadDownOn);
//  Serial.print(" L-");
//  Serial.print(controllerData.dpadLeftOn);
//  Serial.print(" R-");
//  Serial.println(controllerData.dpadRightOn);
// }  
