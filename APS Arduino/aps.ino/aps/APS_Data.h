
/// <summary>
/// A data format for storing relevant APS data from the OpenAPS documentation.
/// https://openaps.readthedocs.io/en/latest/docs/While%20You%20Wait%20For%20Gear/Understand-determine-basal.html#openaps-algorithm-examples
/// </summary>
typedef struct APS_Data {
	/*	Read the documentation for more information on each of the different variable types	*/


	/*	Blood Glucose Information	*/
	double delta;			//	change in Blood Glucose
	double glucose;			//	most recent Blood Glucose value
	double short_avgdelta;	//	average rate of change in Blood Glucose in range [2.5, 17.5] min
	double long_avgdelta;	//	average rate of change in Blood GLucose in range [17.5, 42.5] min

	/*	Insulin Amount Information	*/
	double iob;				//	units of insulin; net insulin rate change
	double basaliob;		//	units of net basal insulin on board
	double bolusiob;		//	units of bolus insulin on board

	/*	Prediction/Statistic Information	*/
	double deviation;		//	how much actual Blood Glucose change is deviating from BGI
	double BGI;				//	Blood Glucose Impact; how much BG should be changing based on just insulin activity
	double ISF;				//	insulin sensitivity factor; tuned to the value of the pump
	double CR;				//	Carb Ratio; anchored to value from pump
	double Eventual_BG;		//	what Blood Glucose should be at end of Duration of Insulin Activity
	double minGuardBG;		//	prediction based on the lowest your BG should be
	double IOBpredBG;		//	prediction based on IOB only
	double UAMpredBG;		//	prediction based on	deviations
	double Sensitivity_Ratio;	//	how sensitive you are to basal and ISF
	double Target;			//	your insulin pump target
	double Carb_Impact;		//	estimated impact of carbohydrates
	double Safety_Threshold;	//	min_bg - 0.5*(min_bg-40)

	/*	User Input	*/
	double predCI;			//	how much carbohydrates the user will consume
	double min_bg;			//	blood glucose target
} APS_Data;