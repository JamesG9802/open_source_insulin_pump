#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "APS_basal-set-temp.h"
#include "APS_round-basal.h"

#include "APS_Temperature.h"
#include "APS_Profile.h"

TempBasalFunctions APS_tempBasalFunctions;

void reason(Temp* rT, const char* msg) {
	snprintf((char* const)(rT->reason), (const size_t) sizeof(rT->reason), (const char* const)"%s%s%s", rT->reason, ". ", msg);
	printf("%s", msg);
}

double getMaxSafeBasal(Profile profile) {
	double max_daily_safety_multiplier = isnan(profile.max_daily_safety_multiplier) ? 3 : profile.max_daily_safety_multiplier;
	double current_basal_safety_multiplier = isnan(profile.current_basal_safety_multiplier) ? 4 : profile.current_basal_safety_multiplier;
	printf("GetMaxSafeBasal %.6lf %.6lf %.6lf %.6lf\n", profile.max_basal, max_daily_safety_multiplier, profile.max_daily_basal, current_basal_safety_multiplier);
	return min(profile.max_basal, min(max_daily_safety_multiplier * profile.max_daily_basal, current_basal_safety_multiplier * profile.current_basal));
}
Temp setTempBasal(double rate, double duration, Profile profile, Temp rT, Temp currenttemp) {
	double maxSafeBasal = APS_tempBasalFunctions.getMaxSafeBasal(profile);

	if (rate < 0) {
		rate = 0;
	}
	else if (rate > maxSafeBasal) {
		rate = maxSafeBasal;
	}
	double suggestedRate = round_basal(rate, profile);

	//	NJIT - original source code had a lot more checks to see if variables were valid
	if (currenttemp.duration > (duration - 10) && currenttemp.duration <= 120 && suggestedRate <= currenttemp.rate * 1.2 
		&& suggestedRate >= currenttemp.rate * 0.8 && duration > 0) {
		snprintf((char *const)(rT.reason), (const size_t)sizeof(rT.reason), (const char* const)"%s %.6lfm left and %.6lf ~ req %.6lf + U/hr no temp required", rT.reason,
			currenttemp.duration, currenttemp.rate, suggestedRate);
		return rT;
	}
	if (suggestedRate == profile.current_basal) {
		if (profile.skip_neutral_temps == 1) {
			if (!isnan(currenttemp.duration) && currenttemp.duration > 0) {
				reason(&rT, "Suggested rate is same as profile rate, a temp basal is active, canceling current temp");
				rT.duration = 0;
				rT.rate = 0;
				return rT;
			} else {
				reason(&rT, "Suggested rate is same as profile rate, no temp basal is active, doing nothing");
				return rT;
			}
		} else {
			char stringBuffer[128];
			snprintf(stringBuffer, sizeof(stringBuffer), "Setting neutral temp basal of %.6lfU/hr", profile.current_basal);
			reason(&rT, stringBuffer);
			rT.duration = duration;
			rT.rate = suggestedRate;
			return rT;
		}
	} else {
		rT.duration = duration;
		rT.rate = suggestedRate;
		return rT;
	}
}
/*	Initializer to set function pointers	*/
void APS_TempBasalFunctions_Init()
{
	APS_tempBasalFunctions.getMaxSafeBasal = getMaxSafeBasal;
	APS_tempBasalFunctions.setTempBasal = setTempBasal;
}