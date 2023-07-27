// The base area for Spooky land, spooky particles and spooky triggers for a spooky halloween
class SpookyArea : EffectArea
{
	// ----------------------------------------------
	// 				INITIAL SETUP
	// ----------------------------------------------
	override void EEInit()
	{
		// We make sure we have the particle array
		if ( !m_ToxicClouds )
			m_ToxicClouds = new array<Particle>;
		
		SetSynchDirty();
		
		#ifdef DEVELOPER
		// Debugs when placing entity by hand using internal tools
		if ( GetGame().IsServer() && !GetGame().IsMultiplayer() )
		{
			Debug.Log("YOU CAN IGNORE THE FOLLOWING DUMP");
			InitZone();
			Debug.Log("YOU CAN USE FOLLOWING DATA PROPERLY");
		}
		#endif
		
		if ( GetGame().IsClient() && GetGame().IsMultiplayer() )
			InitZone();
		
		super.EEInit();
	}
	
	override void InitZoneServer()
	{
		super.InitZoneServer();
		
		// We create the trigger on server
		if ( m_TriggerType != "" )
			CreateTrigger( m_Position, m_Radius );
	}
	
	override void InitZoneClient()
	{
		super.InitZoneClient();
		
		// We spawn VFX on client
		PlaceParticles( GetWorldPosition(), m_Radius, m_InnerRings, m_InnerSpacing, m_OuterRingToggle, m_OuterSpacing, m_OuterRingOffset, m_ParticleID );
	}
}

class SpookyArea23 : SpookyArea
{
	override void OnPlayerEnterServer(PlayerBase player, EffectTrigger trigger)
	{
		super.OnPlayerEnterServer(player, trigger);
		
		EntityAI spookyEntity = EntityAI.Cast(GetGame().CreateObjectEx("SpookyPlayerStalker","0 0 0",ECE_SETUP));
		int boneIdx = player.GetBoneIndexByName("Head");
		player.AddChild(spookyEntity,boneIdx,true);
	}
	
	override void OnPlayerExitServer(PlayerBase player, EffectTrigger trigger)
	{
		super.OnPlayerExitServer(player, trigger);
		
		array<typename> types = {SpookyPlayerStalker};
		MiscGameplayFunctions.RemoveAllAttachedChildrenByTypename(player,types);
	}
}