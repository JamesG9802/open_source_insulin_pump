#pragma once

/*
This is the source Arduino code converted into C for testing.
*/

#include "Arduino.h"

/*  Start of Arduino code   */

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

/*  Take in a button ID and send a signal for .1s to Insulin Pump System (IPS)
    Does nothing if buttonID does not match existing pins.
*/
void PressButton(unsigned char buttonID) {
    switch (buttonID)
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
    for (i = 7; i >= 0; i--) {
        int mask = 1 << i;
        if ((number & mask) == mask) //  == 1
            PressButton(APS_BUTTON1);
        else  //  == 2
            PressButton(APS_BUTTON2);
    }
    PressButton(APS_BUTTON3);
}
/*  Given the current insulin dosage, calculate the appropriate units to send and tell the pump to deliver it.
*/
void SendCommandToPump(double rate, double duration) {

	double units;
    //  Safety check to prevent erroneous output
    if (isnan(rate) || isnan(duration) || rate == 0 || duration == 0)
    {
        return;
    }
    int insulinIntervalToMinutes = INSULIN_INTERVAL / 1000 / 60; // dividing to minutes
    if (duration < insulinIntervalToMinutes) //  should finish by the time the next check occurs
    {
        //  units = number of 1/10th units insulin pump delivers for respective insulin interval
        units = rate * 10 //  units per hr * 10 (1/10th units) per unit
            / 60  //  1 hour per 60 minutes
            * duration; // % active during insulin interval
    }
    else
    {
        //  units = number of 1/10th units insulin pump delivers for respective insulin interval
        units = rate * 10 //  units per hr * 10 (1/10th units) per unit
            / 60  //  1 hour per minute
            * insulinIntervalToMinutes; // 100% active during insulin interval
    }
    unsigned char roundedUnits = (unsigned char)round(units);
    SendNumberToPump(roundedUnits);
}

/*	This is the Insulin Pump Code converted for testing */
unsigned char button_array[3]; 
unsigned char basal_rate[1];
void init_button_array()
{
	button_array[0] = 0;
	button_array[1] = 0;
	button_array[2] = 0;
}

int TIMER_0_task1_cb(unsigned char button_1, unsigned char button_2, unsigned char button_3)
{
	//	We also use button_array[0] to represent the insulinDeliveryAmount
	unsigned char insulinDeliveryAmount = button_array[0];

	//	To minimize changes, we reuse the unused basal_rate to represent the insulinDeliveryIndex
	unsigned char insulinDeliveryIndex = basal_rate[0];

	//	When Button 1 is pressed, the insulinDeliveryIndex is incremented by 1, insulinDelivery amount is left shifted by 1,
	//	and a 1 is |ed to the insulinDeliveryAmount.
	if (button_1)
	{
		/*
		button_array[0]++;
		button_1 = false;
		timer_stop(&TIMER_0);
		flash_erase(&FLASH_0, flash_address, 1);
		store_delivery_data();
		set_leds(button_array[0]);
		*/
		if (insulinDeliveryIndex >= 7)	//	insulinDeliveryAmount is 8 bit number and so trying to add more digits should do nothing
			;
		else
		{
			insulinDeliveryIndex++;
			insulinDeliveryAmount <<= 1;
			insulinDeliveryAmount |= 1;
		}


		//	Calling preexisting functions
		button_array[0] = insulinDeliveryAmount;
		basal_rate[0] = insulinDeliveryIndex;
		return -1;
	}
	//	When Button 2 is pressed, the insulinDeliveryIndex is incremented by 1 and insulinDelivery amount is left shifted by 1.
	else if (button_2)
	{
		/*
		button_array[0]--;
		button_2 = false;
		timer_stop(&TIMER_0);
		flash_erase(&FLASH_0, flash_address, 1);
		store_delivery_data();
		set_leds(button_array[0]);
		*/
		if (insulinDeliveryIndex >= 7)	//	insulinDeliveryAmount is 8 bit number and so trying to add more digits should do nothing
			;
		else
		{
			insulinDeliveryIndex++;
			insulinDeliveryAmount <<= 1;
		}

		//	Calling preexisting functions
		button_array[0] = insulinDeliveryAmount;
		basal_rate[0] = insulinDeliveryIndex;
		return -1;
	}
	//	When Button 3 is pressed, the main() function is called causing the insulinDelivery amount to be delivered.
	//	insulinDeliveryIndex is then reset to -1.
	else if (button_3)
	{
		button_array[2]++;
		printf("Number of units: %d\n", insulinDeliveryAmount);
		return insulinDeliveryAmount;
	}
}