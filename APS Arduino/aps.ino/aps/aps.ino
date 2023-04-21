/*  APS Button Pins for Arduino NANO 33 IoT */

#include "APS_Data.h"
#include "APS_Logic.h"

#define SERIAL_TRANSMITTER 0
#define SERIAL_RECEIVER 1
#define APS_BUTTON1  6
#define APS_BUTTON2  7
#define APS_BUTTON3  18

/*  Global singleton of APS data  */
APS_Data apsData;


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
  //  Serial.Begin(9600); But we don't have a CGM; we only assume it exists
}

/*  Reads in CGM (Continuous Glucose Monitor) data from the USB serial ports. */
CGM_Data ReadCGMData()  {
  //  We assume the CGM exists and we are reading some data
  CGM_Data data;
  data.Average_Glucose = 75;
  data.TIR = .50;
  data.GV = 5;
}
void setup() {
  InitInsulinPump();
  InitCGM();
}
void loop() {
  CGM_Data cgmData = ReadCGMData();
  UpdateData(&apsData, cgmData);
  CalculatePrediction(&apsData);
  CalculateInsulinRate(&apsData);
  //DeliverInsulin  - not implemented yet
  delay(1000 * 60); //  Sleep for 1 minute 
}

