class BonfireLight extends PointLightBase
{
	static float m_FireRadius = 40;
	static float m_FireBrightness = 15.0;
	static string m_MemoryPoint = "bonfire_fire";
	
	void BonfireLight()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(m_FireRadius);
		SetBrightnessTo(m_FireBrightness);
		SetCastShadow(false);
		SetFadeOutTime(1);
		SetDiffuseColor(1.0, 0.45, 0.25);
		SetAmbientColor(1.0, 0.45, 0.25);
		SetFlareVisible(false);
		SetFlickerAmplitude(0.55);
		SetFlickerSpeed(0.75);
		SetDancingShadowsMovementSpeed(0.1);
		SetDancingShadowsAmplitude(0.03);
	}
	
	override void OnFrameLightSource(IEntity other, float timeSlice)
	{
		
	}
}