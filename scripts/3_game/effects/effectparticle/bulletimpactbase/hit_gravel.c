class Hit_Gravel : EffBulletImpactBase
{
	void Hit_Gravel()
	{
		SetEnterParticle(ParticleList.IMPACT_GRAVEL_ENTER);
		SetExitParticle(ParticleList.IMPACT_GRAVEL_EXIT);
		SetRicochetParticle(ParticleList.IMPACT_GRAVEL_RICOCHET);
	}
}