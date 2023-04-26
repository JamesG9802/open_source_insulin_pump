#include <math.h>

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
Glucose_Status glucose_status;
IOB_Data iob_data;

//  User Data
Profile profile;
Autosens autosens;
Meal_Data meal_data;

//  Current Insulin Rates
Temp currenttemp;


/*  Initialize communication with Insulin Pump  */
void InitInsulinPump()  {
  /*  Setup output direction for pins */
  pinMode(APS_BUTTON1, OUTPUT);
  pinMode(APS_BUTTON2, OUTPUT);
  pinMode(APS_BUTTON3, OUTPUT);

  /*  Setup APS library */
  InitAPS();

  /*  Setup internal variables  */
  glucose_status = Create_GlucoseStatus();
  iob_data = Create_IOB_Data();
  currenttemp = Create_Temp();
  profile = Create_Profile();
  autosens = Create_Autosens();
  meal_data = Create_MealData();

  /*  User Data variables are initialized with default test parameters  */
  profile.max_iob = 2.5;
  profile.dia = 3;
  snprintf(profile.type, sizeof(profile.type), "current");
  profile.current_basal = .9;
  profile.max_daily_basal = 1.3;
  profile.max_basal = 3.5;
  profile.max_bg = 120;
  profile.min_bg = 110;
  profile.sens = 40;
  profile.carb_ratio = 10;

  autosens.ratio = 1.0;

  meal_data.carbs = 50;
  meal_data.nsCarbs = 50;
  meal_data.bwCarbs = 0;
  meal_data.journalCarbs = 0;
  meal_data.mealCOB = 0;
  meal_data.currentDeviation = 0;
  meal_data.maxDeviation = 0;
  meal_data.minDeviation = 0;
  meal_data.slopeFromMaxDeviation = 0;
  meal_data.slopeFromMinDeviation = 0;
  meal_data.allDeviations = (double*)malloc(sizeof(double) * 5);
  meal_data.allDeviations[0] = 0;
  meal_data.allDeviations[1] = 0;
  meal_data.allDeviations[2] = 0;
  meal_data.allDeviations[3] = 0;
  meal_data.allDeviations[4] = 0;
  meal_data.bwFound = 0;
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
void SendCommandToPump(Temp currenttemp, Profile profile) {
  double rate = currenttemp.rate; //  rate is in units/hour;
  double duration = currenttemp.duration; //  duration is in minutes;
  double units;
  //  Safety check to prevent erroneous output
  if(isnan(rate) || isnan(duration) || rate == 0 || duration == 0)
  {
    Serial.print("No need to deliver dosage.");
    return;
  }
  if(duration < INSULIN_INTERVAL) //  should finish by the time the next check occurs
  {
    //  units = number of 1/10th units insulin pump delivers for respective insulin interval
    units = rate * 10 //  units per hr * 10 (1/10th units) per unit
    / 60  //  1 hour per 60 minutes
    * (duration / INSULIN_INTERVAL) * INSULIN_INTERVAL; // % active during insulin interval
  }
  else
  {
    //  units = number of 1/10th units insulin pump delivers for respective insulin interval
    units = rate * 10 //  units per hr * 10 (1/10th units) per unit
    / 60  //  1 hour per 60 minutes
    * INSULIN_INTERVAL; // 100% active during insulin interval
  }
  unsigned char roundedUnits = (unsigned char)round_basal(units, profile);
  SendNumberToPump(roundedUnits);
}
/*  Initialize communication between Arduino and Continuous Glucose Monitor (CGM).  
    For our project, we are assuming that a CGM device exists to deliver information to the insulin pump.
*/
void InitCGM()  {
   Serial.begin(9600);
}
Temp ReadDataFromCGM(Glucose_Status* glucose_status, IOB_Data* iob_data, Temp currenttemp) {
  /* Example test case #26: should high-temp when high and falling slower than BGI 
    Expected output of rate > 1 && duration > 30
  */
  glucose_status->delta = -1;
  glucose_status->glucose = 175;
  glucose_status->long_avgdelta = -1;
  glucose_status->short_avgdelta = -1;
  iob_data->iob = 1;
  iob_data->activity = 0.01;
  iob_data->bolussnooze = 0;
  return determine_basal(*glucose_status, currenttemp, iob_data, profile, autosens, meal_data, APS_tempBasalFunctions, 1);
}
void setup() {
  InitInsulinPump();
  InitCGM();
}
void loop() {
  currenttemp = ReadDataFromCGM(&glucose_status, &iob_data, currenttemp);
  delay(INSULIN_INTERVAL); //  1000 (ms/s) * 60 (s/min) * 5 = 5 
}

