class Offroad_02LightProfileFront : VehicleLightProfileFront
{
	void Offroad_02LightProfileFront()
	{
		m_SegregatedBrightness = 5;
		m_SegregatedRadius = 75;
		m_SegregatedAngle = 90;
		m_SegregatedColorRGB = Vector(0.85, 0.85, 0.58);
		
		m_AggregatedBrightness = 10;
		m_AggregatedRadius = 100;
		m_AggregatedAngle = 120;
		m_AggregatedColorRGB = Vector(0.85, 0.85, 0.58);
		
		m_FadeInTime = 0.1;
		m_FadeOutTime = 0.05;
	}
}

class Offroad_02LightProfileRear : VehicleLightProfileBrakeAndReverse
{
	void Offroad_02LightProfileRear()
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
		
		m_FadeInTime = 0.1;
		m_FadeOutTime = 0.05;
	}
};