class Hit_Foliage_Conifer: Hit_Foliage
{
	void Hit_Foliage_Conifer()
	{
		SetEnterParticle(ParticleList.IMPACT_FOLIAGE_CONIFER_ENTER);
		SetExitParticle(ParticleList.IMPACT_FOLIAGE_CONIFER_EXIT);
		SetRicochetParticle(ParticleList.IMPACT_FOLIAGE_CONIFER_RICOCHET);
	}
}