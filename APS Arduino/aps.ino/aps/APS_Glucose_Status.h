#pragma once
#ifdef __cplusplus
extern "C" {
#endif
typedef struct APS_Glucose_Status {
	double delta;
	double glucose;
	double long_avgdelta;
	double short_avgdelta;
	long long date;				//	time since unix epoch
	double noise;
	const char device[512];
	double last_cal;			//	invalid if NAN
} Glucose_Status;
#ifdef __cplusplus
}
#endif