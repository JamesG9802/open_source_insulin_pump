/* Module containing functions to interface Buttons, and also set up three external interrupts
   to those buttons.
   Code written by Matt Payne for University of Canterbury Final Year Project as part the Insulin pump project,
   Project supervised by Geoff Chase, Jennifer Knopp, Lui Holder Pearson and Jake Campbell.
   22/08/20
 */

#ifndef BUTTONS_H_INCLUDED
#define BUTTONS_H_INCLUDED

#ifdef __cplusplus
extern "C" {
	#endif

//	NJIT:	We have reworked the insulin pump to receive input from a secondary device (an Arduino Nano 33 IoT) that serves as
//	an Artificial Pancreas System (APS). As such, the arduino sends signals to the insulin pump by using reusing the buttons as
//	a method of communication. 3 other pins are also used to allow the secondary device to send electrical signals to the pump that
//	are treated as if the buttons are pressed. 
//	When Button 1 is pressed, the insulinDeliveryIndex is incremented by 1, insulinDelivery amount is left shifted by 1,
//	and a 1 is |ed to the insulinDeliveryAmount.
//	When Button 2 is pressed, the insulinDeliveryIndex is incremented by 1 and insulinDelivery amount is left shifted by 1.
//	When Button 3 is pressed, the main() function is called causing the insulinDelivery amount to be delivered. 
//	insulinDeliveryIndex is then reset to -1.
extern int8_t insulinDeliveryIndex;

// Initialise interrupts
void init_interrupts(void);

void TIMER_0_setup(void);

void init_button_array(void);

void set_leds(uint8_t button_count);

void leds_off(void);

	
#ifdef __cplusplus
}
#endif
#endif