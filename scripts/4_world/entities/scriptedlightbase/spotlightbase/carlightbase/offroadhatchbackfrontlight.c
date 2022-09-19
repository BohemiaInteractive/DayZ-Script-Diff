class OffroadHatchbackFrontLight extends CarLightBase
{
	void OffroadHatchbackFrontLight()
	{
		m_SegregatedBrightness = 5;
		m_SegregatedRadius = 55;
		m_SegregatedAngle = 110;
		m_SegregatedColorRGB = Vector(1.0, 0.8, 0.6);
		
		m_AggregatedBrightness = 10;
		m_AggregatedRadius = 75;
		m_AggregatedAngle = 120;
		m_AggregatedColorRGB = Vector(1.0, 0.8, 0.6);
		
		FadeIn(0.3);
		SetFadeOutTime(0.25);
		
		SegregateLight();
	}
}