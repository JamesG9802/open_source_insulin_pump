#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>

#include "APSList.h"

#include "APS_Glucose_Status.h"
#include "APS_Temperature.h"
#include "APS_IOB_Data.h"
#include "APS_Profile.h"
#include "APS_Autosens.h"
#include "APS_Meal_Data.h"
#include "APS_basal-set-temp.h"
#include "APS_round-basal.h"

#include "APS_Logic.h"

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif // !min

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif // !min

void InitAPS() {
	APS_TempBasalFunctions_Init();
}
Profile Create_Profile() {
	Profile profile;
	memset(profile.out_units, 0, sizeof(profile.out_units));	// no listed default (nld)
	profile.max_daily_safety_multiplier = 3;
	profile.current_basal_safety_multiplier = 4;
	profile.max_basal = NAN;	//	nld
	profile.max_daily_basal = NAN;	//	nld
	profile.current_basal = NAN;	//	nld
	memset(profile.model, 0, sizeof(profile.model));	//	nld
	profile.skip_neutral_temps = 0;	// default false
	profile.max_iob = 0;	//	default 0
	profile.min_bg = NAN;	//	nld
	profile.max_bg = NAN;	//	nld
	profile.exercise_mode = 0;	//	default false
	profile.high_temptarget_raises_sensitivity = 0;	//	default false
	profile.low_temptarget_lowers_sensitivity = 0;	//	default false
	profile.half_basal_exercise_target = 160;
	profile.temptargetSet = 0;	//	nld
	profile.autosens_max = 1.2;
	profile.sensitivity_raises_target = 0;	//	nld but implied false
	profile.resistance_lowers_target = 0;
	profile.noisyCGMTargetMultiplier = 1.3;
	profile.maxRaw = 0;	//	nld
	profile.sens = NAN;	//	nld
	profile.carb_ratio = NAN; //	nld
	profile.remainingCarbsCap = 90;
	profile.remainingCarbsFraction = 1;
	profile.carbsReqThreshold = 1;
	profile.dia = NAN;	//	nld
	memset(profile.type, 0, sizeof(profile.type));	//	nld
	return profile;
}
Glucose_Status Create_GlucoseStatus() {
	Glucose_Status status;
	status.delta = NAN;	//	no listed default (nld)
	status.glucose = NAN;	//	nld
	status.long_avgdelta = NAN;	//	nld
	status.short_avgdelta = NAN;	//	nld
	status.date = time(NULL);	//	nld	 but setting it to current time
	//status.date = 0;
	status.noise = 0;	//	nld
	memset(status.device, 0, sizeof(status.device));	// nld
	status.last_cal = NAN;	//	nld
	return status;
}
Temp Create_Temp() {
	Temp temp;
	temp.duration = NAN;	//	no listed default (nld)
	temp.rate = NAN;	//	nld
	memset(temp.temp, 0, sizeof(temp.temp));
	memset(temp.reason, 0, sizeof(temp.reason));
	memset(temp.error, 0, sizeof(temp.error));
	temp.deliverAt = 0;	//	nld
	temp.date = 0;	//	nld
	temp.bg = NAN;	//	nld
	temp.tick = NAN;	//	nld
	temp.eventualBG = NAN;	//	nld
	temp.insulinReq = 0;
	temp.sensitivityRatio = NAN;	//	nld
	temp.predBGs.IOB = NULL;
	temp.predBGs.COB = NULL;
	temp.COB = NAN;	//	nld
	temp.IOB = NAN;	//	nld
	temp.BGI = NAN;	//	nld
	temp.deviation = NAN;	//	nld
	temp.ISF = NAN;	//	nld
	temp.CR = NAN;	//	nld
	temp.target_bg = NAN;	//	nld
	temp.carbsReq = NAN;	//	nld
	return temp;
}
IOB_Data Create_IOB_Data() {
	IOB_Data iob_data;
	iob_data.iob = NAN;	//	no listed default (nld)
	iob_data.activity = NAN;	//	nld
	iob_data.bolussnooze = NAN;	//	nld
	Temp temp = Create_Temp();	//	nld
	memmove(&(iob_data.lastTemp), &temp, sizeof(Temp));
	iob_data.iobWithZeroTemp = NULL;
	iob_data.lastBolusTime = 0;	//	nld
	iob_data.basaliob = NAN;	//	nld
	iob_data.netbasalinsulin = NAN;	//	nld
	iob_data.hightempinsulin = NAN;	//	nld
	return iob_data;
}
Autosens Create_Autosens() {
	Autosens autosens;
	autosens.ratio = 1;
	return autosens;
}
Meal_Data Create_MealData() {
	Meal_Data mealData;
	mealData.carbs = NAN;	//	no listed default (nld)
	mealData.nsCarbs = NAN;	//	nld
	mealData.bwCarbs = NAN;	//	nld
	mealData.journalCarbs = NAN;	//	nld
	mealData.mealCOB = NAN;	//	nld
	mealData.currentDeviation = NAN;	//	nld
	mealData.maxDeviation = NAN;	//	nld
	mealData.minDeviation = NAN;	//	nld
	mealData.slopeFromMaxDeviation = NAN;	//	nld
	mealData.slopeFromMinDeviation = NAN;	//	nld
	mealData.allDeviations = NULL;	//	nld
	mealData.bwFound = 0;	//	nld
	mealData.lastCarbTime = 0;	//	nld
	memset(mealData.reason, 0, sizeof(mealData.reason));
	mealData.boluses = NAN;	//	nld
	return mealData;
}


/*	Implementation of oref0 from https://github.com/openaps/oref0/blob/master/lib/determine-basal/determine-basal.js	*/

// Define various functions used later on, in the main function determine_basal() below

