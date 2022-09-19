class RDG2SmokeGrenade_ColorBase extends SmokeGrenadeBase
{
	const string SOUND_SMOKE_START = "SmokegGrenades_RDG2_start_loop_SoundSet";
	const string SOUND_SMOKE_LOOP = "SmokegGrenades_RDG2_active_loop_SoundSet";
	const string SOUND_SMOKE_END = "SmokegGrenades_RDG2_end_loop_SoundSet";
	
	void RDG2SmokeGrenade_ColorBase()
	{
		SetAmmoType("");
		SetFuseDelay(2);
		SetParticlePosition("0 0.1 0");
		SetSoundSmokeStart(SOUND_SMOKE_START);
		SetSoundSmokeLoop(SOUND_SMOKE_LOOP);
		SetSoundSmokeEnd(SOUND_SMOKE_END);
	}
	
	void ~RDG2SmokeGrenade_ColorBase() {}
};

class RDG2SmokeGrenade_Black extends RDG2SmokeGrenade_ColorBase
{
	void RDG2SmokeGrenade_Black()
	{
		SetParticleSmokeStart(ParticleList.GRENADE_RDG2_BLACK_START);
		SetParticleSmokeLoop(ParticleList.GRENADE_RDG2_BLACK_LOOP);
		SetParticleSmokeEnd(ParticleList.GRENADE_RDG2_BLACK_END);
	}
}

class RDG2SmokeGrenade_White extends RDG2SmokeGrenade_ColorBase
{
	void RDG2SmokeGrenade_White()
	{
		SetParticleSmokeStart(ParticleList.GRENADE_RDG2_WHITE_START);
		SetParticleSmokeLoop(ParticleList.GRENADE_RDG2_WHITE_LOOP);
		SetParticleSmokeEnd(ParticleList.GRENADE_RDG2_WHITE_END);
	}
}
