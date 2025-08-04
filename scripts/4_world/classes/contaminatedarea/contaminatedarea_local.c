class ContaminatedArea_Local : ContaminatedArea_DynamicBase
{
	const float TICK_RATE 	= 1;
	ref Timer 	m_Timer1 	= new Timer;
	float 		m_Lifetime 	= 360;
	
	void ContaminatedArea_Local()
	{
		m_EffectsPriority = -10;
	}
	
	override void SetupZoneData(EffectAreaParams params)
	{
		params.m_ParamPartId 		= ParticleList.CONTAMINATED_AREA_GAS_AROUND;
		params.m_ParamInnerRings 	= 0;
		params.m_ParamPosHeight 	= 3;
		params.m_ParamNegHeight 	= 3;
		params.m_ParamRadius 		= 10;
		params.m_ParamOuterToggle 	= false;
		params.m_ParamTriggerType 	= "ContaminatedTrigger_Local";
		
		params.m_ParamAroundPartId 	= 0;
		params.m_ParamTinyPartId 	= 0;
		
		super.SetupZoneData(params);
		
		InitZone();
	}
	
	override void EEInit()
	{
		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
			m_Timer1.Run(TICK_RATE, this, "Tick", NULL, true);
	}

	override void DeferredInit()
	{
		if (!m_ToxicClouds)
			m_ToxicClouds = new array<Particle>();

		SetupZoneData(new EffectAreaParams);

		super.DeferredInit();
	}
	
	override void SpawnParticles(ParticlePropertiesArray props, vector centerPos, vector partPos, inout int count)
	{
		partPos[1] = GetGame().SurfaceRoadY(partPos[0], partPos[2]);	// Snap particles to ground
		
		// We make sure that spawned particle is inside the trigger	
		if (!Math.IsInRange(partPos[1], centerPos[1] - m_NegativeHeight, centerPos[1] + m_PositiveHeight))				
			partPos[1] = centerPos[1];
		
		props.Insert(ParticleProperties(partPos, ParticlePropertiesFlags.PLAY_ON_CREATION, null, GetGame().GetSurfaceOrientation( partPos[0], partPos[2] ), this));
		++count;
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
		m_Lifetime -= TICK_RATE;
		if (m_Lifetime <= 0)
			Delete();
	}
}