class Motorbike_02LightProfileFront : VehicleLightProfileFront
{
	void Motorbike_02LightProfileFront()
	{
		m_SegregatedBrightness 	= 3;
		m_SegregatedRadius 		= 65;
		m_SegregatedAngle 		= 65;
		m_SegregatedColorRGB 	= Vector(0.85, 0.85, 0.68);
	}
}

class Motorbike_02LightProfileRear : VehicleLightProfileBrakeAndReverse
{
	void Motorbike_02LightProfileRear()
	{
		// Brake light only
		m_SegregatedBrakeBrightness = 4;
		m_SegregatedBrakeRadius 	= 6;
		m_SegregatedBrakeAngle 		= 120;
		m_SegregatedBrakeColorRGB 	= Vector(0.95, 0.16, 0.05);
		
		// Reverse light only
		m_SegregatedBrightness 	= 2;
		m_SegregatedRadius 		= 10;
		m_SegregatedAngle 		= 120;
		m_SegregatedColorRGB 	= Vector(0.80, 0.85, 0.9);
		
		// Brake & Revese lights combined
		m_AggregatedBrightness 	= 4;
		m_AggregatedRadius 		= 10;
		m_AggregatedAngle 		= 120;
		m_AggregatedColorRGB 	= Vector(0.9, 0.4, 0.5);	
	}
}
