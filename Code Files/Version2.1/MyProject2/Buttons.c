/* Module containing functions to interface Buttons, and also set up three external interrupts
   linked to those buttons.
   
   Code written by Matt Payne for University of Canterbury Final Year Project as part the Insulin pump project,
   Project supervised by Geoff Chase, Jennifer Knopp, Lui Holder Pearson and Jake Campbell.
   
   22/08/20
 */

#include "atmel_start_pins.h"
#include "stdint.h"
#include "hal_ext_irq.h"
#include "hal_timer.h"
#include "stdbool.h"
#include "driver_init.h"
#include "utils.h"
#include "Buttons.h"
#include "insulin_rate.h"

/**********************************************************************************************
 * Declare variables
**********************************************************************************************/

//	NJIT:	We have reworked the insulin pump to receive input from a secondary device (an Arduino Nano 33 IoT) that serves as
//	an Artificial Pancreas System (APS). As such, the arduino sends signals to the insulin pump by using reusing the buttons as
//	a method of communication. 3 other pins are also used to allow the secondary device to send electrical signals to the pump that
//	are treated as if the buttons are pressed.
//	When Button 1 is pressed, the insulinDeliveryIndex is incremented by 1, insulinDelivery amount is left shifted by 1,
//	and a 1 is |ed to the insulinDeliveryAmount.
//	When Button 2 is pressed, the insulinDeliveryIndex is incremented by 1 and insulinDelivery amount is left shifted by 1.
//	When Button 3 is pressed, the main() function is called causing the insulinDelivery amount to be delivered.
//	insulinDeliveryIndex is then reset to -1.
//	We reuse button_array[0] to act as insulinDeliveryAmount
extern uint8_t button_array[3];

//	NJIT: We have modified the Insulin pump to take exclusive direction from a secondary device (an Arduino Nano 33 IoT)
//	We have repurposed pins / the original buttons to modify the insulin rate and then deliver it. View Buttons.c for more details.

//	The current insulin rate to be delivered, measured in terms of 1/10th insulin units.
//	We reuse basal_rate[0] to act as insulinDeliveryIndex
extern uint8_t basal_rate[1];
extern uint16_t flash_address;
bool button_1 = false;
bool button_2 = false;
bool button_3 = false;
static struct timer_task TIMER_0_task1;

/**********************************************************************************************
 * Function definitions
**********************************************************************************************/

//	NJIT: Why are these functions throwing implicit function declaration now?
void store_delivery_data();
int main(void);

void init_button_array()
{
	button_array[0] = 0;
	button_array[1] = 0;
	button_array[2] = 0;
}
// cb function called when timer runs down. Checks a set of bools to see which button was
// pressed and increments an array entry linked to that button. Array is checked in main.
static void TIMER_0_task1_cb(const struct timer_task *const timer_task)
	{
		//	We also use button_array[0] to represent the insulinDeliveryAmount
		uint8_t insulinDeliveryAmount = button_array[0];
	
		//	To minimize changes, we reuse the unused basal_rate to represent the insulinDeliveryIndex
		uint8_t insulinDeliveryIndex = basal_rate[0];
		
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
		if(insulinDeliveryIndex >= 7)	//	insulinDeliveryAmount is 8 bit number and so trying to add more digits should do nothing
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
		button_1 = false;
		timer_stop(&TIMER_0);
		flash_erase(&FLASH_0, flash_address, 1);
		store_delivery_data();
		set_leds(button_array[0]);
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
		if(insulinDeliveryIndex >= 7)	//	insulinDeliveryAmount is 8 bit number and so trying to add more digits should do nothing
		;
		else
		{
			insulinDeliveryIndex++;
			insulinDeliveryAmount <<= 1;
		}
		
		//	Calling preexisting functions
		button_array[0] = insulinDeliveryAmount;
		basal_rate[0] = insulinDeliveryIndex;
		button_2 = false;
		timer_stop(&TIMER_0);
		flash_erase(&FLASH_0, flash_address, 1);
		store_delivery_data();
		set_leds(button_array[0]);
	}
	//	When Button 3 is pressed, the main() function is called causing the insulinDelivery amount to be delivered.
	//	insulinDeliveryIndex is then reset to -1.
	else if  (button_3)
	{
		button_array[2]++;
		button_3 = false;
		main();
		insulinDeliveryIndex = -1;
		insulinDeliveryAmount = 0;
		
		button_array[0] = insulinDeliveryAmount;
		basal_rate[0] = insulinDeliveryIndex;
		flash_erase(&FLASH_0, flash_address, 1);
		store_delivery_data();
	}
}

