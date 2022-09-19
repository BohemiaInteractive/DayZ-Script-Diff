class Hit_Dirt : EffBulletImpactBase
{
	void Hit_Dirt()
	{
		SetEnterParticle(ParticleList.IMPACT_DIRT_ENTER);
		SetExitParticle(ParticleList.IMPACT_DIRT_EXIT);
		SetRicochetParticle(ParticleList.IMPACT_DIRT_RICOCHET);
	}
}