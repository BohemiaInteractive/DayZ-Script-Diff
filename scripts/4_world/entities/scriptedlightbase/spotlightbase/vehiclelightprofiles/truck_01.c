class Truck_01LightProfileFront : VehicleLightProfileFront
{
	void Truck_01LightProfileFront()
	{
		m_SegregatedBrightness = 4;
		m_SegregatedRadius = 75;
		m_SegregatedAngle = 90;
		m_SegregatedColorRGB = Vector(0.90, 0.90, 0.68);
		
		m_AggregatedBrightness = 8;
		m_AggregatedRadius = 100;
		m_AggregatedAngle = 120;
		m_AggregatedColorRGB = Vector(0.90, 0.90, 0.68);
		
		m_FadeInTime 	= 0.1;
		m_FadeOutTime 	= 0.05;
	}
}

class Truck_01LightProfileRear : VehicleLightProfileBrakeAndReverse
{
	void Truck_01LightProfileRear()
	{
		// Brake light only
		m_SegregatedBrakeBrightness = 5;
		m_SegregatedBrakeRadius = 6;
		m_SegregatedBrakeAngle = 160;
		m_SegregatedBrakeColorRGB = Vector(0.75, 0.16, 0.05);
		
		// Reverse light only
		m_SegregatedBrightness = 2;
		m_SegregatedRadius = 10;
		m_SegregatedAngle = 120;
		m_SegregatedColorRGB = Vector(0.80, 0.85, 0.9);
		
		// Brake & Revese lights combined
		m_AggregatedBrightness = 5;
		m_AggregatedRadius = 10;
		m_AggregatedAngle = 160;
		m_AggregatedColorRGB = Vector(0.85, 0.35, 0.45);
		
		m_FadeInTime 	= 0.1;
		m_FadeOutTime 	= 0.05;
	}
}
