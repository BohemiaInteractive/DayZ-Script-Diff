class Hit_Grass : EffBulletImpactBase
{
	void Hit_Grass()
	{
		SetEnterParticle(ParticleList.IMPACT_GRASS_ENTER);
		SetExitParticle(ParticleList.IMPACT_GRASS_ENTER);
		SetRicochetParticle(ParticleList.IMPACT_GRASS_RICOCHET);
		
		m_AngledEnter = 0.6;
	}
}