// Rounds value to 'digits' decimal places
double APS_round(double value, unsigned long digits)
{
	//	Note that Javascript breaks ties with round to positive infinity while C's default round breaks ties with round to even.
	long long scale = powl(10, digits);
	if(value < 0 && (value - (long)(value) == -0.5))
		return roundl(value * scale + .01) / scale;
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
Temp determine_basal(Glucose_Status glucose_status, Temp currenttemp, IOB_Data* iob_data,
	Profile profile, Autosens autosens_data, Meal_Data meal_data, TempBasalFunctions tempBasalFunctions, long iobLength)
{
	// Set variables required for evaluating error conditions
	Temp rT = Create_Temp();

	time_t deliverAt = time(NULL);
	if (isnan(profile.current_basal))
	{
		snprintf((char* const)(rT.error), (const size_t)sizeof(rT.error), (const char* const)"Error: could not get current basal rate");
		return rT;
	}
	double profile_current_basal = round_basal(profile.current_basal, profile);
	double basal = profile_current_basal;

	time_t systemTime = time(NULL);

	time_t bgTime = (time_t)glucose_status.date;
	double minAgo = APS_round( (systemTime - bgTime) / 60 / 1000, 1);

	double bg = glucose_status.glucose;
	double noise = glucose_status.noise;

	// Prep various delta variables.
	double tick;

	/*	NJIT - Unclear why this is here in the source code */
	if (glucose_status.delta > -0.5) {
		tick = +APS_round(glucose_status.delta, 0);
	} else {
		tick = APS_round(glucose_status.delta, 0);
	}

	double minDelta = min(glucose_status.delta, glucose_status.short_avgdelta);
	double minAvgDelta = min(glucose_status.short_avgdelta, glucose_status.long_avgdelta);
	double maxDelta = max(glucose_status.delta, max(glucose_status.short_avgdelta, glucose_status.long_avgdelta));


	// Cancel high temps (and replace with neutral) or shorten long zero temps for various error conditions

		// 38 is an xDrip error state that usually indicates sensor failure
		// all other BG values between 11 and 37 mg/dL reflect non-error-code BG values, so we should zero temp for those
	// First, print out different explanations for each different error condition
	if (bg <= 10 || bg == 38 || noise >= 3) {  //Dexcom is in ??? mode or calibrating, or xDrip reports high noise
		snprintf((char* const)(rT.reason), (const size_t)sizeof(rT.reason), 
			(const char* const)"CGM is calibrating, in ??? state, or noise is high");
	}
	double tooflat = 0;
	if (bg > 60 && glucose_status.delta == 0 && glucose_status.short_avgdelta > -1 && glucose_status.short_avgdelta < 1 && glucose_status.long_avgdelta > -1 && glucose_status.long_avgdelta < 1) {
		if (strcmp(glucose_status.device, "fakecgm") == 0) {
			printf("CGM data is unchanged\n");
			printf("Simulator mode detected\n");
		}
		else {
			tooflat = 1;
		}
	}

	if (minAgo > 12 || minAgo < -5) { // Dexcom data is too old, or way in the future
	/*	const char timeString[512];		ctime_s does not work on Arduino Nano 33 IoT due to lack of Real Time Clock
		ctime_s(timeString, sizeof(timeString), &bgTime);
		snprintf((char* const)(rT.reason), (const size_t)sizeof(rT.reason),
			(const char* const)"If current system time is correct, then BG data is too old. The last BG data was read %.6lfm ago at %s", 
			minAgo, timeString);
		*/
		snprintf((char* const)(rT.reason), (const size_t)sizeof(rT.reason),
			(const char* const)"If current system time is correct, then BG data is too old. The last BG data was read ?m ago at %s",
			minAgo);
		// if BG is too old/noisy, or is changing less than 1 mg/dL/5m for 45m, cancel any high temps and shorten any long zero temps
	}
	else if (tooflat) {
		if (!isnan(glucose_status.last_cal) && glucose_status.last_cal < 3) {
			snprintf((char* const)(rT.reason), (const size_t)sizeof(rT.reason),
				(const char* const)"CGM was just calibrated");
		}
		else {
			snprintf((char* const)(rT.reason), (const size_t)sizeof(rT.reason),
				(const char* const)"CGM data is unchanged (%ld+%.6lf) for 5m w/ %.6lf mg/d: ~15 change & %.6lf mg/dL ~ 45m change"
				, bg, glucose_status.delta, glucose_status.short_avgdelta, glucose_status.long_avgdelta);
		}
	}
	// Then, for all such error conditions, cancel any running high temp or shorten any long zero temp, and return.
	if (bg <= 10 || bg == 38 || noise >= 3 || minAgo > 12 || minAgo < -5 || tooflat) {
		if (currenttemp.rate > basal) { // high temp is running
			snprintf((char* const)(rT.reason), (const size_t)sizeof(rT.reason),
				(const char* const)"%s. Replacing high temp basal of %.6lf with neutral temp of %ld"
				, rT.reason, currenttemp.rate, basal);
			rT.deliverAt = deliverAt;
			snprintf((char* const)(rT.temp), (const size_t)sizeof(rT.temp),(const char* const)"absolute");
			rT.duration = 30;
			rT.rate = basal;
			return rT;
			// don't use setTempBasal(), as it has logic that allows <120% high temps to continue running
			//return tempBasalFunctions.setTempBasal(basal, 30, profile, rT, currenttemp);
		}
		else if (currenttemp.rate == 0 && currenttemp.duration > 30) { //shorten long zero temps to 30m
			snprintf((char* const)(rT.reason), (const size_t)sizeof(rT.reason),
				(const char* const)"%s. Shortening %.6lfm long zero temp to 30m. "
				, rT.reason, currenttemp.duration);
			rT.deliverAt = deliverAt;
			snprintf((char* const)(rT.temp), (const size_t)sizeof(rT.temp), (const char* const)"absolute");
			rT.duration = 30;
			rT.rate = 0;
			return rT;
			// don't use setTempBasal(), as it has logic that allows long zero temps to continue running
			//return tempBasalFunctions.setTempBasal(0, 30, profile, rT, currenttemp);
		}
		else { //do nothing.
			snprintf((char* const)(rT.reason), (const size_t)sizeof(rT.reason),
				(const char* const)"%s. Temp %.6lf <= current basal %.6lfU/hr; doing nothing. "
				, rT.reason, currenttemp.rate, basal);
			return rT;
		}
	}
	// Get configured target, and return if unable to do so.
	// This should occur after checking that we're not in one of the CGM-data-related error conditions handled above,
	// and before using target_bg to adjust sensitivityRatio below.
	double max_iob = profile.max_iob; // maximum amount of non-bolus IOB OpenAPS will ever deliver


	// if min and max are set, then set target to their average
	double target_bg;
	double min_bg;
	double max_bg;
	double high_bg;

	if (!isnan(profile.min_bg)) {
		min_bg = profile.min_bg;
	}
	if (!isnan(profile.max_bg)) {
		max_bg = profile.max_bg;
	}
	/*	Used to be code about SMB	*/
	if (!isnan(profile.min_bg) && !isnan(profile.max_bg)) {
		target_bg = (profile.min_bg + profile.max_bg) / 2;
	} else {
		snprintf((char* const)(rT.error), (const size_t)sizeof(rT.error), (const char* const)"Error: could not determine target_bg. ");
		return rT;
	}

	// Calculate sensitivityRatio based on temp targets, if applicable, or using the value calculated by autosens
	double sensitivityRatio;
	double high_temptarget_raises_sensitivity = profile.exercise_mode || profile.high_temptarget_raises_sensitivity;
	double normalTarget = 100; // evaluate high/low temptarget against 100, not scheduled target (which might change)
	
	double halfBasalTarget;
	if (profile.half_basal_exercise_target != 0) {
		halfBasalTarget = profile.half_basal_exercise_target;
	}
	else {
		halfBasalTarget = 160; // when temptarget is 160 mg/dL, run 50% basal (120 = 75%; 140 = 60%)
		// 80 mg/dL with low_temptarget_lowers_sensitivity would give 1.5x basal, but is limited to autosens_max (1.2x by default)
	}
	if (high_temptarget_raises_sensitivity && profile.temptargetSet && target_bg > normalTarget
		|| profile.low_temptarget_lowers_sensitivity && profile.temptargetSet && target_bg < normalTarget) {
		// w/ target 100, temp target 110 = .89, 120 = 0.8, 140 = 0.67, 160 = .57, and 200 = .44
		// e.g.: Sensitivity ratio set to 0.8 based on temp target of 120; Adjusting basal from 1.65 to 1.35; ISF from 58.9 to 73.6
		//sensitivityRatio = 2/(2+(target_bg-normalTarget)/40);
		double c = halfBasalTarget - normalTarget;
		// getting multiplication less or equal to 0 means that we have a really low target with a really low halfBasalTarget
		// with low TT and lowTTlowersSensitivity we need autosens_max as a value
		// we use multiplication instead of the division to avoid "division by zero error"
		if (c * (c + target_bg - normalTarget) <= 0.0) {
			sensitivityRatio = profile.autosens_max;
		}
		else {
			sensitivityRatio = c / (c + target_bg - normalTarget);
		}
		// limit sensitivityRatio to profile.autosens_max (1.2x by default)
		sensitivityRatio = min(sensitivityRatio, profile.autosens_max);
		sensitivityRatio = APS_round(sensitivityRatio, 2);
		printf("Sensitivity ratio set to ");
	}
	else if(!isnan(autosens_data.ratio)) {
		sensitivityRatio = autosens_data.ratio;
		printf("Autosens ratio changed");
	}
	if (sensitivityRatio) {
		basal = profile.current_basal * sensitivityRatio;
		basal = round_basal(basal, profile);
		if (basal != profile_current_basal) {
			printf("Adjusting basal\n");
		}
		else {
			printf("Basal unchanged\n");
		}
	}
	// Conversely, adjust BG target based on autosens ratio if no temp target is running
	// adjust min, max, and target BG for sensitivity, such that 50% increase in ISF raises target from 100 to 120
	if (profile.temptargetSet) {
		//process.stderr.write("Temp Target set, not adjusting with autosens; ");
	}
	else if (!isnan(autosens_data.ratio)) {
		if (profile.sensitivity_raises_target && autosens_data.ratio < 1 || profile.resistance_lowers_target && autosens_data.ratio > 1) {
			// with a target of 100, default 0.7-1.2 autosens min/max range would allow a 93-117 target range

			//	NJIT - not strictly identical because source code did not add ', 0' to round b/c of shortcut
			min_bg = APS_round((min_bg - 60) / autosens_data.ratio, 0) + 60;
			max_bg = APS_round((max_bg - 60) / autosens_data.ratio, 0) + 60;
			double new_target_bg = APS_round((target_bg - 60) / autosens_data.ratio, 0) + 60;
			// don't allow target_bg below 80
			new_target_bg = max(80, new_target_bg);
			if (target_bg == new_target_bg) {
				printf("target_bg unchanged\n");
			}
			else {
				printf("target_bg changed\n");
			}
			target_bg = new_target_bg;
		}
	}

	// Raise target for noisy / raw CGM data.
	if (glucose_status.noise >= 2) {
		// increase target at least 10% (default 30%) for raw / noisy data
		double noisyCGMTargetMultiplier = max(1.1, profile.noisyCGMTargetMultiplier);
		// don't allow maxRaw above 250
		double maxRaw = min(250, profile.maxRaw);
		double adjustedMinBG = APS_round(min(200, min_bg * noisyCGMTargetMultiplier), 0);
		double adjustedTargetBG = APS_round(min(200, target_bg * noisyCGMTargetMultiplier), 0);
		double adjustedMaxBG = APS_round(min(200, max_bg * noisyCGMTargetMultiplier), 0);
		printf("Raising target_bg for noisy / raw CGM data");
		min_bg = adjustedMinBG;
		target_bg = adjustedTargetBG;
		max_bg = adjustedMaxBG;
	}

	// min_bg of 90 -> threshold of 65, 100 -> 70 110 -> 75, and 130 -> 85
	double threshold = min_bg - 0.5 * (min_bg - 40);

	// If iob_data or its required properties are missing, return.
	// This has to be checked after checking that we're not in one of the CGM-data-related error conditions handled above,
	// and before attempting to use iob_data below.

	// Adjust ISF based on sensitivityRatio

	double profile_sens = APS_round(profile.sens, 1);
	double sens = profile.sens;
	if (!isnan(autosens_data.ratio)) {
		sens = profile.sens / sensitivityRatio;
		sens = APS_round(sens, 1);
		if (sens != profile_sens) {
			printf("ISF changed\n");
		}
		else {
			printf("ISF unchanged\n");
		}
		//process.stderr.write(" (autosens ratio "+sensitivityRatio+")");
	}
	printf("CR: ");
	if (iob_data == NULL) {
		snprintf((char* const)(rT.error), (const size_t)sizeof(rT.error), (const char* const)"Error: iob_data undefined. ");
		return rT;
	}

	IOB_Data* iobArray = iob_data;

	if (iobLength < 1 || isnan(iob_data[0].activity) || isnan(iob_data[0].iob)) {
		snprintf((char* const)(rT.error), (const size_t)sizeof(rT.error), (const char* const)"Error: iob_data missing some property. ");
		return rT;
	}
	// Compare currenttemp to iob_data.lastTemp and cancel temp if they don't match, as a safety check
	// This should occur after checking that we're not in one of the CGM-data-related error conditions handled above,
	// and before returning (doing nothing) below if eventualBG is undefined.
	double lastTempAge;
	if (isnan(iob_data[0].lastTemp.duration) || isnan(iob_data[0].lastTemp.rate)) {
		lastTempAge = APS_round((systemTime - iob_data[0].lastTemp.date) / 60000, 0); // in minutes
	}
	else {
		lastTempAge = 0;
	}
	
	double tempModulus = fmod((lastTempAge + currenttemp.duration), 30);
	printf("currenttemp lastTempAge tempModulus\n");
	snprintf((char* const)(rT.temp), (const size_t)sizeof(rT.temp), (const char* const)"absolute");
	rT.deliverAt = deliverAt;
	/*	microBolus is neber allowed in this implementation
	if (microBolusAllowed&& currenttemp&& iob_data.lastTemp&& currenttemp.rate != = iob_data.lastTemp.rate && lastTempAge > 10 && currenttemp.duration) {
		rT.reason = "Warning: currenttemp rate " + currenttemp.rate + " != lastTemp rate " + iob_data.lastTemp.rate + " from pumphistory; canceling temp";
		return tempBasalFunctions.setTempBasal(0, 0, profile, rT, currenttemp);
	}
	*/
	if (!isnan(currenttemp.duration) && !isnan(currenttemp.rate) && 
		!isnan(iob_data[0].iob) && !isnan(iob_data[0].activity)
		&& currenttemp.duration > 0) {
		//console.error(lastTempAge, round(iob_data.lastTemp.duration,1), round(lastTempAge - iob_data.lastTemp.duration,1));
		double lastTempEnded = lastTempAge - iob_data[0].lastTemp.duration;
			if (lastTempEnded > 5 && lastTempAge > 10) {
				snprintf((char* const)(rT.reason), (const size_t)sizeof(rT.reason), 
					(const char* const)"Warning: currenttemp running but lastTemp from pumphistory ended %.6lfm ago; canceling temp",
					lastTempEnded);
				//console.error(currenttemp, round(iob_data.lastTemp,1), round(lastTempAge,1));
				return tempBasalFunctions.setTempBasal(0, 0, profile, rT, currenttemp);
			}
	}

	// Calculate BGI, deviation, and eventualBG.
	// This has to happen after we obtain iob_data

		//calculate BG impact: the amount BG "should" be rising or falling based on insulin activity alone
	double bgi = APS_round((-iob_data[0].activity * sens * 5), 2);
	// project deviations for 30 minutes
	double deviation = APS_round(30 / 5 * (minDelta - bgi), 0);
	// don't overreact to a big negative delta: use minAvgDelta if deviation is negative
	if (deviation < 0) {
		deviation = APS_round((30 / 5) * (minAvgDelta - bgi), 0);
		// and if deviation is still negative, use long_avgdelta
		if (deviation < 0) {
			deviation = APS_round((30 / 5) * (glucose_status.long_avgdelta - bgi), 0);
		}
	}

	double naive_eventualBG;
	// calculate the naive (bolus calculator math) eventual BG based on net IOB and sensitivity
	if (iob_data[0].iob > 0) {
		naive_eventualBG = APS_round(bg - (iob_data[0].iob * sens), 0);
	}
	else { // if IOB is negative, be more conservative and use the lower of sens, profile.sens
		naive_eventualBG = APS_round(bg - (iob_data[0].iob * min(sens, profile.sens)), 0);
	}
	// and adjust it for the deviation above
	double eventualBG = naive_eventualBG + deviation;

	if (isnan(eventualBG)) {
		snprintf((char* const)(rT.error), (const size_t)sizeof(rT.error), (const char* const)"Error: could not calculate eventualBG. ");
		return rT;
	}
	double expectedDelta = calculate_expected_delta(target_bg, eventualBG, bgi);

	//console.error(reservoir_data);

	// Initialize rT (requestedTemp) object. Has to be done after eventualBG is calculated.
	snprintf((char* const)(rT.temp), (const size_t)sizeof(rT.temp), (const char* const)"absolute");
	rT.bg = bg;
	rT.tick = tick;
	rT.eventualBG = eventualBG;
	rT.insulinReq = 0;
	//	No reservoir data
	rT.deliverAt = deliverAt;
	rT.sensitivityRatio = sensitivityRatio;

	// Generate predicted future BGs based on IOB, COB, and current absorption rate

	// Initialize and calculate variables used for predicting BGs

	List* COBpredBGs = List_Create();
	List* IOBpredBGs = List_Create();
//	List* UAMpredBGs = List_Create();	No UAM
//	List* ZTpredBGs = List_Create();	No ZT

	//	Append == Push
	double* fakeBG = malloc(sizeof(double));
  double* fakeBG_Copy = malloc(sizeof(double));	//	lists cannot keep a copy of the same pointer or else double free error
	*fakeBG = bg;
  *fakeBG_Copy = bg;
	List_Append(COBpredBGs, fakeBG);
	List_Append(IOBpredBGs, fakeBG_Copy);
//	List_Append(UAMpredBGs, fakeBG);
//	List_Append(ZTpredBGs, fakeBG);

	/*	No SMB (super micro bolus) */
	// carb impact and duration are 0 unless changed below
	double ci = 0;
	double cid = 0;
	// calculate current carb absorption rate, and how long to absorb all carbs
	// CI = current carb impact on BG in mg/dL/5m
	ci = APS_round((minDelta - bgi), 1);
	double uci = APS_round((minDelta - bgi), 1);
	// ISF (mg/dL/U) / CR (g/U) = CSF (mg/dL/g)

	// use autosens-adjusted sens to counteract autosens meal insulin dosing adjustments so that
	// autotuned CR is still in effect even when basals and ISF are being adjusted by TT or autosens
	// this avoids overdosing insulin for large meals when low temp targets are active
	double csf = sens / profile.carb_ratio;
	printf("profile.sens sens CSF\n");

	double maxCarbAbsorptionRate = 30; // g/h; maximum rate to assume carbs will absorb if no CI observed
	// limit Carb Impact to maxCarbAbsorptionRate * csf in mg/dL per 5m
	double maxCI = APS_round(maxCarbAbsorptionRate * csf * 5 / 60, 1);
	if (ci > maxCI) {
		printf("Limiting carb impact\n");
		ci = maxCI;
	}
	double remainingCATimeMin = 3; // h; minimum duration of expected not-yet-observed carb absorption
	// adjust remainingCATime (instead of CR) for autosens if sensitivityRatio defined
	if (sensitivityRatio) {
		remainingCATimeMin = remainingCATimeMin / sensitivityRatio;
	}

	// 20 g/h means that anything <= 60g will get a remainingCATimeMin, 80g will get 4h, and 120g 6h
	// when actual absorption ramps up it will take over from remainingCATime
	double assumedCarbAbsorptionRate = 20; // g/h; maximum rate to assume carbs will absorb if no CI observed
	double remainingCATime = remainingCATimeMin;
	if (meal_data.carbs) {
		// if carbs * assumedCarbAbsorptionRate > remainingCATimeMin, raise it
		// so <= 90g is assumed to take 3h, and 120g=4h
		remainingCATimeMin = max(remainingCATimeMin, meal_data.mealCOB / assumedCarbAbsorptionRate);
		double lastCarbAge = APS_round((systemTime - meal_data.lastCarbTime) / 60000, 0);
		//console.error(meal_data.lastCarbTime, lastCarbAge);

		double fractionCOBAbsorbed = (meal_data.carbs - meal_data.mealCOB) / meal_data.carbs;
		// if the lastCarbTime was 1h ago, increase remainingCATime by 1.5 hours
		remainingCATime = remainingCATimeMin + 1.5 * lastCarbAge / 60;
		remainingCATime = APS_round(remainingCATime, 1);
		//console.error(fractionCOBAbsorbed, remainingCATimeAdjustment, remainingCATime)
		printf("Last carbs");
	}
	// calculate the number of carbs absorbed over remainingCATime hours at current CI
		// CI (mg/dL/5m) * (5m)/5 (m) * 60 (min/hr) * 4 (h) / 2 (linear decay factor) = total carb impact (mg/dL)
	double totalCI = max(0, ci / 5 * 60 * remainingCATime / 2);
	// totalCI (mg/dL) / CSF (mg/dL/g) = total carbs absorbed (g)
	double totalCA = totalCI / csf;
	double remainingCarbsCap = 90; // default to 90
	double remainingCarbsFraction = 1;
	if (profile.remainingCarbsCap) { remainingCarbsCap = min(90, profile.remainingCarbsCap); }
	if (profile.remainingCarbsFraction) { remainingCarbsFraction = min(1, profile.remainingCarbsFraction); }
	double remainingCarbsIgnore = 1 - remainingCarbsFraction;
	double remainingCarbs = max(0, meal_data.mealCOB - totalCA - meal_data.carbs * remainingCarbsIgnore);
	remainingCarbs = min(remainingCarbsCap, remainingCarbs);
	// assume remainingCarbs will absorb in a /\ shaped bilinear curve
	// peaking at remainingCATime / 2 and ending at remainingCATime hours
	// area of the /\ triangle is the same as a remainingCIpeak-height rectangle out to remainingCATime/2
	// remainingCIpeak (mg/dL/5m) = remainingCarbs (g) * CSF (mg/dL/g) * 5 (m/5m) * 1h/60m / (remainingCATime/2) (h)
	double remainingCIpeak = remainingCarbs * csf * 5 / 60 / (remainingCATime / 2);
	//console.error(profile.min_5m_carbimpact,ci,totalCI,totalCA,remainingCarbs,remainingCI,remainingCATime);

	// calculate peak deviation in last hour, and slope from that to current deviation
	double slopeFromMaxDeviation = APS_round(meal_data.slopeFromMaxDeviation, 2);
	// calculate lowest deviation in last hour, and slope from that to current deviation
	double slopeFromMinDeviation = APS_round(meal_data.slopeFromMinDeviation, 2);
	// assume deviations will drop back down at least at 1/3 the rate they ramped up
	double slopeFromDeviations = min(slopeFromMaxDeviation, -slopeFromMinDeviation / 3);
	//console.error(slopeFromMaxDeviation);


	//5m data points = g * (1U/10g) * (40mg/dL/1U) / (mg/dL/5m)
	// duration (in 5m data points) = COB (g) * CSF (mg/dL/g) / ci (mg/dL/5m)
	// limit cid to remainingCATime hours: the reset goes to remainingCI
	if (ci == 0) {
		// avoid divide by zero
		cid = 0;
	}
	else {
		cid = min(remainingCATime * 60 / 5 / 2, max(0, meal_data.mealCOB * csf / ci));
	}
	// duration (hours) = duration (5m) * 5 / 60 * 2 (to account for linear decay)
	printf("Carb Impact mg/dL per 5m; CI Duration hours; remaining CI ( peak): mg/dL per 5m\n");

	double minIOBPredBG = 999;
	double minCOBPredBG = 999;
	double minUAMPredBG = 999;
	double minGuardBG = bg;
	double minCOBGuardBG = 999;
	double minUAMGuardBG = 999;
	double minIOBGuardBG = 999;
	double minZTGuardBG = 999;
	double minPredBG;
	double avgPredBG;
	double IOBpredBG = eventualBG;
	double maxIOBPredBG = bg;
	double maxCOBPredBG = bg;
	double maxUAMPredBG = bg;
	double eventualPredBG = bg;
	double lastIOBpredBG;
	double lastCOBpredBG;
	double lastUAMpredBG;
	double lastZTpredBG;
	double UAMduration = 0;
	double remainingCItotal = 0;
	List* remainingCIs = List_Create();
	List* predCIs = List_Create();

	/*	No such thing as try/catch in C	*/
	double COBpredBG;
	/*	NJIT - Ok so they treat the iob_data as an array, but if its a single element they treat it as a single element and not an array */
	for (int i = 0; i < iobLength && iobLength != 1; i++)
	{
		IOB_Data iobTick = iob_data[i];
		double predBGI = APS_round((-iobTick.activity * sens * 5), 2);
		// for IOBpredBGs, predicted deviation impact drops linearly from current deviation down to zero
		// over 60 minutes (data points every 5m)
		double predDev = ci * (1 - min(1, IOBpredBGs->length / (60 / 5)));
		IOBpredBG = *((double*)(IOBpredBGs->elements[IOBpredBGs->length - 1])) + predBGI + predDev;
		// calculate predBGs with long zero temp without deviations
		// for COBpredBGs, predicted carb impact drops linearly from current carb impact down to zero
			// eventually accounting for all carbs (if they can be absorbed over DIA)
		double predCI = max(0, max(0, ci) * (1 - COBpredBGs->length / max(cid * 2, 1)));
		// if any carbs aren't absorbed after remainingCATime hours, assume they'll absorb in a /\ shaped
		// bilinear curve peaking at remainingCIpeak at remainingCATime/2 hours (remainingCATime/2*12 * 5m)
		// and ending at remainingCATime h (remainingCATime*12 * 5m intervals)
		double intervals = min(COBpredBGs->length, (remainingCATime * 12) - COBpredBGs->length);
		double remainingCI = max(0, intervals / (remainingCATime / 2 * 12) * remainingCIpeak);
		remainingCItotal += predCI + remainingCI;

		double* num = malloc(sizeof(double));
		*num = APS_round(remainingCI, 0);
		List_Append(remainingCIs, num);

		num = malloc(sizeof(double));
		*num = APS_round(predCI, 0);
		List_Append(remainingCIs, num);

		COBpredBG = *((double*)COBpredBGs->elements[COBpredBGs->length - 1]) + predBGI + min(0, predDev) + predCI + remainingCI;

		// truncate all BG predictions at 4 hours
		if (IOBpredBGs->length < 48) { double* n = malloc(sizeof(double)); *n = IOBpredBG; List_Append(IOBpredBGs, n); }
		if (COBpredBGs->length < 48) { double* n = malloc(sizeof(double)); *n = COBpredBG; List_Append(COBpredBGs, n); }
		// calculate minGuardBGs without a wait from COB, UAM, IOB predBGs
		if (COBpredBG < minCOBGuardBG) { minCOBGuardBG = APS_round(COBpredBG, 0); }
		if (IOBpredBG < minIOBGuardBG) { minIOBGuardBG = APS_round(IOBpredBG, 0); }

		// set minPredBGs starting when currently-dosed insulin activity will peak
			// look ahead 60m (regardless of insulin type) so as to be less aggressive on slower insulins
		double insulinPeakTime = 60;
		// add 30m to allow for insulin delivery (SMBs or temps)
		insulinPeakTime = 90;
		double insulinPeak5m = (insulinPeakTime / 60) * 12;
		//console.error(insulinPeakTime, insulinPeak5m, profile.insulinPeakTime, profile.curve);

		// wait 90m before setting minIOBPredBG
		if (IOBpredBGs->length > insulinPeak5m && (IOBpredBG < minIOBPredBG)) { minIOBPredBG = APS_round(IOBpredBG, 0); }
		if (IOBpredBG > maxIOBPredBG) { maxIOBPredBG = IOBpredBG; }
		// wait 85-105m before setting COB and 60m for UAM minPredBGs
		if ((cid || remainingCIpeak > 0) && COBpredBGs->length > insulinPeak5m && (COBpredBG < minCOBPredBG)) { minCOBPredBG = APS_round(COBpredBG, 0); }
		if ((cid || remainingCIpeak > 0) && COBpredBG > maxIOBPredBG) { maxCOBPredBG = COBpredBG; }
	}
	for (int i = 0; i < IOBpredBGs->length; i++) {
		*((double*)IOBpredBGs->elements[i]) = APS_round(min(401, max(39, *((double*)IOBpredBGs->elements[i]))),0);
	}
	for (long i = IOBpredBGs->length - 1; i > 12; i--) {
		if (IOBpredBGs->elements[i - 1] != IOBpredBGs->elements[i]) break; 
		else free(List_Pop(IOBpredBGs)); 	
	}
	rT.predBGs.IOB = IOBpredBGs;
	lastIOBpredBG = APS_round(*((double*)((List*)IOBpredBGs)->elements[((List*)IOBpredBGs)->length - 1]), 0);
	if (meal_data.mealCOB > 0 && (ci > 0 || remainingCIpeak > 0)) {
		for (int i = 0; i < COBpredBGs->length; i++) {
			*((double*)COBpredBGs->elements[i]) = APS_round(min(401, max(39, *((double*)COBpredBGs->elements[i]))), 0);
		}
		for (long i = COBpredBGs->length - 1; i > 12; i--) {
			if (COBpredBGs->elements[i - 1] != COBpredBGs->elements[i]) break;
			else free(List_Pop(COBpredBGs));
		}
		rT.predBGs.COB = COBpredBGs;
		lastCOBpredBG = APS_round(*((double*)COBpredBGs->elements[COBpredBGs->length - 1]), 0);
		eventualBG = max(eventualBG, APS_round(*((double*)COBpredBGs->elements[COBpredBGs->length - 1]),0));
	}
	else {	//	NJIT - need to free COB if not using it
		rT.predBGs.COB = NULL;
	}

	if (ci > 0 || remainingCIpeak > 0) {
		// set eventualBG based on COB or UAM predBGs
		rT.eventualBG = eventualBG;
	}

	minIOBPredBG = max(39, minIOBPredBG);
	minCOBPredBG = max(39, minCOBPredBG);
	minPredBG = APS_round(minIOBPredBG, 0);

	double fractionCarbsLeft = meal_data.mealCOB / meal_data.carbs;
	if (minCOBPredBG < 999) {
		avgPredBG = APS_round((IOBpredBG + COBpredBG) / 2, 0);
	}
	else {
		avgPredBG = APS_round(IOBpredBG, 0);
	}
	// if avgPredBG is below minZTGuardBG, bring it up to that level
	if (minZTGuardBG > avgPredBG) {
		avgPredBG = minZTGuardBG;
	}
	// if avgPredBG is below minZTGuardBG, bring it up to that level
	if (minZTGuardBG > avgPredBG) {
		avgPredBG = minZTGuardBG;
	}

	// if we have both minCOBGuardBG and minUAMGuardBG, blend according to fractionCarbsLeft
	if ((cid || remainingCIpeak > 0)) {
		minGuardBG = minCOBGuardBG;
	}
	else {
		minGuardBG = minIOBGuardBG;
	}
	minGuardBG = APS_round(minGuardBG, 0);
	//console.error(minCOBGuardBG, minUAMGuardBG, minIOBGuardBG, minGuardBG);

	// if any carbs have been entered recently
	if (meal_data.carbs) {

		// if UAM is disabled, use max of minIOBPredBG, minCOBPredBG
		if (minCOBPredBG < 999) {
			minPredBG = APS_round(max(minIOBPredBG, minCOBPredBG), 0);
			// if we have COB, use minCOBPredBG, or blendedMinPredBG if it's higher
		}
		else if (minCOBPredBG < 999) {
			// calculate blendedMinPredBG based on how many carbs remain as COB
			// NJIT - No ZT predict so use .5 as minimum
			double blendedMinPredBG = fractionCarbsLeft * minCOBPredBG + (1 - fractionCarbsLeft) * .5f;
			// if blendedMinPredBG > minCOBPredBG, use that instead
			minPredBG = APS_round(max(minIOBPredBG, max(minCOBPredBG, blendedMinPredBG)), 0);
			// if carbs have been entered, but have expired, use minUAMPredBG
		}
		else {
			minPredBG = minGuardBG;
		}
	}

	// make sure minPredBG isn't higher than avgPredBG
	minPredBG = min(minPredBG, avgPredBG);

	rT.COB = meal_data.mealCOB;
	rT.IOB = iob_data[0].iob;
	rT.BGI = convert_bg(bgi, profile);
	rT.deviation = convert_bg(deviation, profile);
	rT.ISF = convert_bg(sens, profile);
	rT.CR = APS_round(profile.carb_ratio, 2);
	rT.target_bg = convert_bg(target_bg, profile);

  char buffer[20];
  dtostrf(rT.BGI, 8, 2, buffer);

	snprintf((char* const)(rT.reason), (const size_t)sizeof(rT.reason),
		(const char* const)"COB: %.6lf, Dev: %.6lf, BGI: %s, ISF: %.6lf, CR: %.6lf, minPredBG %.6lf, minGuardBG %.6lf, IOpredBG %.6lf"
		, rT.COB, rT.deviation, buffer, rT.ISF, rT.CR, convert_bg(minPredBG, profile), 
		convert_bg(minGuardBG, profile), convert_bg(lastIOBpredBG, profile));
	printf("\n\n:(\n");
	if (lastCOBpredBG > 0) {
		snprintf((char* const)(rT.reason), (const size_t)sizeof(rT.reason),
			(const char* const)"%s, COBpredBG %.6lf"
			,rT.reason, convert_bg(lastCOBpredBG, profile));
	}
	snprintf((char* const)(rT.reason), (const size_t)sizeof(rT.reason), (const char* const)"%s; ", rT.reason);

	// Use minGuardBG to prevent overdosing in hypo-risk situations
	// use naive_eventualBG if above 40, but switch to minGuardBG if both eventualBGs hit floor of 39
	double carbsReqBG = naive_eventualBG;
	if (carbsReqBG < 40) {
		carbsReqBG = min(minGuardBG, carbsReqBG);
	}
	double bgUndershoot = threshold - carbsReqBG;
	// calculate how long until COB (or IOB) predBGs drop below min_bg
	double minutesAboveMinBG = 240;
	double minutesAboveThreshold = 240;
	if (meal_data.mealCOB > 0 && (ci > 0 || remainingCIpeak > 0)) {
		for (int i = 0; i < COBpredBGs->length; i++) {
			//console.error(COBpredBGs[i], min_bg);
			if (*((double*)(COBpredBGs->elements[i])) < min_bg) {
				minutesAboveMinBG = 5 * i;
				break;
			}
		}
		for (int i = 0; i < COBpredBGs->length; i++) {
			//console.error(COBpredBGs[i], threshold);
			if (*((double*)(COBpredBGs->elements[i])) < threshold) {
				minutesAboveThreshold = 5 * i;
				break;
			}
		}
	}
	else {
		for (int i = 0; i < IOBpredBGs->length; i++) {
			//console.error(IOBpredBGs[i], min_bg);
			if (*((double*)(IOBpredBGs->elements[i])) < min_bg) {
				minutesAboveMinBG = 5 * i;
				break;
			}
		}
		for (int i = 0; i < IOBpredBGs->length; i++) {
			//console.error(IOBpredBGs[i], threshold);
			if (*((double*)(IOBpredBGs->elements[i])) < threshold) {
				minutesAboveThreshold = 5 * i;
				break;
			}
		}
	}
	// Calculate carbsReq (carbs required to avoid a hypo)
	printf("BG projected to remain above %.6lf ", convert_bg(min_bg, profile));
	printf("for %.6lf minutes\n", minutesAboveMinBG);
	if (minutesAboveThreshold < 240 || minutesAboveMinBG < 60) {
		printf("BG projected to remain above %.6lf for ", convert_bg(threshold, profile));
		printf("%.6lf minutes\n",minutesAboveThreshold);
	}
	// include at least minutesAboveThreshold worth of zero temps in calculating carbsReq
	// always include at least 30m worth of zero temp (carbs to 80, low temp up to target)
	double zeroTempDuration = minutesAboveThreshold;
	// BG undershoot, minus effect of zero temps until hitting min_bg, converted to grams, minus COB
	double zeroTempEffect = profile.current_basal * sens * zeroTempDuration / 60;
	// don't count the last 25% of COB against carbsReq
	double COBforCarbsReq = max(0, meal_data.mealCOB - 0.25 * meal_data.carbs);
	double carbsReq = (bgUndershoot - zeroTempEffect) / csf - COBforCarbsReq;
	printf("naive_eventualBG: %.6lf bgUndershoot: %.6lf zeroTempDuration: %.6lf zeroTempEffect: %.6lf carbsReq: %.6lf\n",
		naive_eventualBG, bgUndershoot, zeroTempDuration, zeroTempEffect, carbsReq);
	printf("Check carbsReq %.6lf\n", carbsReq);
	zeroTempEffect = APS_round(zeroTempEffect, 0);
	carbsReq = APS_round(carbsReq, 0);
	printf("naive_eventualBG: %.6lf bgUndershoot: %.6lf zeroTempDuration: %.6lf zeroTempEffect: %.6lf carbsReq: %.6lf\n", 
		naive_eventualBG, bgUndershoot, zeroTempDuration, zeroTempEffect, carbsReq);
	if (strcmp(meal_data.reason,"Could not parse clock data") == 0) {
		printf("carbsReq unknown: Could not parse clock data");
	}
	else if (carbsReq >= profile.carbsReqThreshold && minutesAboveThreshold <= 45) {
		rT.carbsReq = carbsReq;
		snprintf((char* const)(rT.reason), (const size_t)sizeof(rT.reason), (const char* const)"%s add'l carbs req w/in %.6lfm; ", 
			rT.reason, minutesAboveThreshold);
	}

	// Begin core dosing logic: check for situations requiring low or high temps, and return appropriate temp after first match

	double worstCaseInsulinReq;
	double durationReq;

	// don't low glucose suspend if IOB is already super negative and BG is rising faster than predicted
	if (bg < threshold && iob_data[0].iob < -profile.current_basal * 20 / 60 && minDelta > 0 && minDelta > expectedDelta) {
		snprintf((char* const)(rT.reason), (const size_t)sizeof(rT.reason), (const char* const)"%sIOB %.6lf < %.6lf",
			rT.reason, iob_data[0].iob, APS_round(-profile.current_basal * 20 / 60, 2));
		snprintf((char* const)(rT.reason), (const size_t)sizeof(rT.reason), (const char* const)"%s and minDelta %.6lf > expectedDelta %.6lf; ",
			rT.reason, convert_bg(minDelta, profile), convert_bg(expectedDelta, profile));
		// predictive low glucose suspend mode: BG is / is projected to be < threshold
	}
	else if (bg < threshold || minGuardBG < threshold) {
		snprintf((char* const)(rT.reason), (const size_t)sizeof(rT.reason), (const char* const)"%sminGuardBG %.6lf<%.6lf",
			rT.reason, convert_bg(minGuardBG, profile), convert_bg(threshold, profile));
		bgUndershoot = target_bg - minGuardBG;
		worstCaseInsulinReq = bgUndershoot / sens;
		durationReq = round(60 * worstCaseInsulinReq / profile.current_basal);
		durationReq = round(durationReq / 30) * 30;
		// always set a 30-120m zero temp (oref0-pump-loop will let any longer SMB zero temp run)
		durationReq = min(120, max(30, durationReq));

		//	NJIT - Prevent Memory Leak
		if (rT.predBGs.COB != NULL)
			for (int i = 0; i < COBpredBGs->length; i++) free(COBpredBGs->elements[i]); List_Destroy(COBpredBGs);
		for (int i = 0; i < remainingCIs->length; i++) free(remainingCIs->elements[i]); List_Destroy(remainingCIs);
		for (int i = 0; i < predCIs->length; i++) free(predCIs->elements[i]); List_Destroy(predCIs);
		return tempBasalFunctions.setTempBasal(0, durationReq, profile, rT, currenttemp);
	}
	double insulinReq;
	double rate;
	double insulinScheduled;
	if (eventualBG < min_bg) { // if eventual BG is below target:
		snprintf((char* const)(rT.reason), (const size_t)sizeof(rT.reason), (const char* const)"%sEventual BG %.6lf < %.6lf",
			rT.reason, convert_bg(eventualBG, profile), convert_bg(min_bg, profile));
		// if 5m or 30m avg BG is rising faster than expected delta
		if (minDelta > expectedDelta && minDelta > 0 && !carbsReq) {
			// if naive_eventualBG < 40, set a 30m zero temp (oref0-pump-loop will let any longer SMB zero temp run)
			if (naive_eventualBG < 40) {
				snprintf((char* const)(rT.reason), (const size_t)sizeof(rT.reason), (const char* const)"%s, naive_eventualBG < 40.",
					rT.reason);
				//	NJIT - Prevent Memory Leak
				if (rT.predBGs.COB != NULL)
					for (int i = 0; i < COBpredBGs->length; i++) free(COBpredBGs->elements[i]); List_Destroy(COBpredBGs);
				for (int i = 0; i < remainingCIs->length; i++) free(remainingCIs->elements[i]); List_Destroy(remainingCIs);
				for (int i = 0; i < predCIs->length; i++) free(predCIs->elements[i]); List_Destroy(predCIs);
				return tempBasalFunctions.setTempBasal(0, 30, profile, rT, currenttemp);
			}
			if (glucose_status.delta > minDelta) {
				snprintf((char* const)(rT.reason), (const size_t)sizeof(rT.reason), (const char* const)"%s, but Delta %.6lf > expectedDelta %.6lf",
					rT.reason, convert_bg(tick, profile), convert_bg(expectedDelta, profile));
			}
			else {
				snprintf((char* const)(rT.reason), (const size_t)sizeof(rT.reason), 
					(const char* const)"%s, but Min. Delta %.2lf > Exp. Delta %.6lf",
					rT.reason, minDelta, convert_bg(expectedDelta, profile));
			}
			if (currenttemp.duration > 15 && (round_basal(basal, profile) == round_basal(currenttemp.rate, profile))) {
				snprintf((char* const)(rT.reason), (const size_t)sizeof(rT.reason),
					(const char* const)"%s, temp %.6lf ~ req %.6lfU/hr. ",
					rT.reason, currenttemp.rate, basal);
				//	NJIT - Prevent Memory Leak
				if (rT.predBGs.COB != NULL)
					for (int i = 0; i < COBpredBGs->length; i++) free(COBpredBGs->elements[i]); List_Destroy(COBpredBGs);
				for (int i = 0; i < remainingCIs->length; i++) free(remainingCIs->elements[i]); List_Destroy(remainingCIs);
				for (int i = 0; i < predCIs->length; i++) free(predCIs->elements[i]); List_Destroy(predCIs);
				return rT;
			}
			else {
				snprintf((char* const)(rT.reason), (const size_t)sizeof(rT.reason),
					(const char* const)"%s; setting current basal of %.6lf as temp",
					rT.reason, basal);
				//	NJIT - Prevent Memory Leak
				if (rT.predBGs.COB != NULL)
					for (int i = 0; i < COBpredBGs->length; i++) free(COBpredBGs->elements[i]); List_Destroy(COBpredBGs);
				for (int i = 0; i < remainingCIs->length; i++) free(remainingCIs->elements[i]); List_Destroy(remainingCIs);
				for (int i = 0; i < predCIs->length; i++) free(predCIs->elements[i]); List_Destroy(predCIs);
				return tempBasalFunctions.setTempBasal(basal, 30, profile, rT, currenttemp);
			}
		}

		// calculate 30m low-temp required to get projected BG up to target
		// multiply by 2 to low-temp faster for increased hypo safety
		insulinReq = 2 * min(0, (eventualBG - target_bg) / sens);
		insulinReq = APS_round(insulinReq, 2);
		// calculate naiveInsulinReq based on naive_eventualBG
		double naiveInsulinReq = min(0, (naive_eventualBG - target_bg) / sens);
		naiveInsulinReq = APS_round(naiveInsulinReq, 2);
		if (minDelta < 0 && minDelta > expectedDelta) {
			// if we're barely falling, newinsulinReq should be barely negative
			double newinsulinReq = APS_round((insulinReq * (minDelta / expectedDelta)), 2);
			//console.error("Increasing insulinReq from " + insulinReq + " to " + newinsulinReq);
			insulinReq = newinsulinReq;
		}
		// rate required to deliver insulinReq less insulin over 30m:
		rate = basal + (2 * insulinReq);
		rate = round_basal(rate, profile);

		// if required temp < existing temp basal
		insulinScheduled = currenttemp.duration * (currenttemp.rate - basal) / 60;
		// if current temp would deliver a lot (30% of basal) less than the required insulin,
		// by both normal and naive calculations, then raise the rate
		double minInsulinReq = min(insulinReq, naiveInsulinReq);
		if (insulinScheduled < minInsulinReq - basal * 0.3) {
			snprintf((char* const)(rT.reason), (const size_t)sizeof(rT.reason),
				(const char* const)"%s, %.6lfm@ %.2lf is a lot less than needed. ",
				rT.reason, currenttemp.duration, currenttemp.rate);
			//	NJIT - Prevent Memory Leak
			if (rT.predBGs.COB != NULL)
				for (int i = 0; i < COBpredBGs->length; i++) free(COBpredBGs->elements[i]); List_Destroy(COBpredBGs);
			for (int i = 0; i < remainingCIs->length; i++) free(remainingCIs->elements[i]); List_Destroy(remainingCIs);
			for (int i = 0; i < predCIs->length; i++) free(predCIs->elements[i]); List_Destroy(predCIs);
			return tempBasalFunctions.setTempBasal(rate, 30, profile, rT, currenttemp);
		}
		if (!isnan(currenttemp.rate) && (currenttemp.duration > 5 && rate >= currenttemp.rate * 0.8)) {
			snprintf((char* const)(rT.reason), (const size_t)sizeof(rT.reason),
				(const char* const)"%s, temp %.6lf ~< req %.6lfU/hr. ",
				rT.reason, currenttemp.rate, rate);
			//	NJIT - Prevent Memory Leak
			if (rT.predBGs.COB != NULL)
				for (int i = 0; i < COBpredBGs->length; i++) free(COBpredBGs->elements[i]); List_Destroy(COBpredBGs);
			for (int i = 0; i < remainingCIs->length; i++) free(remainingCIs->elements[i]); List_Destroy(remainingCIs);
			for (int i = 0; i < predCIs->length; i++) free(predCIs->elements[i]); List_Destroy(predCIs);
			return rT;
		}
		else {
			// calculate a long enough zero temp to eventually correct back up to target
			if (rate <= 0) {
				bgUndershoot = target_bg - naive_eventualBG;
				worstCaseInsulinReq = bgUndershoot / sens;
				durationReq = APS_round(60 * worstCaseInsulinReq / profile.current_basal, 0);
				if (durationReq < 0) {
					durationReq = 0;
					// don't set a temp longer than 120 minutes
				}
				else {
					durationReq = round(durationReq / 30) * 30;
					durationReq = min(120, max(0, durationReq));
				}
				//console.error(durationReq);
				if (durationReq > 0) {
					snprintf((char* const)(rT.reason), (const size_t)sizeof(rT.reason),
						(const char* const)"%s, setting %.6lfm zero temp. ",
						rT.reason, durationReq);
					//	NJIT - Prevent Memory Leak
					if (rT.predBGs.COB != NULL)
						for (int i = 0; i < COBpredBGs->length; i++) free(COBpredBGs->elements[i]); List_Destroy(COBpredBGs);
					for (int i = 0; i < remainingCIs->length; i++) free(remainingCIs->elements[i]); List_Destroy(remainingCIs);
					for (int i = 0; i < predCIs->length; i++) free(predCIs->elements[i]); List_Destroy(predCIs);
					return tempBasalFunctions.setTempBasal(rate, durationReq, profile, rT, currenttemp);
				}
			}
			else {
				snprintf((char* const)(rT.reason), (const size_t)sizeof(rT.reason),
					(const char* const)"%s, setting %.6lfU/hr. ",
					rT.reason, rate);
			}
			//	NJIT - Prevent Memory Leak
			if (rT.predBGs.COB != NULL)
				for (int i = 0; i < COBpredBGs->length; i++) free(COBpredBGs->elements[i]); List_Destroy(COBpredBGs);
			for (int i = 0; i < remainingCIs->length; i++) free(remainingCIs->elements[i]); List_Destroy(remainingCIs);
			for (int i = 0; i < predCIs->length; i++) free(predCIs->elements[i]); List_Destroy(predCIs);
			return tempBasalFunctions.setTempBasal(rate, 30, profile, rT, currenttemp);
		}
	}
	// if eventual BG is above min but BG is falling faster than expected Delta
	if (minDelta < expectedDelta) {
		// if in SMB mode, don't cancel SMB zero temp
		if (glucose_status.delta < minDelta) {
			snprintf((char* const)(rT.reason), (const size_t)sizeof(rT.reason),
				(const char* const)"%s, Eventual BG %.6lf > %.6lf but Delta %.6lf < Exp. Delta %.6lf",
				rT.reason, convert_bg(eventualBG, profile), convert_bg(min_bg, profile), 
				convert_bg(tick, profile), convert_bg(expectedDelta, profile));
		}
		else {
			snprintf((char* const)(rT.reason), (const size_t)sizeof(rT.reason),
				(const char* const)"%s, Eventual BG %.6lf > %.6lf but Min. Delta %.2lf < Exp. Delta %.6lf",
				rT.reason, convert_bg(eventualBG, profile), convert_bg(min_bg, profile),
				minDelta, convert_bg(expectedDelta, profile));
		}
		if (currenttemp.duration > 15 && (round_basal(basal, profile) == round_basal(currenttemp.rate, profile))) {
			snprintf((char* const)(rT.reason), (const size_t)sizeof(rT.reason),
				(const char* const)"%s, temp %.6lf ~ req %.6lf U/hr. ",
				rT.reason, currenttemp.rate, basal);
			//	NJIT - Prevent Memory Leak
			if (rT.predBGs.COB != NULL)
				for (int i = 0; i < COBpredBGs->length; i++) free(COBpredBGs->elements[i]); List_Destroy(COBpredBGs);
			for (int i = 0; i < remainingCIs->length; i++) free(remainingCIs->elements[i]); List_Destroy(remainingCIs);
			for (int i = 0; i < predCIs->length; i++) free(predCIs->elements[i]); List_Destroy(predCIs);
			return rT;
		}
		else {
			snprintf((char* const)(rT.reason), (const size_t)sizeof(rT.reason),
				(const char* const)"%s; setting current basal of %.6lf as temp. ",
				rT.reason, basal);
			//	NJIT - Prevent Memory Leak
			if (rT.predBGs.COB != NULL)
				for (int i = 0; i < COBpredBGs->length; i++) free(COBpredBGs->elements[i]); List_Destroy(COBpredBGs);
			for (int i = 0; i < remainingCIs->length; i++) free(remainingCIs->elements[i]); List_Destroy(remainingCIs);
			for (int i = 0; i < predCIs->length; i++) free(predCIs->elements[i]); List_Destroy(predCIs);
			return tempBasalFunctions.setTempBasal(basal, 30, profile, rT, currenttemp);
		}
	}
	// eventualBG or minPredBG is below max_bg
	if (min(eventualBG, minPredBG) < max_bg) {
		snprintf((char* const)(rT.reason), (const size_t)sizeof(rT.reason),
			(const char* const)"%s%.6lf-%.6lf in range: no temp required",
			rT.reason, convert_bg(eventualBG, profile), convert_bg(minPredBG, profile));
		if (currenttemp.duration > 15 && (round_basal(basal, profile) == round_basal(currenttemp.rate, profile))) {
			snprintf((char* const)(rT.reason), (const size_t)sizeof(rT.reason),
				(const char* const)"%s, temp %.6lf ~ req %.6lfU/hr",
				rT.reason, currenttemp.rate, basal);
			//	NJIT - Prevent Memory Leak
			if (rT.predBGs.COB != NULL)
				for (int i = 0; i < COBpredBGs->length; i++) free(COBpredBGs->elements[i]); List_Destroy(COBpredBGs);
			for (int i = 0; i < remainingCIs->length; i++) free(remainingCIs->elements[i]); List_Destroy(remainingCIs);
			for (int i = 0; i < predCIs->length; i++) free(predCIs->elements[i]); List_Destroy(predCIs);
			return rT;
		}
		else {
			snprintf((char* const)(rT.reason), (const size_t)sizeof(rT.reason),
				(const char* const)"%s; setting current basal of %.6lf as temp. ",
				rT.reason, basal);

			//	NJIT - Prevent Memory Leak
			if (rT.predBGs.COB != NULL)
				for (int i = 0; i < COBpredBGs->length; i++) free(COBpredBGs->elements[i]); List_Destroy(COBpredBGs);
			for (int i = 0; i < remainingCIs->length; i++) free(remainingCIs->elements[i]); List_Destroy(remainingCIs);
			for (int i = 0; i < predCIs->length; i++) free(predCIs->elements[i]); List_Destroy(predCIs);
			return tempBasalFunctions.setTempBasal(basal, 30, profile, rT, currenttemp);
		}
	}

	// eventual BG is at/above target
	// if iob is over max, just cancel any temps
	if (eventualBG >= max_bg) {
		snprintf((char* const)(rT.reason), (const size_t)sizeof(rT.reason),
			(const char* const)"%sEventual BG %.6lf >= %.6lf, ",
			rT.reason, convert_bg(eventualBG, profile), convert_bg(max_bg, profile));
	}

	if (iob_data[0].iob > max_iob) {
		snprintf((char* const)(rT.reason), (const size_t)sizeof(rT.reason),
			(const char* const)"%sIOB %.6lf > max_iob %.6lf",
			rT.reason, APS_round(iob_data[0].iob, 2), max_iob);
		if (currenttemp.duration > 15 && (round_basal(basal, profile) == round_basal(currenttemp.rate, profile))) {
			snprintf((char* const)(rT.reason), (const size_t)sizeof(rT.reason),
				(const char* const)"%s, temp %.6lf ~ req %.6lfU.hr",
				rT.reason, currenttemp.rate, basal);
			//	NJIT - Prevent Memory Leak
			if (rT.predBGs.COB != NULL)
				for (int i = 0; i < COBpredBGs->length; i++) free(COBpredBGs->elements[i]); List_Destroy(COBpredBGs);
			for (int i = 0; i < remainingCIs->length; i++) free(remainingCIs->elements[i]); List_Destroy(remainingCIs);
			for (int i = 0; i < predCIs->length; i++) free(predCIs->elements[i]); List_Destroy(predCIs);
			return rT;
		}
		else {
			snprintf((char* const)(rT.reason), (const size_t)sizeof(rT.reason),
				(const char* const)"%s; setting current basal of %.6lf as temp. ",
				rT.reason, basal);
			//	NJIT - Prevent Memory Leak
			if (rT.predBGs.COB != NULL)
				for (int i = 0; i < COBpredBGs->length; i++) free(COBpredBGs->elements[i]); List_Destroy(COBpredBGs);
			for (int i = 0; i < remainingCIs->length; i++) free(remainingCIs->elements[i]); List_Destroy(remainingCIs);
			for (int i = 0; i < predCIs->length; i++) free(predCIs->elements[i]); List_Destroy(predCIs);
			return tempBasalFunctions.setTempBasal(basal, 30, profile, rT, currenttemp);
		}
	}
	else { // otherwise, calculate 30m high-temp required to get projected BG down to target

		// insulinReq is the additional insulin required to get minPredBG down to target_bg
		//console.error(minPredBG,eventualBG);
		insulinReq = APS_round((min(minPredBG, eventualBG) - target_bg) / sens, 2);
		// if that would put us over max_iob, then reduce accordingly
		if (insulinReq > max_iob - iob_data[0].iob) {
			snprintf((char* const)(rT.reason), (const size_t)sizeof(rT.reason),
				(const char* const)"%smax_iob %.6lf, ",
				rT.reason, max_iob);
			insulinReq = max_iob - iob_data[0].iob;
		}

		// rate required to deliver insulinReq more insulin over 30m:
		rate = basal + (2 * insulinReq);
		rate = round_basal(rate, profile);
		insulinReq = APS_round(insulinReq, 3);
		rT.insulinReq = insulinReq;
		//console.error(iob_data.lastBolusTime);
		// minutes since last bolus
		double lastBolusAge = APS_round((systemTime - iob_data[0].lastBolusTime) / 60000, 1);
		//console.error(lastBolusAge);
		//console.error(profile.temptargetSet, target_bg, rT.COB);
		// only allow microboluses with COB or low temp targets, or within DIA hours of a bolus

		/*	NJIT - no microbolusing */

		double maxSafeBasal = tempBasalFunctions.getMaxSafeBasal(profile);
		printf("Max safe basal %.6lf\n", maxSafeBasal);
		if (rate > maxSafeBasal) {
			snprintf((char* const)(rT.reason), (const size_t)sizeof(rT.reason),
				(const char* const)"%sadj. req. rate: %.6lf to maxSafeBasal: %.6lf, ",
				rT.reason, rate, maxSafeBasal);
			rate = round_basal(maxSafeBasal, profile);
		}

		insulinScheduled = currenttemp.duration * (currenttemp.rate - basal) / 60;
		if (insulinScheduled >= insulinReq * 2) { // if current temp would deliver >2x more than the required insulin, lower the rate
			snprintf((char* const)(rT.reason), (const size_t)sizeof(rT.reason),
				(const char* const)"%s%.6lfm@%.2lf > 2 * insulinReq. Setting temp basal of %.6lfU/hr. ",
				rT.reason, currenttemp.duration, currenttemp.rate, rate);
			//	NJIT - Prevent Memory Leak
			if (rT.predBGs.COB != NULL)
				for (int i = 0; i < COBpredBGs->length; i++) free(COBpredBGs->elements[i]); List_Destroy(COBpredBGs);
			for (int i = 0; i < remainingCIs->length; i++) free(remainingCIs->elements[i]); List_Destroy(remainingCIs);
			for (int i = 0; i < predCIs->length; i++) free(predCIs->elements[i]); List_Destroy(predCIs);
			return tempBasalFunctions.setTempBasal(rate, 30, profile, rT, currenttemp);
		}

		if (isnan(currenttemp.duration) || currenttemp.duration == 0) { // no temp is set
			snprintf((char* const)(rT.reason), (const size_t)sizeof(rT.reason),
				(const char* const)"%sno temp, setting %.6lfU.hr. ",
				rT.reason, rate);
			//	NJIT - Prevent Memory Leak
			if (rT.predBGs.COB != NULL)
				for (int i = 0; i < COBpredBGs->length; i++) free(COBpredBGs->elements[i]); List_Destroy(COBpredBGs);
			for (int i = 0; i < remainingCIs->length; i++) free(remainingCIs->elements[i]); List_Destroy(remainingCIs);
			for (int i = 0; i < predCIs->length; i++) free(predCIs->elements[i]); List_Destroy(predCIs);
			return tempBasalFunctions.setTempBasal(rate, 30, profile, rT, currenttemp);
		}

		if (currenttemp.duration > 5 && (round_basal(rate, profile) <= round_basal(currenttemp.rate, profile))) { // if required temp <~ existing temp basal
			snprintf((char* const)(rT.reason), (const size_t)sizeof(rT.reason),
				(const char* const)"%stemp %.6lf >~ req %.6lfU/hr. ",
				rT.reason, currenttemp.rate, rate);
			//	NJIT - Prevent Memory Leak
			if (rT.predBGs.COB != NULL)
				for (int i = 0; i < COBpredBGs->length; i++) free(COBpredBGs->elements[i]); List_Destroy(COBpredBGs);
			for (int i = 0; i < remainingCIs->length; i++) free(remainingCIs->elements[i]); List_Destroy(remainingCIs);
			for (int i = 0; i < predCIs->length; i++) free(predCIs->elements[i]); List_Destroy(predCIs);
			return rT;
		}

		// required temp > existing temp basal
		snprintf((char* const)(rT.reason), (const size_t)sizeof(rT.reason),
			(const char* const)"%stemp %.6lf<%.6lfU/hr. ",
			rT.reason, currenttemp.rate, rate);
		//	NJIT - Prevent Memory Leak
		if (rT.predBGs.COB != NULL)
			for (int i = 0; i < COBpredBGs->length; i++) free(COBpredBGs->elements[i]); List_Destroy(COBpredBGs);
		for (int i = 0; i < remainingCIs->length; i++) free(remainingCIs->elements[i]); List_Destroy(remainingCIs);
		for (int i = 0; i < predCIs->length; i++) free(predCIs->elements[i]); List_Destroy(predCIs);
		return tempBasalFunctions.setTempBasal(rate, 30, profile, rT, currenttemp);
	}

	/*
	//	NJIT - Prevent Memory Leak
	if(rT.predBGs.COB != NULL)
	for (int i = 0; i < COBpredBGs->length; i++) free(COBpredBGs->elements[i]); List_Destroy(COBpredBGs);
	for (int i = 0; i < remainingCIs->length; i++) free(remainingCIs->elements[i]); List_Destroy(remainingCIs);
	for (int i = 0; i < predCIs->length; i++) free(predCIs->elements[i]); List_Destroy(predCIs);
	*/
}