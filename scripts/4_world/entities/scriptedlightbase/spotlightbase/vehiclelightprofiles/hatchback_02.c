class Hatchback_02LightProfileFront : VehicleLightProfileFront
{
	void Hatchback_02LightProfileFront()
	{
		m_SegregatedBrightness = 6;
		m_SegregatedRadius = 65;
		m_SegregatedAngle = 80;
		m_SegregatedColorRGB = Vector(0.8, 0.8, 1);
		
		m_AggregatedBrightness = 12;
		m_AggregatedRadius = 90;
		m_AggregatedAngle = 90;
		m_AggregatedColorRGB = Vector(0.8, 0.8, 1);
		
		m_FadeInTime = 0.3;
	}
}

class Hatchback_02LightProfileRear : VehicleLightProfileBrakeAndReverse
{
	void Hatchback_02LightProfileRear()
	{
		// Brake light only
		m_SegregatedBrakeBrightness = 1;
		m_SegregatedBrakeRadius = 6;
		m_SegregatedBrakeAngle = 270;
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
		
		m_FadeInTime 	= 0.1;
		m_FadeOutTime 	= 0.1;
	}
}
