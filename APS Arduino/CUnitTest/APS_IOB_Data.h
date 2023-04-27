#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "APS_Temperature.h"

#ifndef __cplusplus
typedef struct APS_IOB_DATA {
#endif
#ifdef __cplusplus
struct IOB_Data {
#endif
	double iob;					//	invalid if NAN
	double activity;			//	invalid if NAN
	double bolussnooze;
	Temp lastTemp;
	struct APS_IOB_DATA* iobWithZeroTemp;
	long long lastBolusTime;	//	milliseconds since unix epoch
	double basaliob;
	double netbasalinsulin;
	double hightempinsulin;
  #ifdef __cplusplus
  IOB_Data() : IOB_Data(0, 0, 0, Temp(), NULL, 0, 0, 0, 0) {}
  IOB_Data(double a, double b, double c, Temp d, struct APS_IOB_DATA* e, long long f, double g, double h, double i) : 
  iob(a), activity(b), bolussnooze(c), lastTemp(d), iobWithZeroTemp(e), lastBolusTime(f), basaliob(g), netbasalinsulin(h), hightempinsulin(i)
  {}

  IOB_Data& operator=(const IOB_Data& other) { return *this; }
  #endif
}
#ifndef __cplusplus
IOB_Data
#endif
;


#ifdef __cplusplus
}
#endif