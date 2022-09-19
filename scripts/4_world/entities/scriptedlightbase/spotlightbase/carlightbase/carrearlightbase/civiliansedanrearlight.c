class CivilianSedanRearLight extends CarRearLightBase
{
	void CivilianSedanRearLight()
	{
		// Brake light only
		m_SegregatedBrakeBrightness = 1;
		m_SegregatedBrakeRadius = 6;
		m_SegregatedBrakeAngle = 270;
		m_SegregatedBrakeColorRGB = Vector(1, 0.6, 0.05);
		
		// Reverse light only
		m_SegregatedBrightness = 2;
		m_SegregatedRadius = 13;
		m_SegregatedAngle = 180;
		m_SegregatedColorRGB = Vector(1.0, 1.0, 0.8);
		
		// Brake & Revese lights combined
		m_AggregatedBrightness = 2.5;
		m_AggregatedRadius = 15;
		m_AggregatedAngle = 180;
		m_AggregatedColorRGB = Vector(1.0, 0.8, 0.6);
		
		FadeIn(0.1);
		SetFadeOutTime(0.1);
		SetVisibleDuringDaylight(false);
		SetCastShadow(false);
		SetFlareVisible(false);
		
		SegregateLight();
	}
}