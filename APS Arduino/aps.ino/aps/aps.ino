/*  APS Button Pins for Arduino NANO 33 IoT */

#include "APS_Logic.h"

#define SERIAL_TRANSMITTER 0
#define SERIAL_RECEIVER 1
#define APS_BUTTON1  6
#define APS_BUTTON2  7
#define APS_BUTTON3  18

/*  Initialize communication with Insulin Pump  */
void InitInsulinPump()  {
  /*  Setup output direction for pins */
  pinMode(APS_BUTTON1, OUTPUT);
  pinMode(APS_BUTTON2, OUTPUT);
  pinMode(APS_BUTTON3, OUTPUT);
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

/*  Initialize communication between Arduino and Continuous Glucose Monitor (CGM).  */
void InitCGM()  {
  // Serial.begin(9600);
}
void setup() {
  //InitInsulinPump();
  //InitCGM();
  pinMode(13, OUTPUT);
}
void loop() {
  digitalWrite(13, HIGH);   
  delay(50);              
  digitalWrite(13, LOW);   
  delay(50); 
}

