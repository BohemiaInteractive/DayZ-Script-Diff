class PPERequester_SandstormEffect: PPERequester_GameplayBase
{
    protected const float SANDSTORM_THRESHOLD_MIN = 0.20;
    protected const float SANDSTORM_THRESHOLD_MAX = 1.0;
    protected const float SANDSTORM_INTENSITY_MIN = 0;
    protected const float SANDSTORM_INTENSITY_MAX = 1.0;
    protected const float SANDSTORM_INTENSITY_BLEND_POWER = 0.25;

    protected static const float EFFECT_TRANSITION_SPEED = 1.0;
    protected static const float EFFECT_STEP_DELTA = 0.016;
    protected const float EFFECT_STEP = EFFECT_STEP_DELTA * EFFECT_TRANSITION_SPEED;

    //! GLOW COLOR
    protected const vector SANDSTORM_COLOR_LIGHT = "1.0 0.80 0.5";
    protected const vector SANDSTORM_COLOR_HEAVY = "1.0 0.85 0.6";
    protected const float MIN_SANDSTORM_COLOR_ALPHA = 0.2;
    protected const float MAX_SANDSTORM_COLOR_ALPHA = 0.5;
	
	//! GLOW SATURATION
	protected const float SANDSTORM_SATURATION_MIN = 1.0;
	protected const float SANDSTORM_SATURATION_MAX = 0.78;
	
	//! GLOW CONTRAST
	protected const float SANDSTORM_CONTRAST_MIN = 1.0;
	protected const float SANDSTORM_CONTRAST_MAX = 0.92;
	
	//! GLOW BRIGHTNESS
	protected const float SANDSTORM_BRIGHTNESS_MIN = 1.0;
	protected const float SANDSTORM_BRIGHTNESS_MAX = 0.97;
	
	//! GODRAYS
	protected const float SANDSTORM_GODRAYS_MIN = 0;
	protected const float SANDSTORM_GODRAYS_MAX = 0.10;
	
	protected const float SANDSTORM_SUN_VISIBILITY_MIN = 0.5;
	protected const float SANDSTORM_SUN_VISIBILITY_MAX = 0.35;
	
	protected const float SANDSTORM_GODRAYS_OVERBURN_MIN = 0;
	protected const float SANDSTORM_GODRAYS_OVERBURN_MAX = 0.05;
	
	//! Utility
    protected float m_CurrentIntensity = 0.0;
    protected float m_TargetIntensity = 0.0;

    protected float m_EffectPhase = 0.0;
    protected float m_WindFrequency = 0.8;

    protected vector m_SandstormColorLightHSV;
    protected vector m_SandstormColorHeavyHSV;

	protected float m_SandstormBlend;
	
	protected World m_World;
	
    override protected void OnStart(Param par = null)
    {
        super.OnStart(par);

		m_World = g_Game.GetWorld();
        m_CurrentIntensity = 0.0;
        m_TargetIntensity = 0.0;

        m_SandstormColorLightHSV = RGBToHSV(SANDSTORM_COLOR_LIGHT);
        m_SandstormColorHeavyHSV = RGBToHSV(SANDSTORM_COLOR_HEAVY);

        g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(UpdateEffect, 16, true);
    }

    override protected void OnStop(Param par = null)
    {
        super.OnStop(par);

        g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(UpdateEffect);

        m_TargetIntensity = 0.0;
        m_CurrentIntensity = 0.0;
    }

    void SetTargetIntensity(float intensity)
    {
        m_TargetIntensity = Math.Clamp(intensity, SANDSTORM_INTENSITY_MIN, SANDSTORM_INTENSITY_MAX);
    }

    protected void UpdateEffect()
    {
        if (ShouldUpdateEffect())
        {
            if (m_CurrentIntensity < m_TargetIntensity)
			{
                m_CurrentIntensity = Math.Min(m_CurrentIntensity + EFFECT_STEP, m_TargetIntensity);
			}
            else
			{
                m_CurrentIntensity = Math.Max(m_CurrentIntensity - EFFECT_STEP, m_TargetIntensity);
			}

            ProcessEffect(m_CurrentIntensity);
        }
    }
	
	protected bool ShouldUpdateEffect()
	{
		return (m_CurrentIntensity != m_TargetIntensity);
	}

    protected float GetSandstormBlend(float intensity)
    {
        if (intensity <= SANDSTORM_THRESHOLD_MIN)
            return SANDSTORM_INTENSITY_MIN;

        float t = Math.InverseLerp(SANDSTORM_THRESHOLD_MIN, SANDSTORM_THRESHOLD_MAX, intensity);
        t = Math.Clamp(t, SANDSTORM_INTENSITY_MIN, SANDSTORM_INTENSITY_MAX);
        return Math.Pow(t, SANDSTORM_INTENSITY_BLEND_POWER);
    }

    protected void ProcessEffect(float intensity)
    {
		m_SandstormBlend = GetSandstormBlend(intensity);

		vector currentHSV;
		currentHSV[0] = Math.Lerp(m_SandstormColorLightHSV[0], m_SandstormColorHeavyHSV[0], m_SandstormBlend);
		currentHSV[1] = Math.Lerp(m_SandstormColorLightHSV[1], m_SandstormColorHeavyHSV[1], m_SandstormBlend);
		currentHSV[2] = Math.Lerp(m_SandstormColorLightHSV[2], m_SandstormColorHeavyHSV[2], m_SandstormBlend);
		currentHSV[1] = Math.Min(1.0, currentHSV[1] * (1.0 + m_SandstormBlend * 0.3));
		
		vector currentColor = HSVToRGB(currentHSV);
		float colorPower = Math.Pow(m_SandstormBlend, 1.3);
		float red = Math.Lerp(0.0, currentColor[0] - 1.0, colorPower);
		float green = Math.Lerp(0.0, currentColor[1] - 1.0, colorPower);
		float blue = Math.Lerp(0.0, currentColor[2] - 1.0, colorPower);
		float alpha = Math.Lerp(MIN_SANDSTORM_COLOR_ALPHA, MAX_SANDSTORM_COLOR_ALPHA, colorPower);
		
		SetTargetValueColor(PostProcessEffectType.Glow, PPEGlow.PARAM_COLORIZATIONCOLOR, {red, green, blue, alpha}, PPEGlow.L_20_SANDSTORM, PPOperators.ADD);
		SetTargetValueFloat(PostProcessEffectType.Glow, PPEGlow.PARAM_SATURATION, false, Math.Lerp(SANDSTORM_SATURATION_MIN, SANDSTORM_SATURATION_MAX, m_SandstormBlend), PPEGlow.L_20_SANDSTORM, PPOperators.SET);
		SetTargetValueFloat(PostProcessEffectType.Glow, PPEGlow.PARAM_CONTRAST, false, Math.Lerp(SANDSTORM_CONTRAST_MIN, SANDSTORM_CONTRAST_MAX, m_SandstormBlend), PPEGlow.L_20_SANDSTORM, PPOperators.SET);
		SetTargetValueFloat(PostProcessEffectType.Glow, PPEGlow.PARAM_BRIGHTNESS, false, Math.Lerp(SANDSTORM_BRIGHTNESS_MIN, SANDSTORM_BRIGHTNESS_MAX, m_SandstormBlend), PPEGlow.L_20_SANDSTORM, PPOperators.SET);
		
		SetTargetValueFloat(PostProcessEffectType.GodRays, PPEGodRays.PARAM_INTENSITY, Math.Lerp(SANDSTORM_GODRAYS_MIN, SANDSTORM_GODRAYS_MAX, m_SandstormBlend * m_SandstormBlend), PPEGodRays.L_0_SANDSTORM, PPOperators.LOWEST);
		SetTargetValueFloat(PostProcessEffectType.GodRays, PPEGodRays.PARAM_SUNVISIBLE,  Math.Lerp(SANDSTORM_SUN_VISIBILITY_MIN, SANDSTORM_SUN_VISIBILITY_MAX, m_SandstormBlend), PPEGodRays.L_0_SANDSTORM, PPOperators.SET);
		SetTargetValueFloat(PostProcessEffectType.GodRays, PPEGodRays.PARAM_OVERBURNINTENSITY, Math.Lerp(SANDSTORM_GODRAYS_OVERBURN_MIN, SANDSTORM_GODRAYS_OVERBURN_MAX, m_SandstormBlend * m_SandstormBlend), PPEGodRays.L_0_SANDSTORM, PPOperators.SET);
    }
}