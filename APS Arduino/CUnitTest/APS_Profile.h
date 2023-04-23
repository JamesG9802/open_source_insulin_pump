#pragma once
/*	The original documentation lists out profiles in .json format	*/
typedef struct APS_Profile {
	char out_units[128];
	double max_daily_safety_multiplier;		//	if negative, then treat as invalid
	double current_basal_safety_multiplier; //	if negative, then treat as invalid
	double max_basal;
	double max_daily_basal;
	double current_basal;
	char model[512];
	unsigned char skip_neutral_temps;
} Profile;