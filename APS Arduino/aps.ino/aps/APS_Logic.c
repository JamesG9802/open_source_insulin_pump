#include <math.h>

#include "APS_Data.h"
#include "APS_Logic.h"

/// <summary>
/// Updates iob (Insulin on Board) based on CGM data
/// Based on iobCalcBilinear from https://github.com/openaps/oref0/blob/master/lib/iob/calculate.js#enroll-beta
/// </summary>
/// <param name="data"></param>
void UpdateIOB(APS_Data* apsData, CGM_Data cgmData)  {
    if (!apsData) return;  //  Safety NULL check

    double default_dia = 3.0; // assumed duration of insulin activity, in hours
    double peak = 75;        // assumed peak insulin activity, in minutes
    double end = 180;        // assumed end of insulin activity, in minutes

    // Scale minsAgo by the ratio of the default dia / the user's dia 
    // so the calculations for activityContrib and iobContrib work for 
    // other dia values (while using the constants specified above)
    double timeScalar = default_dia / apsData->dia;
    double scaled_minsAgo = timeScalar * apsData->minsAgo;

    double activityContrib = 0;
    double iobContrib = 0;

    // Calc percent of insulin activity at peak, and slopes up to and down from peak
    // Based on area of triangle, because area under the insulin action "curve" must sum to 1
    // (length * height) / 2 = area of triangle (1), therefore height (activityPeak) = 2 / length (which in this case is dia, in minutes)
    // activityPeak scales based on user's dia even though peak and end remain fixed
    double activityPeak = 2 / (apsData->dia * 60);
    double slopeUp = activityPeak / peak;
    double slopeDown = -1 * (activityPeak / (end - peak));

    if (scaled_minsAgo < peak) {

        activityContrib = apsData->treatment_insulin * (slopeUp * scaled_minsAgo);

        double x1 = (scaled_minsAgo / 5) + 1;  // scaled minutes since bolus, pre-peak; divided by 5 to work with coefficients estimated based on 5 minute increments
        iobContrib = apsData->treatment_insulin * ((-0.001852 * x1 * x1) + (0.001852 * x1) + 1.000000);
    }
    else if (scaled_minsAgo < end) {

        double minsPastPeak = scaled_minsAgo - peak;
        activityContrib = apsData->treatment_insulin * (activityPeak + (slopeDown * minsPastPeak));

        double x2 = ((scaled_minsAgo - peak) / 5);  // scaled minutes past peak; divided by 5 to work with coefficients estimated based on 5 minute increments
        iobContrib = apsData->treatment_insulin * ((0.001323 * x2 * x2) + (-0.054233 * x2) + 0.555560);
    }
    apsData->activityContrib = activityContrib;
    apsData->iobContrib = iobContrib;
}

void UpdateData(APS_Data* apsData, CGM_Data cgmData)  {
    if (!apsData) return;
    UpdateIOB(apsData, cgmData);
}

/// <summary>
/// Based on https://openaps.readthedocs.io/en/latest/docs/While%20You%20Wait%20For%20Gear/Understand-determine-basal.html#eventualbg
/// </summary>
/// <param name="data"></param>
void PredictEventualBG(APS_Data* apsData, CGM_Data cgmData) {
    if (!apsData) return;
    //  Based off https://github.com/openaps/oref0/blob/88cf032aa74ff25f69464a7d9cd601ee3940c0b3/lib/determine-basal/determine-basal.js#L416
    double minDelta = apsData->delta < apsData->short_avgdelta ? apsData->delta : apsData->short_avgdelta;
    double minAvgDelta = apsData->short_avgdelta < apsData->long_avgdelta ? apsData->short_avgdelta : apsData->long_avgdelta;

    apsData->BGI = round(-cgmData.activity * apsData->ISF * 5);

    double deviation = round(30 / 5 * (minDelta - apsData->BGI));
    // don't overreact to a big negative delta: use minAvgDelta if deviation is negative
    if (deviation < 0) {
        deviation = round((30 / 5) * (minAvgDelta - apsData->BGI));
        // and if deviation is still negative, use long_avgdelta
        if (deviation < 0) {
            deviation = round((30 / 5) * (apsData->long_avgdelta - apsData->BGI));
        }
    }

    // calculate the naive (bolus calculator math) eventual BG based on net IOB and sensitivity
    double naive_eventualBG = round(cgmData.Current_Glucose - apsData->iob * apsData->ISF);

    // and adjust it for the deviation above
    apsData->Eventual_BG = naive_eventualBG + deviation;
}

void CalculatePrediction(APS_Data* data, CGM_Data cgmData)  {
    if (!data) return;
}

void CalculateInsulinRate(APS_Data* data) {
    if (!data) return;
}