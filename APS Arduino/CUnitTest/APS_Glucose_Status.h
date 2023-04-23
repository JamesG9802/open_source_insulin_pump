#pragma once
typedef struct APS_Glucose_Status {
	double delta;
	double glucose;
	double long_avgdelta;
	double short_avgdelta;
} Glucose_Status;
