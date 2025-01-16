class Hit_Foliage_Green : Hit_Foliage
{
	void Hit_Foliage_Green()
	{
		SetEnterParticle(ParticleList.IMPACT_FOLIAGE_GREEN_ENTER);
		SetExitParticle(ParticleList.IMPACT_FOLIAGE_GREEN_EXIT);
		SetRicochetParticle(ParticleList.IMPACT_FOLIAGE_GREEN_RICOCHET);
	}
}