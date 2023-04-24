#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include "APS_Predictions.h"
typedef struct APS_Temperature {
	double duration;	//	invalid if NAN
	double rate;		//	invalid if NAN
	const char temp[128];
	const char reason[4096];
	const char error[4096];
	long long deliverAt;	//	milliseconds since unix epoch
	long long date;			//	milliseconds since unix epoch
	double bg;
	double tick;
	double eventualBG;
	double insulinReq;
	double sensitivityRatio;
	Predictions predBGs;
	double COB;
	double IOB;
	double BGI;
	double deviation;
	double ISF;
	double CR;
	double target_bg;
	double carbsReq;
} Temp;
#ifdef __cplusplus
}
#endif