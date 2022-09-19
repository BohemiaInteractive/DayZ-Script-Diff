class EntranceLight extends PointLightBase
{
	protected float m_DefaultBrightness = 4;
	protected float m_DefaultRadius = 7.5;
	
	void EntranceLight()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(m_DefaultRadius);
		SetBrightnessTo(m_DefaultBrightness);
		FadeIn(1);
		SetFadeOutTime(2);
		SetFlareVisible(false);
		SetCastShadow(false);
		SetAmbientColor(0.9, 0.9, 1.0);
		SetDiffuseColor(0.9, 0.9, 1.0);
	}
}

class EntranceLightStairs1 : EntranceLight
{
	void EntranceLightStairs1()
	{				
		SetRadiusTo(10);
		SetBrightnessTo(0.5);
		SetCastShadow(true);
		SetAmbientColor(1.0, 0.4, 0.4);
		SetDiffuseColor(1.0, 0.4, 0.4);
	}
}

class EntranceLightStairs2 : EntranceLight
{
	void EntranceLightStairs2()
	{		
		SetRadiusTo(5.5);
		SetBrightnessTo(0.5);
		SetAmbientColor(0.9, 0.9, 0.7);
		SetDiffuseColor(0.9, 0.9, 0.7);
	}
}

class EntranceLightMain1 : EntranceLight
{
	void EntranceLightMain1()
	{		
		SetRadiusTo(10);
		SetBrightnessTo(1.0);
		SetAmbientColor(0.9, 0.9, 0.7);
		SetDiffuseColor(0.9, 0.9, 0.7);
	}
}

class EntranceLightMain2 : EntranceLight
{
	void EntranceLightMain2()
	{				
		SetRadiusTo(10);
		SetBrightnessTo(1.0);
		SetAmbientColor(0.9, 0.9, 0.7);
		SetDiffuseColor(0.9, 0.9, 0.7);
	}
}