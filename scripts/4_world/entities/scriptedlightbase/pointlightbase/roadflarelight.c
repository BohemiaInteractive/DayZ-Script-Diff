class RoadflareLight extends PointLightBase
{
	static float 	m_RoadflareRadius = 30;
	static float 	m_RoadflareBrightness = 8.5;
	static string 	m_MemoryPoint = "light";
	
	void RoadflareLight()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo( m_RoadflareRadius );
		SetBrightnessTo(m_RoadflareBrightness);
		SetCastShadow(true);
		FadeIn(1);
		SetFadeOutTime(1);
		SetDiffuseColor(1.0, 0.3, 0.3);
		SetAmbientColor(1.0, 0.3, 0.3);
		SetFlareVisible(true);
		SetFlickerAmplitude(0.2);
		SetFlickerSpeed(1.5);
		SetDancingShadowsMovementSpeed(0.1);
		SetDancingShadowsAmplitude(0.015);
		EnableHeatHaze(true);
		SetHeatHazeRadius(0.1);
		SetHeatHazePower(0.02);
	}
	
	/*override void OnFrameLightSource(IEntity other, float timeSlice)
	{
		
	}*/
}