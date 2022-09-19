class TorchLight extends PointLightBase
{
	static float m_TorchRadius = 30;
	static float m_TorchBrightness = 5.0;
	
	void TorchLight()
	{
		SetVisibleDuringDaylight(false);
		SetRadiusTo( m_TorchRadius );
		SetBrightnessTo(m_TorchBrightness);
		SetCastShadow(true);
		SetFadeOutTime(1);
		SetDiffuseColor(1.0, 0.45, 0.25);
		SetAmbientColor(1.0, 0.45, 0.25);
		SetFlareVisible(false);
		SetFlickerAmplitude(0.55);
		SetFlickerSpeed(0.75);
		SetDancingShadowsMovementSpeed(0.1);
		SetDancingShadowsAmplitude(0.03);
		EnableHeatHaze(true);
		SetHeatHazeRadius(0.08);
		SetHeatHazePower(0.015);
	}
	
	override void OnFrameLightSource(IEntity other, float timeSlice)
	{
		
	}
}