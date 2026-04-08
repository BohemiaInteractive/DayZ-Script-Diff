// The base area for Spooky land, spooky particles and spooky triggers for a spooky halloween
class SpookyArea : EffectArea
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
		if ( m_TriggerType != "" )
			CreateTrigger(m_PositionTrigger, m_Radius);
	}
	
	override void InitZoneClient()
	{
		super.InitZoneClient();
		
		// We spawn VFX on client
		PlaceParticles(m_Position, m_Radius, m_InnerRings, m_InnerSpacing, m_OuterRingToggle, m_OuterSpacing, m_OuterRingOffset, m_ParticleID);
	}
}

class SpookyArea23 : SpookyArea
{
	override void OnPlayerEnterServer(PlayerBase player, EffectTrigger trigger)
	{
		super.OnPlayerEnterServer(player, trigger);
		
		EntityAI spookyEntity = EntityAI.Cast(g_Game.CreateObjectEx("SpookyPlayerStalker","0 0 0",ECE_SETUP));
		int boneIdx = player.GetBoneIndexByName("Head");
		player.AddChild(spookyEntity,boneIdx,true);
	}
	
	override void OnPlayerExitServer(PlayerBase player, EffectTrigger trigger)
	{
		super.OnPlayerExitServer(player, trigger);
		
		array<typename> types = {SpookyPlayerStalker};
		MiscGameplayFunctions.DeleteAttachedChildrenByTypename(player,types);
	}
}