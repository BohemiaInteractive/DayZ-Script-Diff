class CarRearLightBase extends CarLightBase
{
	// Brake light only. These are just some default values which are overwritten down the hierarchy.
	float 	m_SegregatedBrakeBrightness = 2;
	float 	m_SegregatedBrakeRadius = 6;
	float 	m_SegregatedBrakeAngle = 180;
	vector 	m_SegregatedBrakeColorRGB = Vector(1, 0.05, 0.05);
	
	void CarRearLight()
	{
		// These are just some default values which can be overwrittend down in the hierarchy.
		// Reverse light only
		m_SegregatedBrightness = 2;
		m_SegregatedRadius = 10;
		m_SegregatedAngle = 180;
		m_SegregatedColorRGB = Vector(1.0, 1.0, 1.0);
		
		// Brake & Revese lights combined
		m_AggregatedBrightness = 2.5;
		m_AggregatedRadius = 12;
		m_AggregatedAngle = 180;
		m_AggregatedColorRGB = Vector(1.0, 0.5, 0.5);
		
		FadeIn(0.1);
		SetFadeOutTime(0.1);
		SetVisibleDuringDaylight(false);
		SetCastShadow(false);
		SetFlareVisible(false);
		EnableSpecular(false); // Specular is disabled because it causes ugly shining on cars
		
		SegregateLight();
	}
	
	void SetAsSegregatedBrakeLight()
	{
		SetRadiusTo(m_SegregatedBrakeRadius);
		SetSpotLightAngle(m_SegregatedBrakeAngle);
		SetBrightnessTo(m_SegregatedBrakeBrightness);
		SetAmbientColor(m_SegregatedBrakeColorRGB[0], m_SegregatedBrakeColorRGB[1], m_SegregatedBrakeColorRGB[2]);
		SetDiffuseColor(m_SegregatedBrakeColorRGB[0], m_SegregatedBrakeColorRGB[1], m_SegregatedBrakeColorRGB[2]);
		SetFadeOutTime(0.1);
		SetFlareVisible(false);
	}
	
	void SetAsSegregatedReverseLight()
	{
		SetRadiusTo(m_SegregatedRadius);
		SetSpotLightAngle(m_SegregatedAngle);
		SetBrightnessTo(m_SegregatedBrightness);
		SetAmbientColor(m_SegregatedColorRGB[0], m_SegregatedColorRGB[1], m_SegregatedColorRGB[2]);
		SetDiffuseColor(m_SegregatedColorRGB[0], m_SegregatedColorRGB[1], m_SegregatedColorRGB[2]);
		SetFadeOutTime(0.1);
		SetFlareVisible(false);
	}
}