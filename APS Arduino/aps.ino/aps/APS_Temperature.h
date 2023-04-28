#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include "APS_Predictions.h"
#include "APSList.h"
#ifndef __cplusplus
typedef struct APS_Temperature {
#endif
#ifdef __cplusplus
struct Temp {
#endif
	double duration;	//	invalid if NAN
	double rate;		//	invalid if NAN
	const char temp[128];
	const char reason[512];
	const char error[256];
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
  #ifdef __cplusplus
  Temp() : Temp(0, 0, "", "", "", 0, 0, 0, 0, 0, 0, 0, Predictions(), 0, 0, 0, 0, 0, 0, 0) {}
  Temp(double a, double b, const char c[128], const char d[4096], const char e[4096], long long f, long long g, double h, double i,
  double j, double k, double l, Predictions m, double n, double o, double p, double q, double r, double s, double t) :
    duration(a), rate(b), temp(""), reason(""), error(""), deliverAt(f), date(g), bg(h), tick(i), eventualBG(j), insulinReq(k), sensitivityRatio(l), predBGs(m), COB(n), IOB(o), BGI(p), deviation(q), ISF(r), CR(s), target_bg(t), carbsReq(0)
    {}

  Temp& operator=(const Temp& other) { return *this; }
  #endif
}
#ifndef __cplusplus
Temp
#endif
;

/// <summary>
/// To prevent a memory leak, you must call this on all Temp objects when finished. 
/// Does not free temp as you can call this function on non heap allocated Temps.
/// </summary>
/// <param name="temp"></param>
void Destroy_Temp(Temp* temp);
#ifdef __cplusplus
}
#endif