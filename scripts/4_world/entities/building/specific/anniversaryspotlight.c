class AnniversarySpotLight : Building
{
	#ifndef SERVER
	protected AnniversaryMainLight m_Light;	
	#endif

	protected bool m_LightState;
	
	void AnniversarySpotLight()
	{
		RegisterNetSyncVariableBool("m_LightState");
	}
	
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();

		#ifndef SERVER
		if (m_LightState)
		{
			m_Light = AnniversaryMainLight.Cast(ScriptedLightBase.CreateLight(AnniversaryMainLight, GetPosition()));
		}
		else
		{
			if (m_Light)
				m_Light.FadeOut(0.5);
		}
		#endif
	}
	
	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);
		
		#ifndef SERVER
		if (m_Light)
			m_Light.Destroy();
		#endif
	}
	
	void DeActivate()
	{
		m_LightState = false;
		SetSynchDirty();
	}
	
	void Activate()
	{
		m_LightState = true;
		SetSynchDirty();		
	}		
}