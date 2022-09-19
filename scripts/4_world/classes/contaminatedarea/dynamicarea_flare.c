// The dummy entity which is spawned in the announcement process of Dynamic Contaminated Areas
class DynamicArea_Flare : ScriptedEntity
{
	const float FALL_SPEED = 0.5; // The speed at which this object falls ( units per second )
	const float LIFETIME = 40000; // How long this entity will stay in world
	private Particle m_Particle;
	void DynamicArea_Flare()
	{
		SetEventMask( EntityEvent.FRAME );
		SetFlags( EntityFlags.ACTIVE, false );
		
		// We create the particle effect
		m_Particle = ParticleManager.GetInstance().PlayOnObject( ParticleList.FLAREPROJ_ACTIVATE_RED, this );
		
		// We specify we will delete this object after set time
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater( GetGame().ObjectDeleteOnClient, LIFETIME, false, this );
	}
	
	// We slowly fall
	override void EOnFrame( IEntity other, float timeSlice )
	{
		vector newPos = GetPosition();
		newPos[1] = newPos[1] - ( FALL_SPEED * timeSlice );
		SetPosition( newPos );
	}
	
	// We delete the entity and stop the particles
	override void EEDelete( EntityAI parent )
	{
		if ( m_Particle )
			m_Particle.Stop();
	}
}