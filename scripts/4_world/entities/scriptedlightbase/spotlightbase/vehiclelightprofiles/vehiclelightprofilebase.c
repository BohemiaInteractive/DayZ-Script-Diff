//! Base class for vehicle light profiles - all changeble parameters goes here
class VehicleLightProfileBase
{
	bool m_DaylightVisible 	= false;
	bool m_CastShadow 		= true;
	bool m_FlaresVisible	= true;

	float m_FadeInTime		= 0.3;
	float m_FadeOutTime		= 0.25;
	
	float m_SegregatedBrightness 	= 20;
	float m_SegregatedRadius 		= 66;
	float m_SegregatedAngle 		= 90;
	vector m_SegregatedColorRGB 	= Vector(1.0, 0.8, 0.6);
	
	float m_AggregatedBrightness 	= 30;
	float m_AggregatedRadius 		= 100;
	float m_AggregatedAngle 		= 100;
	vector m_AggregatedColorRGB 	= Vector(1.0, 0.8, 0.6);
	
	// Brake light only. These are just some default values which are overwritten down the hierarchy.
	float m_SegregatedBrakeBrightness 	= 2;
	float m_SegregatedBrakeRadius 		= 6;
	float m_SegregatedBrakeAngle 		= 180;
	vector m_SegregatedBrakeColorRGB 	= Vector(1, 0.05, 0.05);
	
	void ApplySegregatedValues();
	void ApplyAggregatedValues();
}

class VehicleLightProfileFront : VehicleLightProfileBase
{
	
	void VehicleLightProfileFront()
	{
		m_DaylightVisible 		= true;

		m_SegregatedBrightness 	= 20;
		m_SegregatedRadius 		= 66;
		m_SegregatedAngle 		= 90;
		m_SegregatedColorRGB 	= Vector(1.0, 0.8, 0.6);
		
		m_AggregatedBrightness 	= 30;
		m_AggregatedRadius		= 100;
		m_AggregatedAngle 		= 100;
		m_AggregatedColorRGB 	= Vector(1.0, 0.8, 0.6);
	}
	
	override void ApplyAggregatedValues()
	{
		super.ApplyAggregatedValues();

		m_FlaresVisible = false;
	}
}

class VehicleLightProfileRear : VehicleLightProfileBase
{
	void VehicleLightProfileRear()
	{
		m_SegregatedBrightness 	= 2;
		m_SegregatedRadius 		= 10;
		m_SegregatedAngle 		= 180;
		m_SegregatedColorRGB 	= Vector(1.0, 1.0, 1.0);
		
		// Brake & Revese lights combined
		m_AggregatedBrightness 	= 2.5;
		m_AggregatedRadius 		= 12;
		m_AggregatedAngle 		= 180;
		m_AggregatedColorRGB 	= Vector(1.0, 0.5, 0.5);
	}
}

class VehicleLightProfileBrakeAndReverse : VehicleLightProfileBase
{
	void VehicleLightProfileBrakeAndReverse()
	{
		// These are just some default values which can be overwrittend down in the hierarchy.
		// Reverse light only
		m_SegregatedBrightness 	= 2;
		m_SegregatedRadius 		= 10;
		m_SegregatedAngle 		= 180;
		m_SegregatedColorRGB 	= Vector(1.0, 1.0, 1.0);
		
		// Brake & Revese lights combined
		m_AggregatedBrightness 	= 2.5;
		m_AggregatedRadius 		= 12;
		m_AggregatedAngle 		= 180;
		m_AggregatedColorRGB 	= Vector(1.0, 0.5, 0.5);
		
		m_DaylightVisible 	= false;
		m_CastShadow 		= false;
		m_FlaresVisible 	= false;
	}
}