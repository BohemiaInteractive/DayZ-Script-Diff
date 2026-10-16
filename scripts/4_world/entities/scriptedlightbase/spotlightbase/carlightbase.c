
class CarLightBase extends SpotLightBase
{
	// What follows are just default light parameters which are overwriten down in the hierarchy in constructors.
	// 'Segregated' means value for a light source that consists of 1 bulb (ex.: when 1 of 2 headlights are destroyed)
	// 'Aggregated' means value for 1 light source that consists of 2 bulbs (ex.: when both headlights are shining, or brake & reverse lights are shining at once)
	// Aggregating lights saves a lot of performance
	
	float		m_SegregatedBrightness = 20;
	float		m_SegregatedRadius = 66;
	float		m_SegregatedAngle = 90;
	vector		m_SegregatedColorRGB = Vector(1.0, 0.8, 0.6);
	
	float		m_AggregatedBrightness = 30;
	float		m_AggregatedRadius = 100;
	float		m_AggregatedAngle = 100;
	vector		m_AggregatedColorRGB = Vector(1.0, 0.8, 0.6);
	
	VehicleLightProfileBase m_LightProfile;
	
	void CarLightBase()
	{
		SetVisibleDuringDaylight(true);
		SetCastShadow(true);
		SetFlareVisible(true);
		FadeIn(0.3);
		SetFadeOutTime(0.25);
	}
	
	void SetLightProfile(notnull VehicleLightProfileBase profile)
	{
		m_LightProfile = profile;
	}
	
	void AggregateLight()
	{
		//! for backward compatibility
		SetRadiusTo(m_AggregatedRadius);
		SetSpotLightAngle(m_AggregatedAngle);
		SetBrightnessTo(m_AggregatedBrightness);
		SetAmbientColor(m_AggregatedColorRGB[0], m_AggregatedColorRGB[1], m_AggregatedColorRGB[2]);
		SetDiffuseColor(m_AggregatedColorRGB[0], m_AggregatedColorRGB[1], m_AggregatedColorRGB[2]);
		SetFlareVisible(false);
	}
	
	void SegregateLight()
	{
		if (m_LightProfile)
		{
			SetRadiusTo(m_LightProfile.m_SegregatedRadius);
			SetSpotLightAngle(m_LightProfile.m_SegregatedAngle);
			SetBrightnessTo(m_LightProfile.m_SegregatedBrightness);
			vector color = m_LightProfile.m_SegregatedColorRGB;
			SetAmbientColor(color[0], color[1], color[2]);
			SetDiffuseColor(color[0], color[1], color[2]);
			SetFlareVisible(true);

			return;		
		}

		//! for backward compatibility
		SetRadiusTo(m_SegregatedRadius);
		SetSpotLightAngle(m_SegregatedAngle);
		SetBrightnessTo(m_SegregatedBrightness);
		SetAmbientColor(m_SegregatedColorRGB[0], m_SegregatedColorRGB[1], m_SegregatedColorRGB[2]);
		SetDiffuseColor(m_SegregatedColorRGB[0], m_SegregatedColorRGB[1], m_SegregatedColorRGB[2]);
		SetFlareVisible(true);
	}
}
