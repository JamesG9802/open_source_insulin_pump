/*  APS Button Pins for Arduino NANO 33 IoT */

#include "APS_Logic.h"

#define SERIAL_TRANSMITTER 0
#define SERIAL_RECEIVER 1
#define APS_BUTTON1  6  //  increase dosage by .1 units (a unit is .01 ml)
#define APS_BUTTON2  7  //  decrease dosage by .1 units
#define APS_BUTTON3  18 //  send command to deliver dosage



/*  Initialize communication with Insulin Pump  */
void InitInsulinPump()  {
  /*  Setup output direction for pins */
  pinMode(APS_BUTTON1, OUTPUT);
  pinMode(APS_BUTTON2, OUTPUT);
  pinMode(APS_BUTTON3, OUTPUT);

  /*  Setup APS library */
  InitAPS();
}

/*  Take in a button ID and send a signal for .1s to Insulin Pump System (IPS)  
    Does nothing if buttonID does not match existing pins.
*/
void PressButton(unsigned char buttonID)  {
  switch(buttonID)
  {
    case APS_BUTTON1:
    case APS_BUTTON2:
    case APS_BUTTON3:
      digitalWrite(buttonID, HIGH);
      delay(100);
      digitalWrite(buttonID, LOW);
    default:
      break;
  }
}

/*  Initialize communication between Arduino and Continuous Glucose Monitor (CGM).  
    For our project, we are assuming that a CGM device exists to deliver information to the insulin pump.
*/
void InitCGM()  {
   Serial.begin(9600);
}
void setup() {
  InitInsulinPump();
  InitCGM();
}
void loop() {
}

