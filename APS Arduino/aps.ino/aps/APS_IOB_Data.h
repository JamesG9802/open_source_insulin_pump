#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include "APS_Temperature.h"

typedef struct APS_IOB_DATA {
	double iob;					//	invalid if NAN
	double activity;			//	invalid if NAN
	unsigned char bolussnooze;
	Temp lastTemp;
	struct APS_IOB_DATA* iobWithZeroTemp;
	long long lastBolusTime;	//	milliseconds since unix epoch
} IOB_Data;
#ifdef __cplusplus
}
#endif