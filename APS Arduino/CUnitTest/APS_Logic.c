#include <stdlib.h>
#include <string.h>

#include "APS_Glucose_Status.h"
#include "APS_Temperature.h"
#include "APS_IOB_Data.h"
#include "APS_Profile.h"
#include "APS_Autosens.h"
#include "APS_Meal_Data.h"
#include "APS_basal-set-temp.h"

#include "APS_Logic.h"

/*	Implementation of oref0 from https://github.com/openaps/oref0/blob/master/lib/determine-basal/determine-basal.js	*/

// Define various functions used later on, in the main function determine_basal() below

// Rounds value to 'digits' decimal places
double APS_round(double value, unsigned long digits)
{
	long scale = powl(10, digits);
	return roundl(value * scale) / scale;
}

// we expect BG to rise or fall at the rate of BGI,
// adjusted by the rate at which BG would need to rise /
// fall to get eventualBG to target over 2 hours
double calculate_expected_delta(double target_bg, double eventual_bg, double bgi) {
	// (hours * mins_per_hour) / 5 = how many 5 minute periods in 2h = 24
	double five_min_blocks = (2 * 60) / 5;
	double target_delta = target_bg - eventual_bg;
	return /* expectedDelta */ APS_round(bgi + (target_delta / five_min_blocks), 1);
}

double convert_bg(double value, Profile profile)
{
	if (strcmp(profile.out_units, "mmol/L") == 0)
	{
		/*	NJIT - The original code included .toFixed(1) to force rounding even with 0; doesn't matter in C.	*/
		return APS_round(value / 18, 1);
	}
	else
	{
		return roundl(value);
	}
}