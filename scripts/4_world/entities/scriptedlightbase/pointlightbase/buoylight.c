class BuoyLight extends PointLightBase
{
	const string LIGHT_MAT_ON = "DZ\\structures_sakhal\\industrial\\harbour\\data\\Buoy_01_glass_e.rvmat";
	protected const string LIGHT_MAT_OFF = "DZ\\structures_sakhal\\industrial\\harbour\\data\\Buoy_01_glass.rvmat";
	protected const int BASE_BRIGTHNESS = 5;
	
	protected bool m_IsOn;
		
	void BuoyLight()
	{
		SetBrightnessTo(BASE_BRIGTHNESS);
		SetRadiusTo(15);
		SetCastShadow(true);
		SetVisibleDuringDaylight(false);
		SetFlareVisible(false);
		SetAmbientColor(1.0, 0.2, 0.2);
		SetDiffuseColor(1.0, 0.2, 0.2);
		
		DimmingConfig dimCfg = new DimmingConfig();				
		dimCfg.AddDimmingPattern({0}, {0.01}, {3}, {0.01}, {1});
		dimCfg.SetPatternQueueRepeat(-1);
		EnableDimming(BASE_BRIGTHNESS, dimCfg);
	}
	
	override void EOnFrame(IEntity other, float timeSlice)
	{		
		if (!m_LightDimming)
			return;
				
		if (!m_IsOn && m_LightDimming.GetState() == ELightDimmingState.DIMMING)
		{
			m_IsOn = true;
			UpdateLightSourceMaterial(LIGHT_MAT_OFF);
		}
		else if (m_IsOn && m_LightDimming.GetState() == ELightDimmingState.BRIGHTENING)
		{
			m_IsOn = false;
			UpdateLightSourceMaterial(LIGHT_MAT_ON);
		}
	}
}

class MooringBuoyLight extends PointLightBase
{
	const string LIGHT_MAT_ON = "DZ\\structures_sakhal\\industrial\\harbour\\data\\Mooring_Buoy_Glass_e.rvmat";
	
	void MooringBuoyLight()
	{
		SetBrightnessTo(5);
		SetRadiusTo(10);
		SetCastShadow(false);
		SetVisibleDuringDaylight(false);
		SetAmbientColor(1.0, 0.2, 0.2);
		SetDiffuseColor(1.0, 0.2, 0.2);
	}
}