class Offroad_02RearLight extends CarRearLightBase
{
	void Offroad_02RearLight()
	{
		// Brake light only
		m_SegregatedBrakeBrightness = 6;
		m_SegregatedBrakeRadius = 6;
		m_SegregatedBrakeAngle = 220;
		m_SegregatedBrakeColorRGB = Vector(0.95, 0.15, 0.13);
		
		// Reverse light only
		m_SegregatedBrightness = 2;
		m_SegregatedRadius = 15;
		m_SegregatedAngle = 160;
		m_SegregatedColorRGB = Vector(0.64, 0.60, 0.62);
		
		// Brake & Revese lights combined
		m_AggregatedBrightness = 7;
		m_AggregatedRadius = 18;
		m_AggregatedAngle = 170;
		m_AggregatedColorRGB = Vector(1.0, 0.4, 0.5);
		
		FadeIn(0.1);
		SetFadeOutTime(0.05);

		SetVisibleDuringDaylight(false);
		SetCastShadow(false);
		SetFlareVisible(false);
		
		SegregateLight();
	}
};