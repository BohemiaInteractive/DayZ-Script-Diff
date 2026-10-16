class Hit_Sand : EffBulletImpactBase
{
	void Hit_Sand()
	{
		SetEnterParticle(ParticleList.IMPACT_SAND_ENTER);
		SetRicochetParticle(ParticleList.IMPACT_SAND_RICOCHET);
		SetExitParticle(ParticleList.IMPACT_SAND_EXIT);
	}
}