#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "APS_Logic.h"
#include "APS_round-basal.h"
#include "APS_basal-set-temp.h"

int test_round_basal() {
	int passed = 0;
	int numtests = 0;
	/*	Test 1 - no profile	*/
	{
		numtests++;
		double basal = 0.025;
		Profile profile = Create_Profile();	//	default profile
		double output = round_basal(basal, profile);
		if (output == .05)
		{
			printf("Test 1:\tshould round correctly without profile being passed in: SUCCESS\n");
			passed++;
		}
		else
			printf("Test 1:\tshould round correctly without profile being passed in: FAILURE\n");
	}

	/*	Test 2 - old pump model */
	{
		numtests++;
		double basal = 0.025;
		Profile profile = Create_Profile();
		snprintf(profile.model, sizeof(profile.model), "522");
		double output = round_basal(basal, profile);
		if (output == .05)
		{
			printf("Test 2:\tshould round correctly with an old pump model: SUCCESS\n");
			passed++;
		}
		else
			printf("Test 2:\tshould round correctly with an old pump model: FAILURE\n");
	}

	/*	Test 3 - new pump model */
	{
		numtests++;
		double basal = 0.025;
		Profile profile = Create_Profile();
		snprintf(profile.model, sizeof(profile.model), "554");
		double output = round_basal(basal, profile);
		if (output == .025)
		{
			printf("Test 3:\tshould round correctly with a new pump model: SUCCESS\n");
			passed++;
		}
		else
			printf("Test 3:\tshould round correctly with a new pump model: FAILURE\n");
	}

	/*	Test 4 - new pump model */
	{
		numtests++;
		double basal = 0.025;
		Profile profile = Create_Profile();
		snprintf(profile.model, sizeof(profile.model), "HelloThisIsntAPumpModel");
		double output = round_basal(basal, profile);
		if (output == .05)
		{
			printf("Test 4:\tshould round correctly with an invalid pump model: SUCCESS\n");
			passed++;
		}
		else
			printf("Test 4:\tshould round correctly with an invalid pump model: FAILURE\n");
	}

	/*	Test 5 - sample data */
	{
		numtests++;
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
		{
			passed++;
			printf("Test 5:\tshould round basal rates properly with data sample: SUCCESS\n");
		}
		else
			printf("Test 5:\tshould round basal rates properly with data sample (%d/%d): FAILURE\n", testsPassed, 6);
	}
	printf("\n--------------------------------\nSummary for Round Basal: %d/%d tests passed\n", passed, numtests);
	return passed = numtests;
}
int test_determine_basal() {
	int testsCount = 0;
	int testsPassed = 0;

	// standard initial conditions for all determine-basal test cases unless overridden
	Glucose_Status glucose_status = Create_GlucoseStatus();
		glucose_status.delta = 0;
		glucose_status.glucose = 115;
		glucose_status.long_avgdelta = 1.1;
		glucose_status.short_avgdelta = 0;
	Temp currenttemp = Create_Temp();
		currenttemp.duration = 0;
		currenttemp.rate = 0;
		snprintf((char* const)currenttemp.temp, sizeof(currenttemp.temp), "absolute");
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
		testsCount++;
		Temp currenttemp = Create_Temp();
		currenttemp.duration = 30;
		currenttemp.rate = 1.5;
		snprintf(currenttemp.temp, sizeof(currenttemp.temp), "absolute");
		Temp output = determine_basal(glucose_status, currenttemp, &iob_data, profile, autosens, meal_data, APS_tempBasalFunctions, 1);
		if (output.rate == 0.9 && output.duration == 30)
		{
			printf("\n\nTest 1:\tshould cancel high temp when in range w/o IOB (%.6lf and %.6lf): SUCCESS\n",
				output.rate, output.duration);
			testsPassed++;
		}
		else
			printf("\n\nTest 1:\tshould cancel high temp when in range w/o IOB (%.6lf should be 0.9 and %.6lf should be 30): FAILURE\n",
				output.rate, output.duration);
	}

	/*	Test 2 - low glucose suspend test cases */
	{
		testsCount++;
		Glucose_Status glucose_status = Create_GlucoseStatus();
		glucose_status.delta = -5;
		glucose_status.glucose = 75;
		glucose_status.long_avgdelta = -5;
		glucose_status.short_avgdelta = -5;
		Temp output = determine_basal(glucose_status, currenttemp, &iob_data, profile, autosens, meal_data, APS_tempBasalFunctions, 1);
		if (output.rate == 0 && output.duration > 29)
		{
			printf("\n\nTest 2:\tshould temp to 0 when low w/o IOB (%.6lf and %.6lf): SUCCESS\n",
				output.rate, output.duration);
			testsPassed++;
		}
		else
			printf("\n\nTest 2:\tshould temp to 0 when low w/o IOB (%.6lf should be 0 and %.6lf should be above 29): FAILURE\n",
				output.rate, output.duration);
	}

	/*	Test 3 - less than 10m elapsed */
	{
		testsCount++;
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
		{
			printf("\n\nTest 3:\tshould not extend temp to 0 when <10m elapsed (%.6lf and %.6lf): SUCCESS\n",
				output.rate, output.duration);
			testsPassed++;
		}
		else
			printf("\n\nTest 3:\tshould not extend temp to 0 when <10m elapsed (%.6lf and %.6lf should both be NAN): FAILURE\n",
				output.rate, output.duration);
	}

	/*	Test 4 - should do nothing when low and rising w/o IOB */
	{
		testsCount++;
		Glucose_Status glucose_status = Create_GlucoseStatus();
		glucose_status.delta = 6;
		glucose_status.glucose = 75;
		glucose_status.long_avgdelta = 6;
		glucose_status.short_avgdelta = 6;
		Temp output = determine_basal(glucose_status, currenttemp, &iob_data, profile, autosens, meal_data, APS_tempBasalFunctions, 1);
		if(output.rate == 0.9 && output.duration == 30)
		{
			printf("\n\nTest 4:\tshould do nothing when low and rising w/o IOB (%.6lf and %.6lf): SUCCESS\n",
				output.rate, output.duration);
			testsPassed++;
		}
		else
			printf("\n\nTest 4:\tshould do nothing when low and rising w/o IOB (%.6lf should be 0.9 and %.6lf should be 30): FAILURE\n",
				output.rate, output.duration);
	}
	/*	Test 5 - should temp to zero when rising slower than BGI */
	{
		testsCount++;
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
		{
			printf("\n\nTest 5:\tshould temp to zero when rising slower than BGI (%.6lf and %.6lf): SUCCESS\n",
				output.rate, output.duration);
			testsPassed++;
		}
		else
			printf("\n\nTest 5:\tshould temp to zero when rising slower than BGI (%.6lf should be 0 and %.6lf should be 30): FAILURE\n",
				output.rate, output.duration);
	}
	/*	Test 6 - should temp to 0 when low and falling, regardless of BGI */
	{
		testsCount++;
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
		{
			printf("\n\nTest 6:\tshould temp to 0 when low and falling, regardless of BGI (%.6lf and %.6lf): SUCCESS\n",
				output.rate, output.duration);
			testsPassed++;
		}
		else
			printf("\n\nTest 6:\tshould temp to 0 when low and falling, regardless of BGI (%.6lf should be 0 and %.6lf should be >= 29): FAILURE\n",
				output.rate, output.duration);
	}
	/*	Test 7 - should high-temp when > 80-ish and rising w/ lots of negative IOB */
	{
		testsCount++;
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
		{
			printf("\n\nTest 7:\tshould high-temp when > 80-ish and rising w/ lots of negative IOB (%.6lf and %.6lf): SUCCESS\n",
				output.rate, output.duration);
			testsPassed++;
		}
		else
			printf("\n\nTest 7:\tshould high-temp when > 80-ish and rising w/ lots of negative IOB (%.6lf should be > 1 and %.6lf should be 30): FAILURE\n",
				output.rate, output.duration);
	}
	/*	Test 8 - should high-temp when > 180-ish and rising but not more then maxSafeBasal */
	{
		testsCount++;
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
		{
			printf("\n\nTest 8:\tshould high-temp when > 180-ish and rising but not more then maxSafeBasal : SUCCESS\n");
			testsPassed++;
		}
		else
			printf("\n\nTest 8:\tshould high-temp when > 180-ish and rising but not more then maxSafeBasal : FAILURE\n");
	}
	/*	Test 9 - should reduce high-temp when schedule would be above max */
	{
		testsCount++;
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
		{
			printf("\n\nTest 9:\tshould reduce high-temp when schedule would be above max (%.6lf): SUCCESS\n",
				output.duration);
			testsPassed++;
		}
		else
			printf("\n\nTest 9:\tsshould reduce high-temp when schedule would be above max (%.6lf should be 30): FAILURE\n",
				output.duration);
	}
	/*	Test 10	should continue high-temp when required ~= temp running*/
	{
		testsCount++;
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
		{
			printf("\n\nTest 10:\tshould continue high-temp when required ~= temp running (%.6lf and %.6lf): SUCCESS\n",
				output.rate, output.duration);
			testsPassed++;
		}
		else
			printf("\n\nTest 10:\tshould continue high-temp when required ~= temp running (%.6lf and %.6lf should both be NAN): FAILURE\n",
				output.rate, output.duration);
	}
	/*	Test 11	should set high-temp when required running temp is low */
	{
		testsCount++;
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
		{
			printf("\n\nTest 11:\tshould set high-temp when required running temp is low (%.6lf): SUCCESS\n",
				output.duration);
			testsPassed++;
		}
		else
			printf("\n\nTest 11:\tshould set high-temp when required running temp is low (%.6lf = 30): FAILURE\n",
				output.duration);
	}
	/*	Test 12	should stop high-temp when iob is near max_iob. */
	{
		testsCount++;
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
		{
			printf("\n\nTest 12:\tshould stop high-temp when iob is near max_iob. (%.6lf and %.6lf): SUCCESS\n",
				output.rate, output.duration);
			testsPassed++;
		}
		else
			printf("\n\nTest 12:\tshould stop high-temp when iob is near max_iob. (%.6lf = .9 and %.6lf = 30): FAILURE\n",
				output.rate, output.duration);
	}
	/*	Test 13	should temp to 0 when LOW w/ positive IOB */
	{
		testsCount++;
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
		{
			printf("\n\nTest 13:\tshould temp to 0 when LOW w/ positive IOB (%.6lf and %.6lf): SUCCESS\n",
				output.rate, output.duration);
			testsPassed++;
		}
		else
			printf("\n\nTest 13:\tshould temp to 0 when LOW w/ positive IOB (%.6lf = 0 and %.6lf > 29): FAILURE\n",
				output.rate, output.duration);
	}
	/*	Test 14	should low temp when LOW w/ negative IOB */
	{
		testsCount++;
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
		{
			printf("\n\nTest 14:\tshould low temp when LOW w/ negative IOB (%.6lf and %.6lf): SUCCESS\n",
				output.rate, output.duration);
			testsPassed++;
		}
		else
			printf("\n\nTest 14:\tshould low temp when LOW w/ negative IOB (%.6lf < 0.8 and %.6lf > 29): FAILURE\n",
				output.rate, output.duration);
	}
	/*	Test 15	should low-temp when eventualBG < min_bg */
	{
		testsCount++;
		Glucose_Status glucose_status = Create_GlucoseStatus();
		glucose_status.delta = -3;
		glucose_status.glucose = 110;
		glucose_status.long_avgdelta = -1;
		glucose_status.short_avgdelta = -1;
		Temp output = determine_basal(glucose_status, currenttemp, &iob_data, profile, autosens, meal_data, APS_tempBasalFunctions, 1);
		if (output.rate < 0.8 && output.duration > 29)
		{
			printf("\n\nTest 15:\tshould low-temp when eventualBG < min_bg (%.6lf and %.6lf): SUCCESS\n",
				output.rate, output.duration);
			testsPassed++;
		}
		else
			printf("\n\nTest 15:\tshould low-temp when eventualBG < min_bg (%.6lf < 0.8 and %.6lf > 29): FAILURE\n",
				output.rate, output.duration);
	}
	/*	Test 16	should low-temp when eventualBG < min_bg with delta > exp. delta */
	{
		testsCount++;
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
		{
			printf("\n\nTest 16:\tshould low-temp when eventualBG < min_bg with delta > exp. delta (%.6lf and %.6lf): SUCCESS\n",
				output.rate, output.duration);
			testsPassed++;
		}
		else
			printf("\n\nTest 16:\tshould low-temp when eventualBG < min_bg with delta > exp. delta (%.6lf < 0.2 and %.6lf > 29): FAILURE\n",
				output.rate, output.duration);
	}
	/*	Test 17	should low-temp when eventualBG < min_bg with delta > exp. delta */
	{
		testsCount++;
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
		{
			printf("\n\nTest 17:\tshould low-temp when eventualBG < min_bg with delta > exp. delta (%.6lf and %.6lf): SUCCESS\n",
				output.rate, output.duration);
			testsPassed++;
		}
		else
			printf("\n\nTest 17:\tshould low-temp when eventualBG < min_bg with delta > exp. delta (%.6lf < 0.8 and %.6lf = 30): FAILURE\n",
				output.rate, output.duration);
	}
	/*	Test 18	should low-temp much less when eventualBG < min_bg with delta barely negative */
	{
		testsCount++;
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
		{
			printf("\n\nTest 18:\tshould low-temp much less when eventualBG < min_bg with delta barely negative (%.6lf and %.6lf): SUCCESS\n",
				output.rate, output.duration);
			testsPassed++;
		}
		else
			printf("\n\nTest 18:\tshould low-temp much less when eventualBG < min_bg with delta barely negative (0.3 < %.6lf < 0.8 and %.6lf = 30): FAILURE\n",
				output.rate, output.duration);
	}
	/*	Test 19	should cancel low-temp when lowish and avg.delta rising faster than BGI */
	{
		testsCount++;
		Temp currenttemp = Create_Temp();
		currenttemp.duration = 20;
		currenttemp.rate = 0.5;
		snprintf(currenttemp.temp, sizeof(currenttemp.temp), "absolute");
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
		{
			printf("\n\nTest 19:\tshould cancel low-temp when lowish and avg.delta rising faster than BGI (%.6lf and %.6lf): SUCCESS\n",
				output.rate, output.duration);
			testsPassed++;
		}
		else
			printf("\n\nTest 19:\tshould cancel low-temp when lowish and avg.delta rising faster than BGI (%.6lf > 0.8 and %.6lf = 30): FAILURE\n",
				output.rate, output.duration);
	}
	/*	Test 20	should set current basal as temp when lowish and delta rising faster than BGI */
	{
		testsCount++;
		Temp currenttemp = Create_Temp();
		currenttemp.duration = 0;
		currenttemp.rate = 0.5;
		snprintf(currenttemp.temp, sizeof(currenttemp.temp), "absolute");
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
		{
			printf("\n\nTest 20:\tshould set current basal as temp when lowish and delta rising faster than BGI (%.6lf and %.6lf): SUCCESS\n",
				output.rate, output.duration);
			testsPassed++;
		}
		else
			printf("\n\nTest 20:\tshould set current basal as temp when lowish and delta rising faster than BGI (%.6lf = 0.9 and %.6lf = 30): FAILURE\n",
				output.rate, output.duration);
	}
	/*	Test 21	should low-temp when low and rising slower than BGI */
	{
		testsCount++;
		Glucose_Status glucose_status = Create_GlucoseStatus();
		glucose_status.delta = 1;
		glucose_status.glucose = 85;
		glucose_status.long_avgdelta = 1;
		glucose_status.short_avgdelta = 1;
		IOB_Data iob_data = Create_IOB_Data();
		iob_data.iob = -0.5;
		iob_data.activity = -0.01;
		iob_data.bolussnooze = 0;
		Temp output = determine_basal(glucose_status, currenttemp, &iob_data, profile, autosens, meal_data, APS_tempBasalFunctions, 1);
		if (output.rate < 0.8 && output.duration == 30)
		{
			printf("\n\nTest 21:\tshould low-temp when low and rising slower than BGI (%.6lf and %.6lf): SUCCESS\n",
				output.rate, output.duration);
			testsPassed++;
		}
		else
			printf("\n\nTest 21:\tshould low-temp when low and rising slower than BGI (%.6lf < 0.8 and %.6lf = 30): FAILURE\n",
				output.rate, output.duration);
	}
	/*	Test 22	should high-temp when eventualBG > max_bg */
	{
		testsCount++;
		Glucose_Status glucose_status = Create_GlucoseStatus();
		glucose_status.delta = 3;
		glucose_status.glucose = 120;
		glucose_status.long_avgdelta = 0;
		glucose_status.short_avgdelta = 1;
		Temp output = determine_basal(glucose_status, currenttemp, &iob_data, profile, autosens, meal_data, APS_tempBasalFunctions, 1);
		if (output.rate > 1 && output.duration == 30)
		{
			printf("\n\nTest 22:\tshould high-temp when eventualBG > max_bg (%.6lf and %.6lf): SUCCESS\n",
				output.rate, output.duration);
			testsPassed++;
		}
		else
			printf("\n\nTest 22:\tshould high-temp when eventualBG > max_bg (%.6lf > 1 and %.6lf = 30): FAILURE\n",
				output.rate, output.duration);
	}
	/*	Test 23	should cancel high-temp when high and avg. delta falling faster than BGI */
	{
		testsCount++;
		Temp currenttemp = Create_Temp();
		currenttemp.duration = 20;
		currenttemp.rate = 2;
		snprintf(currenttemp.temp, sizeof(currenttemp.temp), "absolute");
		Glucose_Status glucose_status = Create_GlucoseStatus();
		glucose_status.delta = -5;
		glucose_status.glucose = 175;
		glucose_status.long_avgdelta = -5;
		glucose_status.short_avgdelta = -5;
		IOB_Data iob_data = Create_IOB_Data();
		iob_data.iob = 1;
		iob_data.activity = 0.01;
		iob_data.bolussnooze = 0;
		Temp output = determine_basal(glucose_status, currenttemp, &iob_data, profile, autosens, meal_data, APS_tempBasalFunctions, 1);
		if (output.rate == 0.9 && output.duration == 30)
		{
			printf("\n\nTest 23:\tshould cancel high-temp when high and avg. delta falling faster than BGI (%.6lf and %.6lf): SUCCESS\n",
				output.rate, output.duration);
			testsPassed++;
		}
		else
			printf("\n\nTest 23:\tshould cancel high-temp when high and avg. delta falling faster than BGI (%.6lf = 0.9 and %.6lf = 30): FAILURE\n",
				output.rate, output.duration);
	}
	/*	Test 24	should cancel high-temp when high and delta falling faster than BGI */
	{
		testsCount++;
		Temp currenttemp = Create_Temp();
		currenttemp.duration = 20;
		currenttemp.rate = 2;
		snprintf(currenttemp.temp, sizeof(currenttemp.temp), "absolute");
		Glucose_Status glucose_status = Create_GlucoseStatus();
		glucose_status.delta = -5;
		glucose_status.glucose = 175;
		glucose_status.long_avgdelta = -4;
		glucose_status.short_avgdelta = -4;
		IOB_Data iob_data = Create_IOB_Data();
		iob_data.iob = 1;
		iob_data.activity = 0.01;
		iob_data.bolussnooze = 0;
		Temp output = determine_basal(glucose_status, currenttemp, &iob_data, profile, autosens, meal_data, APS_tempBasalFunctions, 1);
		if (output.rate == 0.9 && output.duration == 30)
		{
			printf("\n\nTest 24:\tshould cancel high-temp when high and delta falling faster than BGI (%.6lf and %.6lf): SUCCESS\n",
				output.rate, output.duration);
			testsPassed++;
		}
		else
			printf("\n\nTest 24:\tshould cancel high-temp when high and delta falling faster than BGI (%.6lf = 0.9 and %.6lf = 30): FAILURE\n",
				output.rate, output.duration);
	}
	/*	Test 25	should do nothing when no temp and high and delta falling faster than BGI */
	{
		testsCount++;
		Temp currenttemp = Create_Temp();
		currenttemp.duration = 0;
		currenttemp.rate = 0;
		snprintf(currenttemp.temp, sizeof(currenttemp.temp), "absolute");
		Glucose_Status glucose_status = Create_GlucoseStatus();
		glucose_status.delta = -5;
		glucose_status.glucose = 175;
		glucose_status.long_avgdelta = -4;
		glucose_status.short_avgdelta = -4;
		IOB_Data iob_data = Create_IOB_Data();
		iob_data.iob = 1;
		iob_data.activity = 0.01;
		iob_data.bolussnooze = 0;
		Temp output = determine_basal(glucose_status, currenttemp, &iob_data, profile, autosens, meal_data, APS_tempBasalFunctions, 1);
		if (output.rate == 0.9 && output.duration == 30)
		{
			printf("\n\nTest 25:\tshould do nothing when no temp and high and delta falling faster than BGI (%.6lf and %.6lf): SUCCESS\n",
				output.rate, output.duration);
			testsPassed++;
		}
		else
			printf("\n\nTest 25:\tshould do nothing when no temp and high and delta falling faster than BGI (%.6lf = 0.9 and %.6lf = 30): FAILURE\n",
				output.rate, output.duration);
	}
	/*	Test 26	should high-temp when high and falling slower than BGI */
	{
		testsCount++;
		Glucose_Status glucose_status = Create_GlucoseStatus();
		glucose_status.delta = -1;
		glucose_status.glucose = 175;
		glucose_status.long_avgdelta = -1;
		glucose_status.short_avgdelta = -1;
		IOB_Data iob_data = Create_IOB_Data();
		iob_data.iob = 1;
		iob_data.activity = 0.01;
		iob_data.bolussnooze = 0;
		Temp output = determine_basal(glucose_status, currenttemp, &iob_data, profile, autosens, meal_data, APS_tempBasalFunctions, 1);
		if (output.rate > 1 && output.duration == 30)
		{
			printf("\n\nTest 26:\tshould high-temp when high and falling slower than BGI (%.6lf and %.6lf): SUCCESS\n",
				output.rate, output.duration);
			testsPassed++;
		}
		else
			printf("\n\nTest 26:\tshould high-temp when high and falling slower than BGI (%.6lf > 1 and %.6lf = 30): FAILURE\n",
				output.rate, output.duration);
	}
	/*	Test 27	should high-temp when high and falling slowly with low insulin activity */
	{
		testsCount++;
		Glucose_Status glucose_status = Create_GlucoseStatus();
		glucose_status.delta = -1;
		glucose_status.glucose = 300;
		glucose_status.long_avgdelta = -1;
		glucose_status.short_avgdelta = -1;
		IOB_Data iob_data = Create_IOB_Data();
		iob_data.iob = 0.5;
		iob_data.activity = 0.005;
		iob_data.bolussnooze = 0;
		Temp output = determine_basal(glucose_status, currenttemp, &iob_data, profile, autosens, meal_data, APS_tempBasalFunctions, 1);
		if (output.rate > 2.5 && output.duration == 30)
		{
			printf("\n\nTest 27:\tshould high-temp when high and falling slowly with low insulin activity (%.6lf and %.6lf): SUCCESS\n",
				output.rate, output.duration);
			testsPassed++;
		}
		else
			printf("\n\nTest 27:\tshould high-temp when high and falling slowly with low insulin activity (%.6lf > 2.5 and %.6lf = 30): FAILURE\n",
				output.rate, output.duration);
	}
	/*	Test 28	should profile.current_basal be undefined return error */
	{
		testsCount++;

		Temp output = determine_basal(Create_GlucoseStatus(), 
			Create_Temp(), NULL, Create_Profile(), Create_Autosens(), Create_MealData(), APS_tempBasalFunctions, 0);
		if (strcmp(output.error, "Error: could not get current basal rate") == 0)
		{
			printf("\n\nTest 28:\tshould profile.current_basal be undefined return error (Error: could not get current basal rate): SUCCESS\n");
			testsPassed++;
		}
		else
			printf("\n\nTest 28:\tshould profile.current_basal be undefined return error (%s) should be \"Error: could not get current basal rate\": FAILURE\n",
				output.error);
	}
	/*	Test 29	should let low-temp run when bg < 30 (Dexcom is in ???) */
	{
		testsCount++;
		Temp currenttemp = Create_Temp();
		currenttemp.duration = 30;
		currenttemp.rate = 0;
		snprintf(currenttemp.temp, sizeof(currenttemp.temp), "absolute");
		Glucose_Status glucose_status = Create_GlucoseStatus();
		glucose_status.glucose = 10;
		Temp output = determine_basal(glucose_status, currenttemp, &iob_data, profile, autosens, meal_data, APS_tempBasalFunctions, 1);
		if (isnan(output.rate))
		{
			printf("\n\nTest 29:\tshould let low-temp run when bg < 30 (Dexcom is in ???) (%.6lf): SUCCESS\n",
				output.rate);
			testsPassed++;
		}
		else
			printf("\n\nTest 29:\tshould let low-temp run when bg < 30 (Dexcom is in ???) (%.6lf = NAN): FAILURE\n",
				output.rate);
	}
	/*	Test 30	should cancel high-temp when bg < 30 (Dexcom is in ???) */
	{
		testsCount++;
		Temp currenttemp = Create_Temp();
		currenttemp.duration = 30;
		currenttemp.rate = 2;
		snprintf(currenttemp.temp, sizeof(currenttemp.temp), "absolute");
		Glucose_Status glucose_status = Create_GlucoseStatus();
		glucose_status.glucose = 10;
		Temp output = determine_basal(glucose_status, currenttemp, &iob_data, profile, autosens, meal_data, APS_tempBasalFunctions, 1);
		if (output.rate < 1)
		{
			printf("\n\nTest 30:\tshould cancel high-temp when bg < 30 (Dexcom is in ???) (%.6lf): SUCCESS\n",
				output.rate);
			testsPassed++;
		}
		else
			printf("\n\nTest 30:\tshould cancel high-temp when bg < 30 (Dexcom is in ???) (%.6lf < 1): FAILURE\n",
				output.rate);
	}
	/*	Test 31	profile should contain min_bg,max_bg */
	{
		testsCount++;
		Glucose_Status glucose_status = Create_GlucoseStatus();
		glucose_status.glucose = 100;
		Profile profile = Create_Profile();
		profile.current_basal = 0.0;
		Temp output = determine_basal(glucose_status,
			Create_Temp(), NULL, profile, Create_Autosens(), Create_MealData(), APS_tempBasalFunctions, 0);
		if (strcmp(output.error, "Error: could not determine target_bg. ") == 0)
		{
			printf("\n\nTest 31:\tprofile should contain min_bg,max_bg (%s): SUCCESS\n",
				output.error);
			testsPassed++;
		}
		else
			printf("\n\nTest 31:\tprofile should contain min_bg,max_bg (%s) should be \"Error: could not determine target_bg. \": FAILURE\n",
				output.error);
	}
	/*	Test 32	iob_data should not be undefined */
	{
		testsCount++;
		Glucose_Status glucose_status = Create_GlucoseStatus();
		glucose_status.glucose = 100;
		Profile profile = Create_Profile();
		profile.current_basal = 0.0;
		profile.min_bg = 100;
		profile.max_bg = 1100;
		Temp output = determine_basal(glucose_status,
			Create_Temp(), NULL, profile, Create_Autosens(), Create_MealData(), APS_tempBasalFunctions, 0);
		if (strcmp(output.error, "Error: iob_data undefined. ") == 0)
		{
			printf("\n\nTest 32:\tiob_data should not be undefined (%s): SUCCESS\n",
				output.error);
			testsPassed++;
		}
		else
			printf("\n\nTest 32:\tiob_data should not be undefined (%s) should be \"Error: iob_data undefined. \": FAILURE\n",
				output.error);
	}
	/*	Test 33	should do nothing when requested temp already running with >15m left */
	{
		testsCount++;
		Glucose_Status glucose_status = Create_GlucoseStatus();
		glucose_status.delta = -2;
		glucose_status.glucose = 121;
		glucose_status.long_avgdelta = -1.333;
		glucose_status.short_avgdelta = -1.333;
		IOB_Data iob_data = Create_IOB_Data();
		iob_data.iob = 3.983;
		iob_data.activity = 0.0255;
		iob_data.bolussnooze = 2.58;
		iob_data.basaliob = 0.384;
		iob_data.netbasalinsulin = 0.3;
		iob_data.hightempinsulin = 0.7;
		Meal_Data meal_data = Create_MealData();
		meal_data.carbs = 65;
		meal_data.boluses = 4;
		meal_data.mealCOB = 65;
		Temp currenttemp = Create_Temp();
		currenttemp.duration = 29;
		currenttemp.rate = 1.3;
		snprintf(currenttemp.temp, sizeof(currenttemp.temp), "absolute");
		Profile profile = Create_Profile();
		profile.max_iob = 3;
		snprintf(profile.type, sizeof(profile.type), "current");
		profile.dia = 3;
		profile.current_basal = 1.3;
		profile.max_daily_basal = 1.3;
		profile.max_basal = 3.5;
		profile.min_bg = 105;
		profile.max_bg = 105;
		profile.sens = 40;
		profile.carb_ratio = 10;
		Temp output = determine_basal(glucose_status, currenttemp, &iob_data, profile, autosens, meal_data, APS_tempBasalFunctions, 1);
		if (isnan(output.rate) && isnan(output.duration))
		{
			printf("\n\nTest 33:\tshould do nothing when requested temp already running with >15m left (%.6lf and %.6lf): SUCCESS\n",
				output.rate, output.duration);
			testsPassed++;
		}
		else
			printf("\n\nTest 33:\tshould do nothing when requested temp already running with >15m left (%.6lf = NAN and %.6lf = NAN): FAILURE\n",
				output.rate, output.duration);
	}
	/*	Test 34	should temp to zero with double sensitivity adjustment */
	{
		testsCount++;
		IOB_Data iob_data = Create_IOB_Data();
		iob_data.iob = 0.5;
		iob_data.activity = 0.001;
		iob_data.bolussnooze = 0.0;
		iob_data.basaliob = 0.5;
		Autosens autosens = Create_Autosens();
		autosens.ratio = 0.5;

		Temp output = determine_basal(glucose_status, currenttemp, &iob_data, profile, autosens, meal_data, APS_tempBasalFunctions, 1);
		if (output.rate == 0 && output.duration == 30)
		{
			printf("\n\nTest 34:\tshould temp to zero with double sensitivity adjustment (%.6lf and %.6lf): SUCCESS\n",
				output.rate, output.duration);
			testsPassed++;
		}
		else
			printf("\n\nTest 34:\tshould temp to zero with double sensitivity adjustment (%.6lf = 0 and %.6lf = 30): FAILURE\n",
				output.rate, output.duration);
	}
	/*	Test 35	maxSafeBasal current_basal_safety_multiplier of 1 should cause the current rate to be set, even if higher is needed */
	{
		testsCount++;
		Glucose_Status glucose_status = Create_GlucoseStatus();
		glucose_status.delta = 5;
		glucose_status.glucose = 185;
		glucose_status.long_avgdelta = 5;
		glucose_status.short_avgdelta = 5;
		IOB_Data iob_data = Create_IOB_Data();
		iob_data.iob = 0;
		iob_data.activity = -0.01;
		iob_data.bolussnooze = 0;

		Profile tempProfile;
		memmove(&tempProfile, &profile, sizeof(Profile));
		tempProfile.current_basal_safety_multiplier = 1;

		Temp output = determine_basal(glucose_status, currenttemp, &iob_data, tempProfile, autosens, meal_data, APS_tempBasalFunctions, 1);
		if (output.rate == 0.9)
		{
			printf("\n\nTest 35:\tmaxSafeBasal current_basal_safety_multiplier of 1 should cause the current rate to be set, even if higher is needed (%.6lf): SUCCESS\n",
				output.rate);
			testsPassed++;
		}
		else
			printf("\n\nTest 35:\tmaxSafeBasal current_basal_safety_multiplier of 1 should cause the current rate to be set, even if higher is needed (%.6lf = 0.9): FAILURE\n",
				output.rate);
	}
	/*	Test 36	maxSafeBasal max_daily_safety_multiplier of 1 should cause the max daily rate to be set, even if higher is needed */
	{
		testsCount++;
		Glucose_Status glucose_status = Create_GlucoseStatus();
		glucose_status.delta = 5;
		glucose_status.glucose = 185;
		glucose_status.long_avgdelta = 5;
		glucose_status.short_avgdelta = 5;
		IOB_Data iob_data = Create_IOB_Data();
		iob_data.iob = 0;
		iob_data.activity = -0.01;
		iob_data.bolussnooze = 0;

		Profile tempProfile;
		memmove(&tempProfile, &profile, sizeof(Profile));
		tempProfile.current_basal_safety_multiplier = NAN;
		tempProfile.max_daily_safety_multiplier = 1;
		Temp output = determine_basal(glucose_status, currenttemp, &iob_data, tempProfile, autosens, meal_data, APS_tempBasalFunctions, 1);
		if (output.rate == 1.3)
		{
			printf("\n\nTest 36:\tmaxSafeBasal max_daily_safety_multiplier of 1 should cause the max daily rate to be set, even if higher is needed (%.6lf): SUCCESS\n",
				output.rate);
			testsPassed++;
		}
		else
			printf("\n\nTest 36:\tmaxSafeBasal current_basal_safety_multiplier of 1 should cause the current rate to be set, even if higher is needed (%.6lf = 1.3): FAILURE\n",
				output.rate);
		printf("Reason: %s\n", output.reason);
	}
	/*	Test 37	overriding maxSafeBasal multipliers to 10 should increase temp */
	{
		testsCount++;
		Glucose_Status glucose_status = Create_GlucoseStatus();
		glucose_status.delta = 5;
		glucose_status.glucose = 285;
		glucose_status.long_avgdelta = 5;
		glucose_status.short_avgdelta = 5;
		IOB_Data iob_data = Create_IOB_Data();
		iob_data.iob = 0;
		iob_data.activity = -0.01;
		iob_data.bolussnooze = 0;

		Profile tempProfile;
		memmove(&tempProfile, &profile, sizeof(Profile));
		tempProfile.max_basal = 5;
		tempProfile.current_basal_safety_multiplier = 10;
		tempProfile.max_daily_safety_multiplier = 10;
		Temp output = determine_basal(glucose_status, currenttemp, &iob_data, tempProfile, autosens, meal_data, APS_tempBasalFunctions, 1);
		if (output.rate == 5)
		{
			printf("\n\nTest 37:\toverriding maxSafeBasal multipliers to 10 should increase temp (%.6lf): SUCCESS\n",
				output.rate);
			testsPassed++;
		}
		else
			printf("\n\nTest 37:\toverriding maxSafeBasal multipliers to 10 should increase temp (%.6lf = 1.3): FAILURE\n",
				output.rate);
		printf("Reason: %s\n", output.reason);
	}
	/*	Test 38	should round appropriately for small basals when setting basal to maxSafeBasal  */
	{
		testsCount++;
		Glucose_Status glucose_status = Create_GlucoseStatus();
		glucose_status.delta = 5;
		glucose_status.glucose = 185;
		glucose_status.long_avgdelta = 5;
		glucose_status.short_avgdelta = 5;
		Profile profile2 = Create_Profile();
		profile2.max_iob = 2.5;
		profile2.dia = 3;
		snprintf(profile2.type, sizeof(profile2.type), "current");
		profile2.current_basal = 0.025;
		profile2.max_daily_basal = 1.3;
		profile2.max_basal = .05;
		profile2.max_bg = 120;
		profile2.min_bg = 110;
		profile2.sens = 200;
		snprintf(profile2.model, sizeof(profile2.model), "523");
		Temp currenttemp = Create_Temp();
		currenttemp.duration = 0;
		currenttemp.rate = 0;
		snprintf(currenttemp.temp, sizeof(currenttemp.temp), "absolute");
		Temp output = determine_basal(glucose_status, currenttemp, &iob_data, profile2, autosens, meal_data, APS_tempBasalFunctions, 1);
		if (output.rate == 0.05 && output.duration == 30)
		{
			printf("\n\nTest 38:\tshould round appropriately for small basals when setting basal to maxSafeBasal  (%.6lf and %.6lf): SUCCESS\n",
				output.rate, output.duration);
			testsPassed++;
		}
		else
			printf("\n\nTest 38:\tshould round appropriately for small basals when setting basal to maxSafeBasal  (%.6lf = 0.05 and %.6lf = 30): FAILURE\n",
				output.rate, output.duration);
		printf("Reason: %s\n", output.reason);
	}
	/*	Test 39	should match the basal rate precision available on a 523  */
	{
		testsCount++;
		Temp currenttemp = Create_Temp();
		currenttemp.duration = 0;
		currenttemp.rate = 0;
		snprintf(currenttemp.temp, sizeof(currenttemp.temp), "absolute");
		Profile tempProfile;
		memmove(&tempProfile, &profile, sizeof(Profile));
		tempProfile.current_basal = .825;
		snprintf(tempProfile.model, sizeof(tempProfile.model), "523");
		
		Temp output = determine_basal(glucose_status, currenttemp, &iob_data, tempProfile, autosens, meal_data, APS_tempBasalFunctions, 1);
		if (output.rate == 0.825 && output.duration == 30)
		{
			printf("\n\nTest 39:\tshould match the basal rate precision available on a 523 (%.6lf and %.6lf): SUCCESS\n",
				output.rate, output.duration);
			testsPassed++;
		}
		else
			printf("\n\nTest 39:\tshould match the basal rate precision available on a 523 (%.6lf = 0.825 and %.6lf = 30): FAILURE\n",
				output.rate, output.duration);
		printf("Reason: %s\n", output.reason);
	}
	/*	Test 40	should match the basal rate precision available on a 522  */
	{
		testsCount++;
		Temp currenttemp = Create_Temp();
		currenttemp.duration = 0;
		currenttemp.rate = 0;
		snprintf(currenttemp.temp, sizeof(currenttemp.temp), "absolute");
		Profile tempProfile;
		memmove(&tempProfile, &profile, sizeof(Profile));
		tempProfile.current_basal = .875;
		snprintf(tempProfile.model, sizeof(tempProfile.model), "522");

		Temp output = determine_basal(glucose_status, currenttemp, &iob_data, tempProfile, autosens, meal_data, APS_tempBasalFunctions, 1);
		if (output.rate == 0.9 && output.duration == 30)
		{
			printf("\n\nTest 40:\tshould match the basal rate precision available on a 522 (%.6lf and %.6lf): SUCCESS\n",
				output.rate, output.duration);
			testsPassed++;
		}
		else
			printf("\n\nTest 40:\tshould match the basal rate precision available on a 522 (%.6lf = 0.9 and %.6lf = 30): FAILURE\n",
				output.rate, output.duration);
		printf("Reason: %s\n", output.reason);
	}
	printf("\n--------------------------------\nSummary for Determine Basal: %d/%d tests passed\n", testsPassed, testsCount);
	return testsPassed == testsCount;
}
int startTest() {
	InitAPS();
	int round_basal_test = test_round_basal();
	int determine_basal_test = test_determine_basal();
	printf("\n--------------------------------\nSummary for all tests:\n");
	
	printf("Round Basal: %s\n", round_basal_test ? "PASSED" : "FAILED");
	printf("Determine Basal: %s\n", determine_basal_test ? "PASSED" : "FAILED");
	return round_basal_test && determine_basal_test;
}
int main() {
<<<<<<< Updated upstream
	return !startTest();	//	in github workflow a value of 1 (which is true in C) is treated as false
=======
	return !startTest();	//	for github workflow, return result is 0 for passing.
>>>>>>> Stashed changes
}