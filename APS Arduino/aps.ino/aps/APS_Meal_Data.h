#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#ifndef __cplusplus
typedef struct APS_Meal_Data {
#endif
#ifdef __cplusplus
struct Meal_Data {
#endif
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
  #ifdef __cplusplus
  Meal_Data() : Meal_Data(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, 0, 0, "", 0) {}
  Meal_Data(double a, double b, double c, double d, double e, double f, double g, double h, double i, double j,
  double* k, unsigned char l, long long m, const char* n, double o) : carbs(a),
      nsCarbs(b), bwCarbs(c), journalCarbs(d), mealCOB(e), currentDeviation(f), maxDeviation(g), minDeviation(h), slopeFromMaxDeviation(i),
      slopeFromMinDeviation(j), allDeviations(k), bwFound(l), lastCarbTime(m), reason(""), boluses(o)
  {}
  Meal_Data& operator=(const Meal_Data& other) { return *this; }
  #endif
}
#ifndef __cplusplus
Meal_Data
#endif
;


#ifdef __cplusplus
}
#endif