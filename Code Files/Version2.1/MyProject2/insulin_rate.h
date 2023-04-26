/* .h module containing functions to set and retreive insulin rate to determine delivery amount
   Code written by Matt Payne for UC summer scholarship as part the Insulin pump project,
   15/01/20
 */

#ifndef	INSULIN_RATE_H_INCLUDED
#define INSULIN_RATE_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

//	NJIT: We have modified the Insulin pump to take exclusive direction from a secondary device (an Arduino Nano 33 IoT)
//	We have repurposed pins / the original buttons to modify the insulin rate and then deliver it. View Buttons.c for more details.

//	The current insulin rate to be delivered, measured in terms of 1/10th insulin units.
extern uint8_t insulinDeliveryAmount;

void set_insulin_data(uint8_t insulin_rates[]);
uint8_t get_current_rate(uint8_t insulin_rates[], uint8_t address[]);

#ifdef __cplusplus
}
#endif
#endif