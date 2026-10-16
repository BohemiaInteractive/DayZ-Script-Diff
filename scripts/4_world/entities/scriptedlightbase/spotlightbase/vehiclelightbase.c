class VehicleLightBase : SpotLightBase
{
	// What follows are just default light parameters which are overwriten down in the hierarchy in constructors.
	// 'Segregated' means value for a light source that consists of 1 bulb (ex.: when 1 of 2 headlights are destroyed)
	// 'Aggregated' means value for 1 light source that consists of 2 bulbs (ex.: when both headlights are shining, or brake & reverse lights are shining at once)
	// Aggregating lights saves a lot of performance

	VehicleLightProfileBase m_LightProfile;
	
	void VehicleLightBase()
	{
		SetVisibleDuringDaylight(true);
		SetCastShadow(true);
		SetFlareVisible(true);
		FadeIn(0.3);
		SetFadeOutTime(0.25);
		
		SegregateLight();
	}
	
	void SetLightProfile(notnull VehicleLightProfileBase profile)
	{
		m_LightProfile = profile;
	}
	
	void AggregateLight()
	{
		if (m_LightProfile)
		{
			m_LightProfile.ApplyAggregatedValues();

			SetRadiusTo(m_LightProfile.m_AggregatedRadius);
			SetSpotLightAngle(m_LightProfile.m_AggregatedAngle);
			SetBrightnessTo(m_LightProfile.m_AggregatedBrightness);
			vector color = m_LightProfile.m_AggregatedColorRGB;
			SetAmbientColor(color[0], color[1], color[2]);
			SetDiffuseColor(color[0], color[1], color[2]);
			SetFlareVisible(m_LightProfile.m_FlaresVisible);
		}
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
			SetFlareVisible(m_LightProfile.m_FlaresVisible);
		}
	}
	
	void SetAsSegregatedBrakeLight()
	{
		if (m_LightProfile)
		{
			SetRadiusTo(m_LightProfile.m_SegregatedBrakeRadius);
			SetSpotLightAngle(m_LightProfile.m_SegregatedBrakeAngle);
			SetBrightnessTo(m_LightProfile.m_SegregatedBrakeBrightness);
			vector color = m_LightProfile.m_SegregatedBrakeColorRGB;
			SetAmbientColor(color[0], color[1], color[2]);
			SetDiffuseColor(color[0], color[1], color[2]);
			SetFadeOutTime(m_LightProfile.m_FadeOutTime);
			SetFlareVisible(m_LightProfile.m_FlaresVisible);
		}
	}

	void SetAsSegregatedReverseLight()
	{
		if (m_LightProfile)
		{
			SetRadiusTo(m_LightProfile.m_SegregatedRadius);
			SetSpotLightAngle(m_LightProfile.m_SegregatedAngle);
			SetBrightnessTo(m_LightProfile.m_SegregatedBrightness);
			vector color = m_LightProfile.m_SegregatedColorRGB;
			SetAmbientColor(color[0], color[1], color[2]);
			SetDiffuseColor(color[0], color[1], color[2]);
			SetFadeOutTime(m_LightProfile.m_FadeOutTime);
			SetFlareVisible(m_LightProfile.m_FlaresVisible);
		}
	}
	
}
