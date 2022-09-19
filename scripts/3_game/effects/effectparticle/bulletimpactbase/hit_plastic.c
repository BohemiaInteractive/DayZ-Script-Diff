class Hit_Plastic : EffBulletImpactBase
{
	void Hit_Plastic()
	{
		SetEnterParticle(ParticleList.IMPACT_PLASTIC_ENTER);
		SetRicochetParticle(ParticleList.IMPACT_PLASTIC_ENTER);
		SetExitParticle(ParticleList.IMPACT_PLASTIC_ENTER);
	}
}