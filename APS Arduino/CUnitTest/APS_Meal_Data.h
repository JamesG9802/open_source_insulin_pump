#pragma once
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
} Meal_Data;