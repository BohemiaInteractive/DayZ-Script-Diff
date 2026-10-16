class Hit_Mud_Brick : EffBulletImpactBase
{
	void Hit_Mud_Brick()
	{
		SetEnterParticle(ParticleList.IMPACT_MUD_BRICK_ENTER);
		SetRicochetParticle(ParticleList.IMPACT_MUD_BRICK_RICOCHET);
		SetExitParticle(ParticleList.IMPACT_MUD_BRICK_EXIT);
	}
}