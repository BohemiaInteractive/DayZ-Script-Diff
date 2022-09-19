class ChemlightLight extends PointLightBase
{
	private static float m_DefaultBrightness = 1.5;
	private static float m_DefaultRadius = 7.5;
	
	void ChemlightLight()
	{
		SetVisibleDuringDaylight(false);
		SetRadiusTo(m_DefaultRadius);
		SetBrightnessTo(m_DefaultBrightness);
		FadeIn(1);
		SetFadeOutTime(2);
		SetFlareVisible(false);
	}
	
	void SetIntensity( float coef, float time )
	{
		FadeBrightnessTo(m_DefaultBrightness * coef, time);
		FadeRadiusTo(m_DefaultRadius * coef, time);
	}
	
	void SetColorToWhite()
	{
		SetAmbientColor(0.9, 0.9, 1.0);
		SetDiffuseColor(0.9, 0.9, 1.0);
	}
	
	void SetColorToRed()
	{
		SetAmbientColor(1.0, 0.3, 0.2);
		SetDiffuseColor(1.0, 0.3, 0.2);
	}
	
	void SetColorToGreen()
	{
		SetAmbientColor(0.3, 1.0, 0.2);
		SetDiffuseColor(0.3, 1.0, 0.2);
	}
	
	void SetColorToBlue()
	{
		SetAmbientColor(0.3, 0.3, 1.0);
		SetDiffuseColor(0.3, 0.3, 1.0);
	}
	
	void SetColorToYellow()
	{
		SetAmbientColor(1.0, 1.0, 0.3);
		SetDiffuseColor(1.0, 1.0, 0.3);
	}
}