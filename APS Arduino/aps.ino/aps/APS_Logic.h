#ifdef __cplusplus
extern "C" {
#endif

/// <summary>
/// Updates APS_Data with data from CGM.
/// </summary>
void UpdateData(APS_Data* apsData, CGM_Data cgmData);

/// <summary>
/// Using updated APS_Data, update predictions for BG.
/// </summary>
/// <param name="data"></param>
void CalculatePrediction(APS_Data* data, CGM_Data cgmData);

/// <summary>
/// Using updated APS_Data, calculate the new target insulin rate.
/// </summary>
/// <param name="data"></param>
void CalculateInsulinRate(APS_Data* data);

#ifdef __cplusplus
}
#endif