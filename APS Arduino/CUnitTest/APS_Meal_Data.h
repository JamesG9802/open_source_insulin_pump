#pragma once
#ifdef __cplusplus
extern "C" {
#endif
typedef struct APS_Meal_Data {
	double carbs;
	double nsCarbs;
	double bwCarbs;
	double journalCarbs;
	double mealCOB;
	double currentDeviation;
	double maxDeviation;
	double minDeviation;
	double slopeFromMaxDeviation;
	double slopeFromMinDeviation;
	double* allDeviations;	//	array of deviations
	unsigned char bwFound;
	long long lastCarbTime;	//	milliseconds since unix epoch
	const char reason[4096];
	double boluses;
} Meal_Data;
#ifdef __cplusplus
}
#endif