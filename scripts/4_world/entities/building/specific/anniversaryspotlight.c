class AnniversarySpotLight : Building
{
	#ifndef SERVER
	#ifndef NO_GUI
	protected AnniversaryMainLight m_Light;	
	#endif
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
		#ifndef NO_GUI
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
		#endif
	}
	
	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);
		
		#ifndef SERVER
		#ifndef NO_GUI
		if (m_Light)
			m_Light.Destroy();
		#endif
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