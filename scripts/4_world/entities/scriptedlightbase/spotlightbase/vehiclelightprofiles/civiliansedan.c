class CivilianSedanLightProfileFront : VehicleLightProfileFront
{
	void CivilianSedanLightProfileFront()
	{
		m_SegregatedBrightness = 5;
		m_SegregatedRadius = 65;
		m_SegregatedAngle = 90;
		m_SegregatedColorRGB = Vector(1.0, 0.8, 0.8);
		
		m_AggregatedBrightness = 10;
		m_AggregatedRadius = 80;
		m_AggregatedAngle = 100;
		m_AggregatedColorRGB = Vector(1.0, 0.8, 0.8);
	}
}

class CivilianSedanLightProfileRear : VehicleLightProfileBrakeAndReverse
{
	void CivilianSedanLightProfileRear()
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
		
		m_FadeInTime 	= 0.1;
		m_FadeOutTime 	= 0.1;
	}
}
