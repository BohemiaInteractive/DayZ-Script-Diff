class CupidsBoltSimulation : Managed
{
	protected Particle m_ParticleTail;
	
	void OnFire(Entity entity)
	{
		if (m_ParticleTail)
			m_ParticleTail.Stop();

		m_ParticleTail = ParticleManager.GetInstance().PlayOnObject(ParticleList.BOLT_CUPID_TAIL, entity);
		m_ParticleTail.SetWiggle(7, 0.3);
	}
	
	void ~CupidsBoltSimulation()
	{
		if (m_ParticleTail)
			m_ParticleTail.Stop();
	}	
}
