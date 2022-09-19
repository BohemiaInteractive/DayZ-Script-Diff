class Truck_01FrontLight extends CarLightBase
{
	void Truck_01FrontLight()
	{
		m_SegregatedBrightness = 4;
		m_SegregatedRadius = 75;
		m_SegregatedAngle = 90;
		m_SegregatedColorRGB = Vector(0.90, 0.90, 0.68);
		
		m_AggregatedBrightness = 8;
		m_AggregatedRadius = 100;
		m_AggregatedAngle = 120;
		m_AggregatedColorRGB = Vector(0.90, 0.90, 0.68);
		
		FadeIn(0.1);
		SetFadeOutTime(0.05);
		
		SegregateLight();
	}
}