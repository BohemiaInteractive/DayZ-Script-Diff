class Sedan_02FrontLight extends CarLightBase
{
	void Sedan_02FrontLight()
	{
		m_SegregatedBrightness = 4;
		m_SegregatedRadius = 75;
		m_SegregatedAngle = 90;
		m_SegregatedColorRGB = Vector(0.95, 0.95, 0.78);
		
		m_AggregatedBrightness = 8;
		m_AggregatedRadius = 100;
		m_AggregatedAngle = 120;
		m_AggregatedColorRGB = Vector(0.95, 0.95, 0.78);
		
		FadeIn(0.1);
		SetFadeOutTime(0.05);
		
		SegregateLight();
	}
}