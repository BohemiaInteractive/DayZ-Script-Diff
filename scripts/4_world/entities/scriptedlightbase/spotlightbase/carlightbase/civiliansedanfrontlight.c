class CivilianSedanFrontLight extends CarLightBase
{
	void CivilianSedanFrontLight()
	{
		m_SegregatedBrightness = 5;
		m_SegregatedRadius = 65;
		m_SegregatedAngle = 90;
		m_SegregatedColorRGB = Vector(1.0, 0.8, 0.8);
		
		m_AggregatedBrightness = 10;
		m_AggregatedRadius = 80;
		m_AggregatedAngle = 100;
		m_AggregatedColorRGB = Vector(1.0, 0.8, 0.8);
		
		FadeIn(0.3);
		SetFadeOutTime(0.25);
		
		SegregateLight();
	}
}