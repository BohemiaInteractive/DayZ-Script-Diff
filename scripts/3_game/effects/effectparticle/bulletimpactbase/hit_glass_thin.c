class Hit_Glass_Thin : EffBulletImpactBase
{
	void Hit_Glass_Thin()
	{
		SetEnterParticle(ParticleList.IMPACT_GLASS_ENTER);
		SetExitParticle(ParticleList.IMPACT_GLASS_EXIT);
		SetRicochetParticle(ParticleList.IMPACT_GLASS_RICOCHET);
	}
}