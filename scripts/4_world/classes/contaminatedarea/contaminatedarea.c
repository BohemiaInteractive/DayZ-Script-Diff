class ContaminatedArea_Base : EffectArea
{
	override void OnPlayerEnterServer(PlayerBase player, EffectTrigger trigger)
	{
		super.OnPlayerEnterServer(player, trigger);

		player.IncreaseContaminatedAreaCount();
	}
	
	override void OnPlayerExitServer(PlayerBase player, EffectTrigger trigger)
	{
		super.OnPlayerExitServer(player, trigger);

		player.DecreaseContaminatedAreaCount();
	}
	
	override void InitZoneClient()
	{
		super.InitZoneClient();
		
		g_Game.GetMission().GetDynamicMusicPlayer().RegisterDynamicLocation(this, DynamicMusicLocationTypes.CONTAMINATED_ZONE, m_Radius);
	}
	
	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);
		
		if (!g_Game.IsServer())
			g_Game.GetMission().GetDynamicMusicPlayer().UnregisterDynamicLocation(this);
	}
}

class ContaminatedArea_Static : ContaminatedArea_Base
{
	// ----------------------------------------------
	// 				INITIAL SETUP
	// ----------------------------------------------
	
	override void EEInit()
	{
		if (!m_ToxicClouds)
			m_ToxicClouds = new array<Particle>();
		
		SetSynchDirty();
		
		super.EEInit();
	}
	
	override void DeferredInit()
	{
		super.DeferredInit();
		
		InitZone();		
	}
	
	override void InitZoneServer()
	{
		super.InitZoneServer();
		
		// We create the trigger on server
		if (m_TriggerType != "")
			CreateTrigger(m_PositionTrigger, m_Radius);
	}
	
	override void InitZoneClient()
	{
		super.InitZoneClient();
		
		// We spawn VFX on client
		FillWithParticles(m_Position, m_Radius, m_OuterRingOffset, m_InnerSpacing, m_ParticleID);
	}
	
	override void OnDebugSpawn()
	{
		super.OnDebugSpawn();
		
		InitZone();
	}
}