// Initialisation for TIMER_0 peripheral
void TIMER_0_setup(void)
{
	TIMER_0_task1.interval = 10;
	TIMER_0_task1.cb       = TIMER_0_task1_cb;
	TIMER_0_task1.mode     = TIMER_TASK_REPEAT;
	
	timer_add_task(&TIMER_0, &TIMER_0_task1);
}

// cb function which starts TIMER_0 when button 1 pressed
static void button_1_cb(void)
{
	button_1 = true;
	timer_start(&TIMER_0);
}

// cb function which starts TIMER_0 when button 2 pressed
static void button_2_cb(void)
{
	button_2 = true;
	timer_start(&TIMER_0);
}

// cb function which starts TIMER_0 when button 3 pressed
static void button_3_cb(void)
{
	button_3 = true;
	timer_start(&TIMER_0);
}


// Initialise external interrupts, and tie them to callback functions.
// Interrupts are disabled and then enabled, otherwise they are triggered
// on startup.
void init_interrupts(void)
{
	ext_irq_disable(BUTTON1);
	ext_irq_disable(BUTTON2);
	ext_irq_disable(BUTTON3);
	ext_irq_register(BUTTON2, button_2_cb);
	ext_irq_register(BUTTON1, button_1_cb);
	ext_irq_register(BUTTON3, button_3_cb);
	ext_irq_enable(BUTTON1);
	ext_irq_enable(BUTTON2);
	ext_irq_enable(BUTTON3);
	
	/*	CS 490 James	*/
	ext_irq_disable(APS_BUTTON1);
	ext_irq_disable(APS_BUTTON2);
	ext_irq_disable(APS_BUTTON3);
	ext_irq_register(APS_BUTTON2, button_2_cb);
	ext_irq_register(APS_BUTTON1, button_1_cb);
	ext_irq_register(APS_BUTTON3, button_3_cb);
	ext_irq_enable(APS_BUTTON1);
	ext_irq_enable(APS_BUTTON2);
	ext_irq_enable(APS_BUTTON3);
}

void set_leds(uint8_t button_count)
{
	if(button_count < 1) 
	{
		gpio_set_pin_level(OUT7, false);
		gpio_set_pin_level(OUT1, false);
		gpio_set_pin_level(OUT6, false);
		gpio_set_pin_level(OUT4, false);
	}
	if (button_count == 1 )
	{
		gpio_set_pin_level(OUT7, true);
		gpio_set_pin_level(OUT1, false);
		gpio_set_pin_level(OUT6, false);
		gpio_set_pin_level(OUT4, false);
		
	}
	if (button_count == 2 )
	{
		gpio_set_pin_level(OUT7, true);
		gpio_set_pin_level(OUT1, true);
		gpio_set_pin_level(OUT6, false);
		gpio_set_pin_level(OUT4, false);
		
	}
	if (button_count == 3 )
	{
		gpio_set_pin_level(OUT7, true);
		gpio_set_pin_level(OUT1, true);
		gpio_set_pin_level(OUT6, true);
		gpio_set_pin_level(OUT4, false);
		
	}
	if (button_count == 4 )
	{
		gpio_set_pin_level(OUT7, true);
		gpio_set_pin_level(OUT1, true);
		gpio_set_pin_level(OUT6, true);
		gpio_set_pin_level(OUT4, true);
	}
}

void leds_off(void)
{
	gpio_set_pin_level(OUT1, false);
	gpio_set_pin_level(OUT6, false);
	gpio_set_pin_level(OUT7, false);
	gpio_set_pin_level(OUT4, false);
}




