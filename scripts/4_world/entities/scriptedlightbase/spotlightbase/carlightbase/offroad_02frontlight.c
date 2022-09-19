class Offroad_02FrontLight extends CarLightBase
{
	void Offroad_02FrontLight()
	{
		m_SegregatedBrightness = 5;
		m_SegregatedRadius = 75;
		m_SegregatedAngle = 90;
		m_SegregatedColorRGB = Vector(0.85, 0.85, 0.58);
		
		m_AggregatedBrightness = 10;
		m_AggregatedRadius = 100;
		m_AggregatedAngle = 120;
		m_AggregatedColorRGB = Vector(0.85, 0.85, 0.58);
		
		FadeIn(0.1);
		SetFadeOutTime(0.05);
		
		SegregateLight();
	}
};