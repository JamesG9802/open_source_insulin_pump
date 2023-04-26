#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#ifndef __cplusplus
typedef struct APS_Glucose_Status {
#endif
#ifdef __cplusplus
struct Glucose_Status {
#endif
	double delta;
	double glucose;
	double long_avgdelta;
	double short_avgdelta;
	long long date;				//	time since unix epoch
	double noise;
	const char device[512];
	double last_cal;			//	invalid if NAN
  #ifdef __cplusplus
  Glucose_Status() : Glucose_Status(0, 0, 0, 0, 0, 0, 0, "", 0) {}
  Glucose_Status(double a, double b, double c, double d, double e, long long f, double g, const char* h, double i) 
  : delta(a), glucose(b), long_avgdelta(c), short_avgdelta(d), device(""), date(f), noise(g), last_cal(i) {}
  Glucose_Status& operator=(const Glucose_Status& other) { return *this; }
  #endif
}
#ifndef __cplusplus
Glucose_Status
#endif
;
#ifdef __cplusplus
}
#endif