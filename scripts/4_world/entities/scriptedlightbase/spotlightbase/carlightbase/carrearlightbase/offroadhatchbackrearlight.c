class OffroadHatchbackRearLight extends CarRearLightBase
{
	void OffroadHatchbackRearLight()
	{
		// Brake light only
		m_SegregatedBrakeBrightness = 1;
		m_SegregatedBrakeRadius = 6;
		m_SegregatedBrakeAngle = 180;
		m_SegregatedBrakeColorRGB = Vector(1, 0.05, 0.05);
		
		// Reverse light only
		m_SegregatedBrightness = 2;
		m_SegregatedRadius = 13;
		m_SegregatedAngle = 180;
		m_SegregatedColorRGB = Vector(1.0, 1.0, 1.0);
		
		// Brake & Revese lights combined
		m_AggregatedBrightness = 2.5;
		m_AggregatedRadius = 15;
		m_AggregatedAngle = 180;
		m_AggregatedColorRGB = Vector(1.0, 0.5, 0.5);
		
		FadeIn(0.1);
		SetFadeOutTime(0.1);
		SetVisibleDuringDaylight(false);
		SetCastShadow(false);
		SetFlareVisible(false);
		
		SegregateLight();
	}
}