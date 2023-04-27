#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "APS_Logic.h"
#include "APS_Glucose_Status.h"
#include "APS_Temperature.h"
#include "APS_IOB_Data.h"
#include "APS_Profile.h"
#include "APS_Autosens.h"
#include "APS_Meal_Data.h"
#include "APS_basal-set-temp.h"
#include "APS_round-basal.h"

/*  APS Button Pins for Arduino NANO 33 IoT */

#define SERIAL_TRANSMITTER 0
#define SERIAL_RECEIVER 1
#define APS_BUTTON1  6  //  send a '1'
#define APS_BUTTON2  7  //  send a '0'
#define APS_BUTTON3  18 //  send command to deliver dosage

#define INSULIN_INTERVAL  (1000 * 60 * 5) //  amount of time for how long it takes to recalculate required basal: default 5 min.
//  CGM varaibles
Glucose_Status* glucose_status;
IOB_Data* iob_data;

//  User Data
Profile* profile;
Autosens* autosens;
Meal_Data* meal_data;

//  Current Insulin Rates
Temp* currenttemp;


/*  Initialize communication with Insulin Pump  */
void InitInsulinPump()  {
  /*  Setup output direction for pins */
  pinMode(APS_BUTTON1, OUTPUT);
  pinMode(APS_BUTTON2, OUTPUT);
  pinMode(APS_BUTTON3, OUTPUT);

  /*  Setup APS library */
  InitAPS();

  /*  Setup internal variables  */
  glucose_status = (Glucose_Status*)malloc(sizeof(Glucose_Status));

  iob_data = (IOB_Data*)malloc(sizeof(IOB_Data));
  
  currenttemp = (Temp*)malloc(sizeof(Temp));
  
  profile = (Profile*)malloc(sizeof(Profile));
  
  autosens = (Autosens*)malloc(sizeof(Autosens));
  
  meal_data = (Meal_Data*)malloc(sizeof(Meal_Data));
  
  Glucose_Status tempg = Create_GlucoseStatus();
  
  IOB_Data tempi = Create_IOB_Data();
  
  Temp tempt = Create_Temp();

  Profile tempp = Create_Profile();
  
  Autosens tempa = Create_Autosens();

  Meal_Data tempm = Create_MealData();

  /*  User Data variables are initialized with default test parameters  */
  tempp.max_iob = 2.5;
  tempp.dia = 3;
  snprintf(tempp.type, sizeof(tempp.type), "current");
  tempp.current_basal = .9;
  tempp.max_daily_basal = 1.3;
  tempp.max_basal = 3.5;
  tempp.max_bg = 120;
  tempp.min_bg = 110;
  tempp.sens = 40;
  tempp.carb_ratio = 10;

  tempa.ratio = 1.0;

  tempm.carbs = 50;
  tempm.nsCarbs = 50;
  tempm.bwCarbs = 0;
  tempm.journalCarbs = 0;
  tempm.mealCOB = 0;
  tempm.currentDeviation = 0;
  tempm.maxDeviation = 0;
  tempm.minDeviation = 0;
  tempm.slopeFromMaxDeviation = 0;
  tempm.slopeFromMinDeviation = 0;
  tempm.allDeviations = (double*)malloc(sizeof(double) * 5);
  tempm.allDeviations[0] = 0;
  tempm.allDeviations[1] = 0;
  tempm.allDeviations[2] = 0;
  tempm.allDeviations[3] = 0;
  tempm.allDeviations[4] = 0;
  tempm.bwFound = 0;

  
  memmove(glucose_status, &tempg, sizeof(Glucose_Status));
  memmove(iob_data, &tempi, sizeof(IOB_Data));
  memmove(currenttemp, &tempt, sizeof(Temp));
  memmove(profile, &tempp, sizeof(Profile));
  memmove(autosens, &tempa, sizeof(Autosens));
  memmove(meal_data, &tempm, sizeof(Meal_Data));
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
/*  An 8-bit number is translated into its individual bits to be sent to the pump
    The bits are sent from right to left.
*/
void SendNumberToPump(unsigned char number) {
  int i;
  for(i = 7; i >= 0; i--) {
    int mask = 1 << i;
    if((number & mask) == mask) //  == 1
      PressButton(APS_BUTTON1);
    else  //  == 2
      PressButton(APS_BUTTON2);
  }
  PressButton(APS_BUTTON3);
}
/*  Given the current insulin dosage, calculate the appropriate units to send and tell the pump to deliver it.
*/
void SendCommandToPump(Temp temp, Profile userProfile) {
  double rate = temp.rate; //  rate is in units/hour;
  double duration = temp.duration; //  duration is in minutes;
  double units;
  Serial.print("\nRate: ");
  Serial.print(rate);
  Serial.print(" |Duration: ");
  Serial.print(duration);
  Serial.print(" |Units: ");
  Serial.print(units);
  Serial.println();

  //  Safety check to prevent erroneous output
  if(isnan(rate) || isnan(duration) || rate == 0 || duration == 0)
  {
    Serial.print("No need to deliver dosage.");
    return;
  }
  int insulinIntervalToMinutes = INSULIN_INTERVAL / 1000 / 60; // dividing to minutes
  if(duration < INSULIN_INTERVAL) //  should finish by the time the next check occurs
  {
    //  units = number of 1/10th units insulin pump delivers for respective insulin interval
    units = rate * 10 //  units per hr * 10 (1/10th units) per unit
    / 60  //  1 hour per 60 minutes
    * (duration / insulinIntervalToMinutes) * insulinIntervalToMinutes
    ; // % active during insulin interval
  }
  else
  {
    //  units = number of 1/10th units insulin pump delivers for respective insulin interval
    units = rate * 10 //  units per hr * 10 (1/10th units) per unit
    / 60  //  1 hour per 60 minutes
    * insulinIntervalToMinutes; // 100% active during insulin interval
  }
  unsigned char roundedUnits = (unsigned char)round_basal(units, userProfile);

  Serial.print("\nUnits: ");
  Serial.print(units);
  Serial.print(" |rounded Units: ");
  Serial.print(roundedUnits);
  Serial.print("\n");

  char buffer[32];
  snprintf(buffer, sizeof(buffer), "Sending %u units\n", roundedUnits);
  Serial.print(buffer);

  
  Serial.print("Reason (if any): ");
  Serial.print(temp.reason);
  Serial.print("\n");

  Serial.print("Error (if any): ");
  Serial.print(temp.error);
  Serial.print("\n");
  SendNumberToPump(roundedUnits);
}
/*  Initialize communication between Arduino and Continuous Glucose Monitor (CGM).  
    For our project, we are assuming that a CGM device exists to deliver information to the insulin pump.
*/
void InitCGM()  {
   Serial.begin(9600);
}
Temp ReadDataFromCGM(Glucose_Status* gs, IOB_Data* id, Temp currenttemp) {
  /* Example test case #26: should high-temp when high and falling slower than BGI 
    Expected output of rate > 1 && duration > 30
  */
  gs->delta = -1;
  gs->glucose = 175;
  gs->long_avgdelta = -1;
  gs->short_avgdelta = -1;
  id->iob = 1;
  id->activity = 0.01;
  id->bolussnooze = 0;
  return determine_basal(*gs, currenttemp, id, *profile, *autosens, *meal_data, APS_tempBasalFunctions, 1);
}
void setup() {
InitInsulinPump();
InitCGM();
pinMode(LED_BUILTIN, OUTPUT);
}
void loop() {
 Temp newTemp = ReadDataFromCGM(glucose_status, iob_data, *currenttemp);
  memmove(currenttemp, &newTemp, sizeof(Temp));
  SendCommandToPump(*currenttemp, *profile);
//  delay(INSULIN_INTERVAL); //  1000 (ms/s) * 60 (s/min) * 5 = 5 

Serial.println("\nLOOP:     ");
digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(1000);                      // wait for a second
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
  delay(1000);   
}

