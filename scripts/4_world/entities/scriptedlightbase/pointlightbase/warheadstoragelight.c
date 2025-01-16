class WarheadStorageLight extends PointLightBase
{	
	bool m_IsDimmed;
	bool m_IsLowEnergyDim;
	Land_WarheadStorage_Main m_Bunker;
	
	const string LIGHT_MAT_ON = "DZ\\structures_sakhal\\military\\storage\\data\\Warhead_Storage_Lamp_Glass_e_int.rvmat";
	const string LIGHT_MAT_DIM = "DZ\\structures_sakhal\\military\\storage\\data\\Warhead_Storage_Lamp_Glass_dim_int.rvmat";
	const string LIGHT_MAT_OFF = "DZ\\structures_sakhal\\military\\storage\\data\\Warhead_Storage_Lamp_Glass_int.rvmat";
	
	void WarheadStorageLight()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(7.5);
		SetBrightnessTo(6.5);
		FadeIn(1);
		SetFadeOutTime(2);
		SetFlareVisible(false);
		SetCastShadow(true);
		SetAmbientColor(1, 0.7, 0.3);
		SetDiffuseColor(1, 0.7, 0.3);
		
		UpdateLightSourceMaterial(LIGHT_MAT_ON);
		
		DimmingConfig dimCfg = new DimmingConfig();				
		dimCfg.AddDimmingPattern({60, 40, 45, 35}, {0.5, 0.1, 0.1, 0.3}, {0, 0, 0, 0}, {0.4, 0.1, 0.3, 0.6}, {3.5, 0, 0.5, 2.5});
		dimCfg.AddDimmingPattern({42, 50, 55, 38}, { 0.2, 0.5, 0.1, 0.1}, {0, 0, 0, 0}, {0.2, 0.5, 0.1, 1}, {0.2, 2, 1.2, 1.8});
		dimCfg.AddDimmingPattern({50, 40, 35, 55}, { 0.1, 0.2, 0.5, 0.2}, {0, 0, 0, 0}, {0.1, 1, 0.5, 0.3}, {1.25, 0.4, 1.5, 0.5});
		
		DimmingConfig dimCfgSecondary = new DimmingConfig();
		dimCfgSecondary.AddDimmingPattern({15, 15, 15, 15}, {0.1, 0.1, 0.1, 0.1}, {2, 0.3, 0.3, 2.5}, {0.7, 1.2, 0.9, 0.7}, {0.5, 0.9, 0.6, 0.7});
		dimCfgSecondary.AddDimmingPattern({10, 10, 10, 10}, {0.1, 0.1, 0.1, 0.1}, {2, 0.3, 0.3, 2.5}, {0.7, 1.2, 0.9, 0.7}, {0.5, 0.9, 0.6, 0.7});
		dimCfgSecondary.AddDimmingPattern({5, 5, 5, 5}, {0.1, 0.1, 0.1, 0.1}, {2, 0.3, 0.3, 2.5}, {0.7, 1.2, 0.9, 0.7}, {0.5, 0.9, 0.6, 0.7});
		dimCfgSecondary.AddDimmingPattern({0, 0, 0, 0}, {0.1, 0.1, 0.1, 0.1}, {2, 0.3, 0.3, 2.5}, {0.7, 1.2, 0.9, 0.7}, {0.5, 0.9, 0.6, 0.7});
		dimCfgSecondary.AddDimmingPattern({0, 0, 0, 0}, {0.1, 0.1, 0.1, 0.1}, {2, 0.3, 0.3, 2.5}, {0.7, 1.2, 0.9, 0.7}, {0.5, 0.9, 0.6, 0.7});
		dimCfgSecondary.AddDimmingPattern({0, 0, 0, 0}, {0.1, 0.1, 0.1, 0.1}, {2, 0.3, 0.3, 2.5}, {0.7, 1.2, 0.9, 0.7}, {0.5, 0.9, 0.6, 0.7});
		
		EnableDimming(6.5, dimCfg);
		GetDimming().AddConfig(dimCfgSecondary);
	}
	
	void ~WarheadStorageLight()
	{
		UpdateLightSourceMaterial(LIGHT_MAT_OFF);
	}
		
	override void EOnFrame(IEntity other, float timeSlice)
	{		
		if (!m_LightDimming)
			return;
		
		if (!m_Bunker)
			m_Bunker = Land_WarheadStorage_Main.Cast(m_Parent);
		
		if (!m_IsDimmed && m_LightDimming.GetState() == ELightDimmingState.DIMMING)
		{
			m_IsDimmed = true;
			if(m_IsLowEnergyDim)
			{
				UpdateLightSourceMaterial(LIGHT_MAT_OFF);
			}
			else
			{
				UpdateLightSourceMaterial(LIGHT_MAT_DIM);
			};
		}
		else if (m_IsDimmed && m_LightDimming.GetState() == ELightDimmingState.BRIGHTENING)
		{
			m_IsDimmed = false;
			UpdateLightSourceMaterial(LIGHT_MAT_ON);
		}
		
		if (m_Bunker.IsLowEnergy() && !m_IsLowEnergyDim)
		{
			m_IsLowEnergyDim = true;
			GetDimming().SwapConfig(1);
		}
		else if (!m_Bunker.IsLowEnergy() && m_IsLowEnergyDim)
		{
			m_IsLowEnergyDim = false;
			GetDimming().SwapConfig(0);
			UpdateLightSourceMaterial(LIGHT_MAT_ON);
		}
	}
}
