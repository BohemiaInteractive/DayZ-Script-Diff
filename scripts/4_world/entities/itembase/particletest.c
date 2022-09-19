// Particle test for Maxman

class ParticleTest extends ItemBase
{
	protected int	PARTICLE_PATH;
	protected Particle 	m_Particle;
	
	// Constructor
	void ParticleTest()
	{
		if ( !GetGame().IsServer()  ||  !GetGame().IsMultiplayer() ) // Client side
		{
			string path = ParticleList.GetPathToParticles();
			
			
			
			
			
			// Enter particle ID to play when ParticleTest spawns
			PARTICLE_PATH = ParticleList.DEBUG_DOT; 
			
			// Alternatively, uncomment the second line and enter particle filename without *.ptc suffix instead. Example: "menu_engine_fire" 
			string particle_filename = "menu_engine_fire";
			//PARTICLE_PATH = ParticleList.GetParticleID( path + particle_filename );
			
			
			
			
			
			m_Particle = ParticleManager.GetInstance().PlayOnObject( PARTICLE_PATH, this, GetPosition());
		}
	}

	// Destructor
	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);
		
		if (m_Particle  &&  GetGame()) // GetGame() is null when the game is being shut down
		{
			m_Particle.Stop();
			GetGame().ObjectDelete(m_Particle);
		}
	}
}