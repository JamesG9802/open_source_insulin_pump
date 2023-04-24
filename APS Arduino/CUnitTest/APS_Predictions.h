#pragma once

#include "APSList.h"
typedef struct APS_Predictions {
	List* IOB;
	List* COB;
} Predictions;