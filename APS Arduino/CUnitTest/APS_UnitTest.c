#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "APS_Logic.h"
#include "APS_round-basal.h"
#include "APS_basal-set-temp.h"
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

/// <summary>
/// Creates a blank Temp for testing.
/// </summary>
/// <returns></returns>
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
	return iob_data;
}
/// <summary>
/// Creates a blank Autosens for testing.
/// </summary>
/// <returns></returns>
Autosens Create_Autosens() {
	Autosens autosens;
	autosens.ratio = 1;
	return autosens;
}

/// <summary>
/// Creates a blank MealData for testing.
/// </summary>
/// <returns></returns>
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
	return mealData;
}



void test_round_basal() {

	/*	Test 1 - no profile	*/
	{
		double basal = 0.025;
		Profile profile = Create_Profile();	//	default profile
		double output = round_basal(basal, profile);
		if (output == .05)
			printf("Test 1:\tshould round correctly without profile being passed in: SUCCESS\n");
		else
			printf("Test 1:\tshould round correctly without profile being passed in: FAILURE\n");
	}

	/*	Test 2 - old pump model */
	{
		double basal = 0.025;
		Profile profile = Create_Profile();
		snprintf(profile.model, sizeof(profile.model), "522");
		double output = round_basal(basal, profile);
		if (output == .05)
			printf("Test 2:\tshould round correctly with an old pump model: SUCCESS\n");
		else
			printf("Test 2:\tshould round correctly with an old pump model: FAILURE\n");
	}

	/*	Test 3 - new pump model */
	{
		double basal = 0.025;
		Profile profile = Create_Profile();
		snprintf(profile.model, sizeof(profile.model), "554");
		double output = round_basal(basal, profile);
		if (output == .025)
			printf("Test 3:\tshould round correctly with a new pump model: SUCCESS\n");
		else
			printf("Test 3:\tshould round correctly with a new pump model: FAILURE\n");
	}

	/*	Test 4 - new pump model */
	{
		double basal = 0.025;
		Profile profile = Create_Profile();
		snprintf(profile.model, sizeof(profile.model), "HelloThisIsntAPumpModel");
		double output = round_basal(basal, profile);
		if (output == .05)
			printf("Test 4:\tshould round correctly with an invalid pump model: SUCCESS\n");
		else
			printf("Test 4:\tshould round correctly with an invalid pump model: FAILURE\n");
	}

	/*	Test 5 - sample data */
	{
		double basalInput[6] = {
			0.83,
			0.86,
			1.83,
			1.86,
			10.83,
			10.86
		};
		double roundedOutput[6] = {
			0.85,
			0.85,
			1.85,
			1.85,
			10.8,
			10.9
		};
		int testsPassed = 0;
		for (int i = 0; i < 6; i++) {
			Profile profile = Create_Profile();
			double output = round_basal(basalInput[i], profile);
			if (output == roundedOutput[i])
				testsPassed++;
			else
				printf("Test 5.%d: should round basal rates properly (%.6lf -> %.6lf) but got %.6lf: FAILURE\n",
					i, basalInput[i], roundedOutput[i], output);
		}
		if(testsPassed == 6)
			printf("Test 5:\tshould round basal rates properly with data sample: SUCCESS\n");
		else
			printf("Test 5:\tshould round basal rates properly with data sample (%d/%d): FAILURE\n", testsPassed, 6);
	}
}
void test_determine_basal() {
	// standard initial conditions for all determine-basal test cases unless overridden
	Glucose_Status glucose_status = Create_GlucoseStatus();
		glucose_status.delta = 0;
		glucose_status.glucose = 115;
		glucose_status.long_avgdelta = 1.1;
		glucose_status.short_avgdelta = 0;
	Temp currenttemp = Create_Temp();
		currenttemp.duration = 0;
		currenttemp.rate = 0;
		snprintf(currenttemp.temp, sizeof(currenttemp.temp), "absolute");
	IOB_Data iob_data = Create_IOB_Data();
		iob_data.iob = 0;
		iob_data.activity = 0;
		iob_data.bolussnooze = 0;
	Autosens autosens = Create_Autosens();
		autosens.ratio = 1.0;
	Profile profile = Create_Profile();
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
	Meal_Data meal_data = Create_MealData();
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
		meal_data.allDeviations = malloc(sizeof(double) * 5);
		meal_data.allDeviations[0] = 0;
		meal_data.allDeviations[1] = 0;
		meal_data.allDeviations[2] = 0;
		meal_data.allDeviations[3] = 0;
		meal_data.allDeviations[4] = 0;
		meal_data.bwFound = 0;

	/*	Test 1 - should cancel high temp when in range w/o IOB	*/
	{
		Temp currenttemp = Create_Temp();
		currenttemp.duration = 30;
		currenttemp.rate = 1.5;
		snprintf(currenttemp.temp, sizeof(currenttemp.temp), "absolute");
		Temp output = determine_basal(glucose_status, currenttemp, &iob_data, profile, autosens, meal_data, APS_tempBasalFunctions, 1);
		if (output.rate == 0.9 && output.duration == 30)
			printf("\n\nTest 1:\tshould cancel high temp when in range w/o IOB (%.6lf and %.6lf): SUCCESS\n",
				output.rate, output.duration);
		else
			printf("\n\nTest 1:\tshould cancel high temp when in range w/o IOB (%.6lf should be 0.9 and %.6lf should be 30): FAILURE\n",
				output.rate, output.duration);
	}

	/*	Test 2 - low glucose suspend test cases */
	{
		Glucose_Status glucose_status = Create_GlucoseStatus();
		glucose_status.delta = -5;
		glucose_status.glucose = 75;
		glucose_status.long_avgdelta = -5;
		glucose_status.short_avgdelta = -5;
		Temp output = determine_basal(glucose_status, currenttemp, &iob_data, profile, autosens, meal_data, APS_tempBasalFunctions, 1);
		if (output.rate == 0 && output.duration > 29)
			printf("\n\nTest 2:\tshould temp to 0 when low w/o IOB (%.6lf and %.6lf): SUCCESS\n",
				output.rate, output.duration);
		else
			printf("\n\nTest 2:\tshould temp to 0 when low w/o IOB (%.6lf should be 0 and %.6lf should be above 29): FAILURE\n",
				output.rate, output.duration);
	}

	/*	Test 3 - less than 10m elapsed */
	{
		Temp currenttemp = Create_Temp();
		currenttemp.duration = 57;
		currenttemp.rate = 0;
		snprintf(currenttemp.temp, sizeof(currenttemp.temp), "absolute");
		Glucose_Status glucose_status = Create_GlucoseStatus();
		glucose_status.delta = -5;
		glucose_status.glucose = 75;
		glucose_status.long_avgdelta = -5;
		glucose_status.short_avgdelta = -5;
		Temp output = determine_basal(glucose_status, currenttemp, &iob_data, profile, autosens, meal_data, APS_tempBasalFunctions, 1);
		if (isnan(output.rate) && isnan(output.duration))
			printf("\n\nTest 3:\tshould not extend temp to 0 when <10m elapsed (%.6lf and %.6lf): SUCCESS\n",
				output.rate, output.duration);
		else
			printf("\n\nTest 3:\tshould not extend temp to 0 when <10m elapsed (%.6lf and %.6lf should both be NAN): FAILURE\n",
				output.rate, output.duration);
	}

	/*	Test 4 - should do nothing when low and rising w/o IOB */
	{
		Glucose_Status glucose_status = Create_GlucoseStatus();
		glucose_status.delta = 6;
		glucose_status.glucose = 75;
		glucose_status.long_avgdelta = 6;
		glucose_status.short_avgdelta = 6;
		Temp output = determine_basal(glucose_status, currenttemp, &iob_data, profile, autosens, meal_data, APS_tempBasalFunctions, 1);
		if(output.rate == 0.9 && output.duration == 30)
			printf("\n\nTest 4:\tshould do nothing when low and rising w/o IOB (%.6lf and %.6lf): SUCCESS\n",
				output.rate, output.duration);
		else
			printf("\n\nTest 4:\tshould do nothing when low and rising w/o IOB (%.6lf should be 0.9 and %.6lf should be 30): FAILURE\n",
				output.rate, output.duration);
	}
	/*	Test 5 - should temp to zero when rising slower than BGI */
	{
		Glucose_Status glucose_status = Create_GlucoseStatus();
		glucose_status.delta = 1;
		glucose_status.glucose = 75;
		glucose_status.long_avgdelta = 1;
		glucose_status.short_avgdelta = 1;
		IOB_Data iob_data = Create_IOB_Data();
		iob_data.iob = -0.5;
		iob_data.activity = -0.01;
		iob_data.bolussnooze = 0;
		Temp output = determine_basal(glucose_status, currenttemp, &iob_data, profile, autosens, meal_data, APS_tempBasalFunctions, 1);
		if (output.rate == 0 && output.duration == 30)
			printf("\n\nTest 5:\tshould temp to zero when rising slower than BGI (%.6lf and %.6lf): SUCCESS\n",
				output.rate, output.duration);
		else
			printf("\n\nTest 5:\tshould temp to zero when rising slower than BGI (%.6lf should be 0 and %.6lf should be 30): FAILURE\n",
				output.rate, output.duration);
	}
	/*	Test 6 - should temp to 0 when low and falling, regardless of BGI */
	{
		Glucose_Status glucose_status = Create_GlucoseStatus();
		glucose_status.delta = -1;
		glucose_status.glucose = 75;
		glucose_status.long_avgdelta = -1;
		glucose_status.short_avgdelta = -1;
		IOB_Data iob_data = Create_IOB_Data();
		iob_data.iob = 1;
		iob_data.activity = 0.01;
		iob_data.bolussnooze = 0.5;
		Temp output = determine_basal(glucose_status, currenttemp, &iob_data, profile, autosens, meal_data, APS_tempBasalFunctions, 1);
		if (output.rate == 0 && output.duration >= 29)
			printf("\n\nTest 6:\tshould temp to 0 when low and falling, regardless of BGI (%.6lf and %.6lf): SUCCESS\n",
				output.rate, output.duration);
		else
			printf("\n\nTest 6:\tshould temp to 0 when low and falling, regardless of BGI (%.6lf should be 0 and %.6lf should be >= 29): FAILURE\n",
				output.rate, output.duration);
	}
	/*	Test 7 - should high-temp when > 80-ish and rising w/ lots of negative IOB */
	{
		Glucose_Status glucose_status = Create_GlucoseStatus();
		glucose_status.delta = 5;
		glucose_status.glucose = 85;
		glucose_status.long_avgdelta = 5;
		glucose_status.short_avgdelta = 5;
		IOB_Data iob_data = Create_IOB_Data();
		iob_data.iob = -1;
		iob_data.activity = -0.01;
		iob_data.bolussnooze = 0;
		Temp output = determine_basal(glucose_status, currenttemp, &iob_data, profile, autosens, meal_data, APS_tempBasalFunctions, 1);
		if (output.rate > 1 && output.duration == 30)
			printf("\n\nTest 7:\tshould high-temp when > 80-ish and rising w/ lots of negative IOB (%.6lf and %.6lf): SUCCESS\n",
				output.rate, output.duration);
		else
			printf("\n\nTest 7:\tshould high-temp when > 80-ish and rising w/ lots of negative IOB (%.6lf should be > 1 and %.6lf should be 30): FAILURE\n",
				output.rate, output.duration);
	}
	/*	Test 8 - should high-temp when > 180-ish and rising but not more then maxSafeBasal */
	{
		Glucose_Status glucose_status = Create_GlucoseStatus();
		glucose_status.delta = 5;
		glucose_status.glucose = 185;
		glucose_status.long_avgdelta = 5;
		glucose_status.short_avgdelta = 5;
		IOB_Data iob_data = Create_IOB_Data();
		iob_data.iob = 0;
		iob_data.activity = -0.01;
		iob_data.bolussnooze = 0;
		Temp output = determine_basal(glucose_status, currenttemp, &iob_data, profile, autosens, meal_data, APS_tempBasalFunctions, 1);
		if (strstr(output.reason, "adj. req. rate:") != NULL)
			printf("\n\nTest 8:\tshould high-temp when > 180-ish and rising but not more then maxSafeBasal : SUCCESS\n");
		else
			printf("\n\nTest 8:\tshould high-temp when > 180-ish and rising but not more then maxSafeBasal : FAILURE\n");
	}
	/*	Test 9 - should reduce high-temp when schedule would be above max */
	{
		Glucose_Status glucose_status = Create_GlucoseStatus();
		glucose_status.delta = 5;
		glucose_status.glucose = 145;
		glucose_status.long_avgdelta = 5;
		glucose_status.short_avgdelta = 5;
		Temp currenttemp = Create_Temp();
		currenttemp.duration = 160;
		currenttemp.rate = 1.9;
		snprintf(currenttemp.temp, sizeof(currenttemp.temp), "absolute");
		IOB_Data iob_data = Create_IOB_Data();
		iob_data.iob = 0;
		iob_data.activity = -0.01;
		iob_data.bolussnooze = 0;
		Temp output = determine_basal(glucose_status, currenttemp, &iob_data, profile, autosens, meal_data, APS_tempBasalFunctions, 1);
		if (output.duration == 30)
			printf("\n\nTest 9:\tshould reduce high-temp when schedule would be above max (%.6lf): SUCCESS\n",
				output.duration);
		else
			printf("\n\nTest 9:\tsshould reduce high-temp when schedule would be above max (%.6lf should be 30): FAILURE\n",
				output.duration);
	}
	/*	Test 10	should continue high-temp when required ~= temp running*/
	{
		Glucose_Status glucose_status = Create_GlucoseStatus();
		glucose_status.delta = 5;
		glucose_status.glucose = 145;
		glucose_status.long_avgdelta = 5;
		glucose_status.short_avgdelta = 5;
		Temp currenttemp = Create_Temp();
		currenttemp.duration = 30;
		currenttemp.rate = 3.5;
		snprintf(currenttemp.temp, sizeof(currenttemp.temp), "absolute");
		IOB_Data iob_data = Create_IOB_Data();
		iob_data.iob = 0;
		iob_data.activity = -0.01;
		iob_data.bolussnooze = 0;
		Temp output = determine_basal(glucose_status, currenttemp, &iob_data, profile, autosens, meal_data, APS_tempBasalFunctions, 1);
		if (isnan(output.rate) && isnan(output.duration))
			printf("\n\nTest 10:\tshould continue high-temp when required ~= temp running (%.6lf and %.6lf): SUCCESS\n",
				output.rate, output.duration);
		else
			printf("\n\nTest 10:\tshould continue high-temp when required ~= temp running (%.6lf and %.6lf should both be NAN): FAILURE\n",
				output.rate, output.duration);
	}
	/*	Test 11	should set high-temp when required running temp is low */
	{
		Glucose_Status glucose_status = Create_GlucoseStatus();
		glucose_status.delta = 5;
		glucose_status.glucose = 145;
		glucose_status.long_avgdelta = 5;
		glucose_status.short_avgdelta = 5;
		Temp currenttemp = Create_Temp();
		currenttemp.duration = 30;
		currenttemp.rate = 1.1;
		snprintf(currenttemp.temp, sizeof(currenttemp.temp), "absolute");
		IOB_Data iob_data = Create_IOB_Data();
		iob_data.iob = 0;
		iob_data.activity = -0.01;
		iob_data.bolussnooze = 0;
		Temp output = determine_basal(glucose_status, currenttemp, &iob_data, profile, autosens, meal_data, APS_tempBasalFunctions, 1);
		if (output.duration == 30)
			printf("\n\nTest 11:\tshould set high-temp when required running temp is low (%.6lf): SUCCESS\n",
				output.duration);
		else
			printf("\n\nTest 11:\tshould set high-temp when required running temp is low (%.6lf = 30): FAILURE\n",
				output.duration);
	}
	/*	Test 12	should stop high-temp when iob is near max_iob. */
	{
		Glucose_Status glucose_status = Create_GlucoseStatus();
		glucose_status.delta = 5;
		glucose_status.glucose = 485;
		glucose_status.long_avgdelta = 5;
		glucose_status.short_avgdelta = 5;
		IOB_Data iob_data = Create_IOB_Data();
		iob_data.iob = 3.5;
		iob_data.activity = 0.05;
		iob_data.bolussnooze = 0;
		Temp output = determine_basal(glucose_status, currenttemp, &iob_data, profile, autosens, meal_data, APS_tempBasalFunctions, 1);
		if (output.rate == 0.9 && output.duration == 30)
			printf("\n\nTest 12:\tshould stop high-temp when iob is near max_iob. (%.6lf and %.6lf): SUCCESS\n",
				output.rate, output.duration);
		else
			printf("\n\nTest 12:\tshould stop high-temp when iob is near max_iob. (%.6lf = .9 and %.6lf = 30): FAILURE\n",
				output.rate, output.duration);
	}
	/*	Test 13	should temp to 0 when LOW w/ positive IOB */
	{
		Glucose_Status glucose_status = Create_GlucoseStatus();
		glucose_status.delta = 0;
		glucose_status.glucose = 39;
		glucose_status.long_avgdelta = -1.1;
		glucose_status.short_avgdelta = 0;
		IOB_Data iob_data = Create_IOB_Data();
		iob_data.iob = 1;
		iob_data.activity = 0.01;
		iob_data.bolussnooze = 0.5;
		Temp output = determine_basal(glucose_status, currenttemp, &iob_data, profile, autosens, meal_data, APS_tempBasalFunctions, 1);
		if (output.rate == 0 && output.duration > 29)
			printf("\n\nTest 13:\tshould temp to 0 when LOW w/ positive IOB (%.6lf and %.6lf): SUCCESS\n",
				output.rate, output.duration);
		else
			printf("\n\nTest 13:\tshould temp to 0 when LOW w/ positive IOB (%.6lf = 0 and %.6lf > 29): FAILURE\n",
				output.rate, output.duration);
	}
	/*	Test 14	should low temp when LOW w/ negative IOB */
	{
		Glucose_Status glucose_status = Create_GlucoseStatus();
		glucose_status.delta = 0;
		glucose_status.glucose = 39;
		glucose_status.long_avgdelta = -1.1;
		glucose_status.short_avgdelta = 0;
		IOB_Data iob_data = Create_IOB_Data();
		iob_data.iob = -2.5;
		iob_data.activity = -0.03;
		iob_data.bolussnooze = 0;
		Temp output = determine_basal(glucose_status, currenttemp, &iob_data, profile, autosens, meal_data, APS_tempBasalFunctions, 1);
		if (output.rate < 0.8 && output.duration > 29)
			printf("\n\nTest 14:\tshould low temp when LOW w/ negative IOB (%.6lf and %.6lf): SUCCESS\n",
				output.rate, output.duration);
		else
			printf("\n\nTest 14:\tshould low temp when LOW w/ negative IOB (%.6lf < 0.8 and %.6lf > 29): FAILURE\n",
				output.rate, output.duration);
	}
	/*	Test 15	should low-temp when eventualBG < min_bg */
	{
		Glucose_Status glucose_status = Create_GlucoseStatus();
		glucose_status.delta = -3;
		glucose_status.glucose = 110;
		glucose_status.long_avgdelta = -1;
		glucose_status.short_avgdelta = -1;
		Temp output = determine_basal(glucose_status, currenttemp, &iob_data, profile, autosens, meal_data, APS_tempBasalFunctions, 1);
		if (output.rate < 0.8 && output.duration > 29)
			printf("\n\nTest 15:\tshould low-temp when eventualBG < min_bg (%.6lf and %.6lf): SUCCESS\n",
				output.rate, output.duration);
		else
			printf("\n\nTest 15:\tshould low-temp when eventualBG < min_bg (%.6lf < 0.8 and %.6lf > 29): FAILURE\n",
				output.rate, output.duration);
	}
	/*	Test 16	should low-temp when eventualBG < min_bg with delta > exp. delta */
	{
		Glucose_Status glucose_status = Create_GlucoseStatus();
		glucose_status.delta = -5;
		glucose_status.glucose = 115;
		glucose_status.long_avgdelta = -6;
		glucose_status.short_avgdelta = -6;
		IOB_Data iob_data = Create_IOB_Data();
		iob_data.iob = 2;
		iob_data.activity = 0.05;
		iob_data.bolussnooze = 0;
		Temp output = determine_basal(glucose_status, currenttemp, &iob_data, profile, autosens, meal_data, APS_tempBasalFunctions, 1);
		if (output.rate < 0.2 && output.duration > 29)
			printf("\n\nTest 16:\tshould low-temp when eventualBG < min_bg with delta > exp. delta (%.6lf and %.6lf): SUCCESS\n",
				output.rate, output.duration);
		else
			printf("\n\nTest 16:\tshould low-temp when eventualBG < min_bg with delta > exp. delta (%.6lf < 0.2 and %.6lf > 29): FAILURE\n",
				output.rate, output.duration);
	}
	/*	Test 17	should low-temp when eventualBG < min_bg with delta > exp. delta */
	{
		Glucose_Status glucose_status = Create_GlucoseStatus();
		glucose_status.delta = -2;
		glucose_status.glucose = 156;
		glucose_status.long_avgdelta = -1.33;
		glucose_status.short_avgdelta = -1.33;
		IOB_Data iob_data = Create_IOB_Data();
		iob_data.iob = 3.51;
		iob_data.activity = 0.06;
		iob_data.bolussnooze = 0;
		Temp output = determine_basal(glucose_status, currenttemp, &iob_data, profile, autosens, meal_data, APS_tempBasalFunctions, 1);
		if (output.rate < 0.8 && output.duration == 30)
			printf("\n\nTest 17:\tshould low-temp when eventualBG < min_bg with delta > exp. delta (%.6lf and %.6lf): SUCCESS\n",
				output.rate, output.duration);
		else
			printf("\n\nTest 17:\tshould low-temp when eventualBG < min_bg with delta > exp. delta (%.6lf < 0.8 and %.6lf = 30): FAILURE\n",
				output.rate, output.duration);
	}
	/*	Test 18	should low-temp much less when eventualBG < min_bg with delta barely negative */
	{
		Glucose_Status glucose_status = Create_GlucoseStatus();
		glucose_status.delta = -1;
		glucose_status.glucose = 115;
		glucose_status.long_avgdelta = -1;
		glucose_status.short_avgdelta = -1;
		IOB_Data iob_data = Create_IOB_Data();
		iob_data.iob = 2;
		iob_data.activity = 0.05;
		iob_data.bolussnooze = 0;
		Temp output = determine_basal(glucose_status, currenttemp, &iob_data, profile, autosens, meal_data, APS_tempBasalFunctions, 1);
		if (output.rate > 0.3 && output.rate < 0.8 && output.duration == 30)
			printf("\n\nTest 18:\tshould low-temp much less when eventualBG < min_bg with delta barely negative (%.6lf and %.6lf): SUCCESS\n",
				output.rate, output.duration);
		else
			printf("\n\nTest 18:\tshould low-temp much less when eventualBG < min_bg with delta barely negative (0.3 < %.6lf < 0.8 and %.6lf = 30): FAILURE\n",
				output.rate, output.duration);
	}
	/*	Test 19	should cancel low-temp when lowish and avg.delta rising faster than BGI */
	{
		Temp currenttemp = Create_Temp();
		currenttemp.duration = 20;
		currenttemp.rate = 0.5;
		Glucose_Status glucose_status = Create_GlucoseStatus();
		glucose_status.delta = 3;
		glucose_status.glucose = 85;
		glucose_status.long_avgdelta = 3;
		glucose_status.short_avgdelta = 3;
		IOB_Data iob_data = Create_IOB_Data();
		iob_data.iob = -0.7;
		iob_data.activity = -0.01;
		iob_data.bolussnooze = 0;
		Temp output = determine_basal(glucose_status, currenttemp, &iob_data, profile, autosens, meal_data, APS_tempBasalFunctions, 1);
		if (output.rate > 0.8 && output.duration == 30)
			printf("\n\nTest 19:\tshould cancel low-temp when lowish and avg.delta rising faster than BGI (%.6lf and %.6lf): SUCCESS\n",
				output.rate, output.duration);
		else
			printf("\n\nTest 19:\tshould cancel low-temp when lowish and avg.delta rising faster than BGI (%.6lf > 0.8 and %.6lf = 30): FAILURE\n",
				output.rate, output.duration);
	}
	/*	Test 20	should set current basal as temp when lowish and delta rising faster than BGI */
	{
		Temp currenttemp = Create_Temp();
		currenttemp.duration = 0;
		currenttemp.rate = 0.5;
		Glucose_Status glucose_status = Create_GlucoseStatus();
		glucose_status.delta = 3;
		glucose_status.glucose = 85;
		glucose_status.long_avgdelta = 3;
		glucose_status.short_avgdelta = 3;
		IOB_Data iob_data = Create_IOB_Data();
		iob_data.iob = -0.7;
		iob_data.activity = -0.01;
		iob_data.bolussnooze = 0;
		Temp output = determine_basal(glucose_status, currenttemp, &iob_data, profile, autosens, meal_data, APS_tempBasalFunctions, 1);
		if (output.rate == 0.9 && output.duration == 30)
			printf("\n\nTest 20:\tshould set current basal as temp when lowish and delta rising faster than BGI (%.6lf and %.6lf): SUCCESS\n",
				output.rate, output.duration);
		else
			printf("\n\nTest 20:\tshould set current basal as temp when lowish and delta rising faster than BGI (%.6lf = 0.9 and %.6lf = 30): FAILURE\n",
				output.rate, output.duration);
	}
}
void startTest() {
	InitAPS();
	test_round_basal();
	test_determine_basal();
}
int main() {
	startTest();
	while (1);
	return 0;
}