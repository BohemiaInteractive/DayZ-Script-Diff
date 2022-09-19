class Hit_Metal : EffBulletImpactBase
{
	void Hit_Metal()
	{
		SetEnterParticle(ParticleList.IMPACT_METAL_ENTER);
		SetExitParticle(ParticleList.IMPACT_METAL_EXIT);
		SetRicochetParticle(ParticleList.IMPACT_METAL_RICOCHET);
	}
}