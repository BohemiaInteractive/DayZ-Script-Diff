class PPERequester_HeavyMetalPoisoning_3 extends PPERequester_GameplayBase
{
	const float WAVE_PROGRESS_MIN = 0;
	const float WAVE_PROGRESS_MAX = Math.PI;
	const float WAVE_AMPLITUDE_COEF = 0.7; //utilized values clamped to <-1,1> range, this affects the duration of plateau and steepness of change
	const float WAVE_TILT = -0.12; //anything in the closed range of (1,-1), left to right. UNDEFINED for '0' value!
	
	const float DISTORT_MIN = 0;
	const float DISTORT_MAX = -0.8;
	
	//const float PULSING_FREQUENCY = 1.0;
	
	float m_ProgressAngle;
	float m_CurveValue;
	float m_CurveValueClamped;
	float m_LenseOffsetAngle;
	float m_LenseOffsetDist;
	
	void SetInitialLenseOffset(float offsetAngle, float offsetDistance = 0.5)
	{
		m_LenseOffsetAngle = offsetAngle;
		m_LenseOffsetDist = Math.Clamp(offsetDistance,0,1);
	}
	
	void SetEffectProgress(float progress) //0..1
	{
		m_ProgressAngle = Math.Lerp(WAVE_PROGRESS_MIN,WAVE_PROGRESS_MAX,progress);
		m_CurveValue = (1 / WAVE_TILT) * (Math.Atan((WAVE_TILT * Math.Sin(m_ProgressAngle)) / (1 - WAVE_TILT * Math.Cos(m_ProgressAngle)))) * WAVE_AMPLITUDE_COEF;
		m_CurveValueClamped = Math.Clamp(m_CurveValue,-1,1);
		
		float distort = Math.Lerp(DISTORT_MIN,DISTORT_MAX,m_CurveValueClamped);
		float centerX = Math.Lerp(0,m_LenseOffsetDist,Math.Sin(m_ProgressAngle + m_LenseOffsetAngle));
		float centerY = Math.Lerp(0,m_LenseOffsetDist,Math.Cos(m_ProgressAngle + m_LenseOffsetAngle));
		float chromAbb = Math.AbsFloat(m_CurveValueClamped); //scale
		
		float chromShiftR = 0.1;
		float chromShiftG = 0;
		float chromShiftB = 0.75;
		
		float alpha01 =  Math.Sin( -Math.PI_HALF +  7 *  m_ProgressAngle) * 0.5 + 0.5;
		vector colabA = "-1 -1 -0.85";
		vector colabB = "0 0 0";	
		vector absim = vector.Lerp(colabA, colabB, alpha01);
		
		array<float> color_mod = {absim[0], absim[1], absim[2], 1.0};
		
		vector coloverA = "0 0 0";
		vector coloverB = "0.25 0.9 0.1";	
		vector oversim = vector.Lerp(coloverA, coloverB, m_CurveValueClamped);
		
		array<float> color_overlay = {oversim[0], oversim[1], oversim[2], 0.5};
		
		SetTargetValueFloat(PostProcessEffectType.Distort,PPEDistort.PARAM_DISTORT,false, distort,PPEDistort.L_0_HMP,PPOperators.ADD);
		SetTargetValueFloat(PostProcessEffectType.Distort,PPEDistort.PARAM_CENTERX,false, centerX,PPEDistort.L_1_HMP,PPOperators.ADD);
		SetTargetValueFloat(PostProcessEffectType.Distort,PPEDistort.PARAM_CENTERY,false, centerY,PPEDistort.L_2_HMP,PPOperators.ADD);
		SetTargetValueFloat(PostProcessEffectType.Distort,PPEDistort.PARAM_CHROM_ABB,true, chromAbb,PPEDistort.L_3_HMP,PPOperators.ADD);
		SetTargetValueFloat(PostProcessEffectType.Distort,PPEDistort.PARAM_CHROM_ABB_SHIFTR,true, chromShiftR,PPEDistort.L_4_HMP,PPOperators.SET);
		SetTargetValueFloat(PostProcessEffectType.Distort,PPEDistort.PARAM_CHROM_ABB_SHIFTG,true, chromShiftG,PPEDistort.L_5_HMP,PPOperators.SET);
		SetTargetValueFloat(PostProcessEffectType.Distort,PPEDistort.PARAM_CHROM_ABB_SHIFTB,true, chromShiftB,PPEDistort.L_6_HMP,PPOperators.SET);
		SetTargetValueColor(PostProcessEffectType.Distort,PPEDistort.PARAM_CHROM_COLOR_MOD,color_mod,PPEDistort.L_7_HMP,PPOperators.ADD);
		
		SetTargetValueColor(PostProcessEffectType.Glow,PPEGlow.PARAM_COLORIZATIONCOLOR,color_overlay,PPEGlow.L_23_HMP,PPOperators.ADD);
	}
}