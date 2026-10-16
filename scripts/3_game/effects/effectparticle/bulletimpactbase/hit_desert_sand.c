class Hit_Desert_Sand : EffBulletImpactBase
{
	void Hit_Desert_Sand()
	{
		SetEnterParticle(ParticleList.IMPACT_DESERT_SAND_ENTER);
		SetRicochetParticle(ParticleList.IMPACT_DESERT_SAND_RICOCHET);
		SetExitParticle(ParticleList.IMPACT_DESERT_SAND_EXIT);
	}
}