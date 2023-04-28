#include "APS_Glucose_Status.h"
#include "APS_IOB_Data.h"
#include "APS_Profile.h"
#include "APS_Temperature.h"
#include "APS_Autosens.h"
#include "APS_Meal_Data.h"



void TestCase0(Glucose_Status* glucose_status, IOB_Data* iob_data, Temp* currenttemp) {
	currenttemp->duration = 30;
	currenttemp->rate = 1.5;
}
void TestCase1(Glucose_Status* glucose_status, IOB_Data* iob_data, Temp* currenttemp) {
	glucose_status->delta = -5;
	glucose_status->glucose = 75;
	glucose_status->long_avgdelta = -5;
	glucose_status->short_avgdelta = -5;
}
void TestCase2(Glucose_Status* glucose_status, IOB_Data* iob_data, Temp* currenttemp) {
	currenttemp->duration = 57;
	currenttemp->rate = 0;

	glucose_status->delta = -5;
	glucose_status->glucose = 75;
	glucose_status->long_avgdelta = -5;
	glucose_status->short_avgdelta = -5;
}
void TestCase3(Glucose_Status* glucose_status, IOB_Data* iob_data, Temp* currenttemp) {
	currenttemp->duration = 57;
	currenttemp->rate = 0;
	glucose_status->delta = 6;
	glucose_status->glucose = 75;
	glucose_status->long_avgdelta = 6;
	glucose_status->short_avgdelta = 6;
}
void TestCase4(Glucose_Status* glucose_status, IOB_Data* iob_data, Temp* currenttemp) {

	glucose_status->delta = 6;
	glucose_status->glucose = 75;
	glucose_status->long_avgdelta = 6;
	glucose_status->short_avgdelta = 6;
}
void TestCase5(Glucose_Status* glucose_status, IOB_Data* iob_data, Temp* currenttemp) {

	glucose_status->delta = 1;
	glucose_status->glucose = 75;
	glucose_status->long_avgdelta = 1;
	glucose_status->short_avgdelta = 1;
	iob_data->iob = -.05;
	iob_data->activity = -.01;
	iob_data->bolussnooze = 0;
}
void TestCase6(Glucose_Status* glucose_status, IOB_Data* iob_data, Temp* currenttemp) {

	glucose_status->delta = -1;
	glucose_status->glucose = 75;
	glucose_status->long_avgdelta = -1;
	glucose_status->short_avgdelta = -1;
	iob_data->iob = 1;
	iob_data->activity = .01;
	iob_data->bolussnooze = 0.5;
}
void TestCase7(Glucose_Status* glucose_status, IOB_Data* iob_data, Temp* currenttemp) {

	glucose_status->delta = 5;
	glucose_status->glucose = 85;
	glucose_status->long_avgdelta = 5;
	glucose_status->short_avgdelta = 5;
	iob_data->iob = -1;
	iob_data->activity = -.01;
	iob_data->bolussnooze = 0;
}
void TestCase8(Glucose_Status* glucose_status, IOB_Data* iob_data, Temp* currenttemp) {

	glucose_status->delta = 5;
	glucose_status->glucose = 185;
	glucose_status->long_avgdelta = 5;
	glucose_status->short_avgdelta = 5;
	iob_data->iob = 0;
	iob_data->activity = -.01;
	iob_data->bolussnooze = 0;
}
void TestCase9(Glucose_Status* glucose_status, IOB_Data* iob_data, Temp* currenttemp) {

	glucose_status->delta = 5;
	glucose_status->glucose = 145;
	glucose_status->long_avgdelta = 5;
	glucose_status->short_avgdelta = 5;
	currenttemp->duration = 180;
	currenttemp->rate = 1.9;
	iob_data->iob = 0;
	iob_data->activity = -.01;
	iob_data->bolussnooze = 0;
}
void TestCase10(Glucose_Status* glucose_status, IOB_Data* iob_data, Temp* currenttemp) {

	glucose_status->delta = 5;
	glucose_status->glucose = 145;
	glucose_status->long_avgdelta = 5;
	glucose_status->short_avgdelta = 5;
	currenttemp->duration = 30;
	currenttemp->rate = 3.5;
	iob_data->iob = 0;
	iob_data->activity = -.01;
	iob_data->bolussnooze = 0;
}

void LoadTestCase(Glucose_Status* g, IOB_Data* i, Temp* t, int testCaseNumber) {
	switch (testCaseNumber)
	{
	default:
	case 0:
		TestCase0(g, i, t);
		break;
	case 1:
		TestCase1(g, i, t);
		break;
	case 2:
		TestCase2(g, i, t);
		break;
	case 3:
		TestCase3(g, i, t);
		break;
	case 4:
		TestCase4(g, i, t);
		break;
	case 5:
		TestCase5(g, i, t);
		break;
	case 6:
		TestCase6(g, i, t);
		break;
	case 7:
		TestCase7(g, i, t);
		break;
	case 8:
		TestCase8(g, i, t);
		break;
	case 9:
		TestCase9(g, i, t);
		break;
	case 10:
		TestCase10(g, i, t);
		break;
	}
}