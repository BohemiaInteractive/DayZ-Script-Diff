class Hit_Ice : EffBulletImpactBase
{
	void Hit_Ice()
	{
		SetEnterParticle(ParticleList.IMPACT_ICE_ENTER);
		SetExitParticle(ParticleList.IMPACT_ICE_EXIT);
		SetRicochetParticle(ParticleList.IMPACT_ICE_RICOCHET);
	}
}