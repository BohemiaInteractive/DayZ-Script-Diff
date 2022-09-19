class ContaminatedArea_Static : EffectArea
{
	// ----------------------------------------------
	// 				INITIAL SETUP
	// ----------------------------------------------
	
	override void SetupZoneData(  EffectAreaParams params )
	{
		super.SetupZoneData( params );
	}
	
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
	
	override void EEDelete( EntityAI parent )
	{
		super.EEDelete( parent );
	}
}