class Hit_Rubber : EffBulletImpactBase
{
	void Hit_Rubber()
	{
		SetEnterParticle(ParticleList.IMPACT_RUBBER_ENTER);
		SetExitParticle(ParticleList.IMPACT_RUBBER_EXIT);
		SetRicochetParticle(ParticleList.IMPACT_RUBBER_RICOCHET);
	}
}