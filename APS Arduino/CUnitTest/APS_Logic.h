#pragma once
#ifdef __cplusplus
extern "C" {
#endif
	/*	Translation of Javascript to Python of the Open APS algorithm (oref0)
	 *	Unless specified, all comnments are from original source code. 
	https://github.com/openaps/oref0/blob/master/lib/determine-basal/determine-basal.js
	*/

#include <math.h>
#include "APS_Glucose_Status.h"
#include "APS_Temperature.h"
#include "APS_IOB_Data.h"
#include "APS_Profile.h"
#include "APS_Autosens.h"
#include "APS_Meal_Data.h"
#include "APS_basal-set-temp.h"


/// <summary>
/// Creates a blank profile for testing.
/// </summary>
/// <returns></returns>
Profile Create_Profile();

/// <summary>
/// Creates a blank Glucose status for testing.
/// </summary>
/// <returns></returns>
Glucose_Status Create_GlucoseStatus();

/// <summary>
/// Creates a blank Temp for testing.
/// </summary>
/// <returns></returns>
Temp Create_Temp();

IOB_Data Create_IOB_Data();

/// <summary>
/// Creates a blank Autosens for testing.
/// </summary>
/// <returns></returns>
Autosens Create_Autosens();

/// <summary>
/// Creates a blank MealData for testing.
/// </summary>
/// <returns></returns>
Meal_Data Create_MealData();

void InitAPS();

/*	Due to conflict with round() from <math.h>, this function has been renamed	*/
//	Rounds value to 'digits' decimal places
double APS_round(double value, unsigned long digits);

// we expect BG to rise or fall at the rate of BGI,
// adjusted by the rate at which BG would need to rise /
// fall to get eventualBG to target over 2 hours
double calculate_expected_delta(double target_bg, double eventual_bg, double bgi);

double convert_bg(double value, Profile profile);

//	NJIT - Not implemented right now.
/*	double enable_smb()	*/

/*	Source code includes additional variables for microBolusAllowed, reservoid_data, and currentTime 
	Also an addition variable for length of iob_data array
*/
Temp determine_basal(Glucose_Status glucose_status, Temp currenttemp, IOB_Data* iob_data,
	Profile profile, Autosens autosens_data, Meal_Data meal_data, TempBasalFunctions tempBasalFunctions, long iobLength);
#ifdef __cplusplus
}
#endif

