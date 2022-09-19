class SmokeSimulation : Entity
{
	protected Particle 			m_ParMainSmoke;
	int particle_id;
	
	void SmokeSimulation()
	{
		particle_id = ParticleList.GRENADE_M18_PURPLE_LOOP;
	}

	void OnFire( Entity flare)
	{
		m_ParMainSmoke = ParticleManager.GetInstance().PlayOnObject( particle_id, flare);
		m_ParMainSmoke.SetWiggle( 7, 0.3);
	}

	void ~SmokeSimulation()
	{
		if (m_ParMainSmoke)
			m_ParMainSmoke.Stop();
	}
}

class SmokeSimulation_Black : SmokeSimulation
{
	void SmokeSimulation_Black()
	{
		particle_id = ParticleList.GRENADE_M18_BLACK_LOOP;
	}
}
class SmokeSimulation_White : SmokeSimulation
{
	void SmokeSimulation_White()
	{
		particle_id = ParticleList.GRENADE_M18_WHITE_LOOP;
	}
}
class SmokeSimulation_Red : SmokeSimulation
{
	void SmokeSimulation_Red()
	{
		particle_id = ParticleList.GRENADE_M18_RED_LOOP;
	}
}
class SmokeSimulation_Green : SmokeSimulation
{
	void SmokeSimulation_Green()
	{
		particle_id = ParticleList.GRENADE_M18_GREEN_LOOP;
	}
}
