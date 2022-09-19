class Hit_Textile : EffBulletImpactBase
{
	void Hit_Textile()
	{
		SetEnterParticle(ParticleList.IMPACT_TEXTILE_ENTER);
		SetRicochetParticle(ParticleList.IMPACT_TEXTILE_ENTER);
		SetExitParticle(ParticleList.IMPACT_TEXTILE_EXIT);
	}
}