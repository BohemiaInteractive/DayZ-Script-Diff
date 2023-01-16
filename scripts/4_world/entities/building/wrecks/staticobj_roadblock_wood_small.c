class StaticObj_Roadblock_Wood_Small: House
{		
	PointLightBase m_BlinkingLight;
	
	override void EEInit()
	{
		super.EEInit();
	
		if ( !GetGame().IsDedicatedServer() )
		{
			m_BlinkingLight = EntranceLight.Cast(ScriptedLightBase.CreateLightAtObjMemoryPoint(Roadblock_Light, this, "Light"));
		}
	}
	
	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);
		
		if ( !GetGame().IsDedicatedServer() )
		{
			if ( m_BlinkingLight )
				m_BlinkingLight.Destroy();
		}	
	}
};

class Roadblock_Light extends PointLightBase
{
	protected float m_DefaultBrightness = 5.5;
	protected float m_DefaultRadius = 20;
	
	void Roadblock_Light()
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
		
		SetBlinkingSpeed(1);
	}
}