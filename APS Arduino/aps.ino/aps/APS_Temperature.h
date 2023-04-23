#pragma once
typedef struct APS_Temperature {
	double duration;	//	invalid if NAN
	double rate;		//	invalid if NAN
	const char temp[128];
	const char reason[4096];
} Temp;