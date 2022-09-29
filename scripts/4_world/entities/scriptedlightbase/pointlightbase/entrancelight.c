class EntranceLight extends PointLightBase
{
	protected float m_DefaultBrightness = 0.5;
	protected float m_DefaultRadius = 5.5;
	
	void EntranceLight()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(m_DefaultRadius);
		SetBrightnessTo(m_DefaultBrightness);
		FadeIn(1);
		SetFadeOutTime(2);
		SetFlareVisible(false);
		SetCastShadow(false);
		SetAmbientColor(0.9, 0.9, 0.7);
		SetDiffuseColor(0.9, 0.9, 0.7);
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

class EntranceLightStairs2 : EntranceLight { }

class EntranceLightMain1 : EntranceLight { }

class EntranceLightMain2 : EntranceLight { }