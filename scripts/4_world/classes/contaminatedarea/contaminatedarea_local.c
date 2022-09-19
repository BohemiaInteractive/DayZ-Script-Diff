class ContaminatedArea_Local : ContaminatedArea_Dynamic
{
	const float TICK_RATE 	= 1;
	ref Timer 	m_Timer1 	= new Timer;
	float 		m_Lifetime 	= 360;
	// ----------------------------------------------
	// 				INITIAL SETUP
	// ----------------------------------------------
	
	override void SetupZoneData(  EffectAreaParams params )
	{
		params.m_ParamPartId 		= ParticleList.CONTAMINATED_AREA_GAS_AROUND;
		params.m_ParamInnerRings 	= 0;
		params.m_ParamPosHeight 	= 3;
		params.m_ParamNegHeight 	= 5;
		params.m_ParamRadius 		= 10;
		params.m_ParamOuterToggle 	= false;
		params.m_ParamTriggerType 	= "ContaminatedTrigger_Dynamic";
		
		params.m_ParamAroundPartId 	= 0;
		params.m_ParamTinyPartId 	= 0;
		
		super.SetupZoneData( params );
	}
	
	override void EEInit()
	{
		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
			SetupZoneData(new EffectAreaParams);
			m_Timer1.Run(TICK_RATE, this, "Tick", NULL, true);
		}
	}
	
	override void SpawnItems()
	{
		// override base funcionality as we don't want any items spawned here
	}
	
	override float GetStartDecayLifetime()
	{
		return 20;
	}
	
	override float GetFinishDecayLifetime()
	{
		return 10;
	}
	
	override float GetRemainingTime()
	{
		return m_Lifetime;
	}

	override void Tick()
	{
		super.Tick();
		m_Lifetime -= TICK_RATE;
		if (m_Lifetime <= 0)
		{
			Delete();
		}
	}

	
}