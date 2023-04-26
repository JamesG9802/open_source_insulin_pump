#pragma once
#ifdef __cplusplus
extern "C" {
#endif
/*	The original documentation lists out profiles in .json format	
	Default values based on oref documentation.
*/
typedef struct APS_Profile {
	char out_units[128];
	double max_daily_safety_multiplier;		//	if NAN,		then treat as invalid
	double current_basal_safety_multiplier; //	if NAN,		then treat as invalid
	double max_basal;
	double max_daily_basal;
	double current_basal;					//	if NAN,		then treat as invalid
	char model[512];
	unsigned char skip_neutral_temps;
	double max_iob;
	double min_bg;							//	if NAN,		then treat as invalid
	double max_bg;							//	if NAN,		then treat as invalid
	unsigned char exercise_mode;
	unsigned char high_temptarget_raises_sensitivity;
	unsigned char low_temptarget_lowers_sensitivity;
	double half_basal_exercise_target;
	unsigned char temptargetSet;
	double autosens_max;
	unsigned char sensitivity_raises_target;
	unsigned char resistance_lowers_target;
	double noisyCGMTargetMultiplier;
	double maxRaw;
	double sens;
	double carb_ratio;
	double remainingCarbsCap;				//	invalid if NAN
	double remainingCarbsFraction;			//	invalid if NAN
	double carbsReqThreshold;
	double dia;
	char type[256];
} Profile; 
#ifdef __cplusplus
}
#endif
