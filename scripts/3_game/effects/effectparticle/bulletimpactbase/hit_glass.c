class Hit_Glass : EffBulletImpactBase
{
	void Hit_Glass()
	{
		SetEnterParticle(ParticleList.IMPACT_GLASS_ENTER);
		SetExitParticle(ParticleList.IMPACT_GLASS_EXIT);
		SetRicochetParticle(ParticleList.IMPACT_GLASS_RICOCHET);
	}
}