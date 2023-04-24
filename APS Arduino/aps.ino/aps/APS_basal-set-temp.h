#pragma once
#ifdef __cplusplus
extern "C" {
#endif
/*	Implementation of https://github.com/openaps/oref0/blob/88cf032aa74ff25f69464a7d9cd601ee3940c0b3/lib/basal-set-temp.js	
	Since the algorithm was originally developed in Javascript, they take advantage of anonymous classes. Meanwhile in C, we 
	have to get away with workarounds.
*/
#include "APS_Profile.h"
#include "APS_Temperature.h"

typedef struct APS_TempBasalFunctions {
	double (*getMaxSafeBasal)(Profile profile);
	Temp (*setTempBasal)(double rate, double duration, Profile profile, Temp rT, Temp currenttemp);
} TempBasalFunctions;

extern TempBasalFunctions APS_tempBasalFunctions;

/*	Initializer to set function pointers	*/
void APS_TempBasalFunctions_Init();
#ifdef __cplusplus
}
#endif