class ShockDealtEffect
{
	const float INTENSITY_COEF_OVERALL = 0.8; //intensity percentage multiplier
	const float INTENSITY_COEF_BLUR = 1;
	const float INTENSITY_COEF_COLOR = 0.2;
	const float INTENSITY_COEF_VIGNETTE = 0.7;
	const float DURATION_MIN = 0.6;
	
	ref Param3<float,float,float> m_EffectParam;
	
	float m_HitDuration;
	float m_BreakPoint;
	float m_TimeActive;
	float m_ShockIntensityMax;
	
	void ShockDealtEffect(float intensity_max)
	{
		float duration_coef = Math.Clamp(intensity_max,DURATION_MIN,1);
		m_HitDuration = 1 * duration_coef;
		m_BreakPoint = 0.2 * duration_coef;
		m_ShockIntensityMax = Math.Clamp(intensity_max,0,1);
		
		m_EffectParam = new Param3<float,float,float>(0,0,0);
		//Print(intensity_max);
		//Print("HitSyncDebug | ShockDealtEffect: " + GetGame().GetPlayer().GetSimulationTimeStamp());
	}
	
	void ~ShockDealtEffect()
	{
		if (GetGame())
			PPERequesterBank.GetRequester(PPERequester_ShockHitReaction).Stop();
	}
	
	void Update(float deltatime)
	{
		float value;
		
		if ( m_TimeActive <= m_BreakPoint )
		{
			value = Math.InverseLerp(0, m_BreakPoint, m_TimeActive);
		}
		else
		{
			float tmp_value = Math.InverseLerp(m_BreakPoint, m_HitDuration, m_TimeActive);
			value = 1 - tmp_value;
		}
		
		m_TimeActive += deltatime;
		value = Math.Clamp(value,0,1);
		
		//value calculations
		float val = Math.Clamp(Math.Lerp(0, m_ShockIntensityMax, value),0,m_ShockIntensityMax);
		float val_color = Math.Clamp(val * INTENSITY_COEF_COLOR,0,m_ShockIntensityMax);
		
		//Postprocess application
		float blur = val * INTENSITY_COEF_OVERALL * INTENSITY_COEF_BLUR;
		float factor = val_color * INTENSITY_COEF_OVERALL;
		float vignette = val * INTENSITY_COEF_OVERALL * INTENSITY_COEF_VIGNETTE;
		
		m_EffectParam.param1 = blur;
		m_EffectParam.param2 = vignette;
		m_EffectParam.param3 = factor;
		PPERequesterBank.GetRequester(PPERequester_ShockHitReaction).Start(m_EffectParam);
		
		if ( m_TimeActive >= m_HitDuration )
		{
			delete this;
		}
	}
}