#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include "APS_Profile.h"
/*	Does this really need to be a separate class?	*/
double round_basal(double basal, Profile profile);
#ifdef __cplusplus
}
#endif