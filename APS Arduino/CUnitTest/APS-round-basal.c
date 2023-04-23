#include <string.h>
#include <math.h>

#include "APS_Profile.h"
#include "APS_round-basal.h"

/*  https://stackoverflow.com/a/744822  */
int endsWith(const char* str, const char* suffix)
{
    if (!str || !suffix)
        return 0;
    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);
    if (lensuffix > lenstr)
        return 0;
    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

double round_basal(double basal, Profile profile) {

    /* x23 and x54 pumps change basal increment depending on how much basal is being delivered:
            0.025u for 0.025 < x < 0.975
            0.05u for 1 < x < 9.95
            0.1u for 10 < x
      To round numbers nicely for the pump, use a scale factor of (1 / increment). */

    double lowest_rate_scale = 20;

    //  NJIT - there is originally a check to ensure profile exists, which is more of a concern in JS compared to C.
    // Make sure optional model has been set
    if (profile.model)
    {
        if (endsWith(profile.model, "54") || endsWith(profile.model, "23"))
        {
            lowest_rate_scale = 40;
        }
    }

    double rounded_basal = basal;
    // Shouldn't need to check against 0 as pumps can't deliver negative basal anyway?
    if (basal < 1)
    {
        rounded_basal = round(basal * lowest_rate_scale) / lowest_rate_scale;
    }
    else if (basal < 10)
    {
        rounded_basal = round(basal * 20) / 20;
    }
    else
    {
        rounded_basal = round(basal * 10) / 10;
    }

    return rounded_basal;
}