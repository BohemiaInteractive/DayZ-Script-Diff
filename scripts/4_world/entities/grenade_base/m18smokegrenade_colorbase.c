class M18SmokeGrenade_ColorBase extends SmokeGrenadeBase
{
	const string SOUND_SMOKE_START = "SmokegGrenades_M18_start_loop_SoundSet";
	const string SOUND_SMOKE_LOOP = "SmokegGrenades_M18_active_loop_SoundSet";
	const string SOUND_SMOKE_END = "SmokegGrenades_M18_end_loop_SoundSet";

	void M18SmokeGrenade_ColorBase()
	{
		SetAmmoType("");
		SetFuseDelay(2);
		SetSoundSmokeStart(SOUND_SMOKE_START);
		SetSoundSmokeLoop(SOUND_SMOKE_LOOP);
		SetSoundSmokeEnd(SOUND_SMOKE_END);
	}
	
	void ~M18SmokeGrenade_ColorBase() {}
}

class M18SmokeGrenade_Red extends M18SmokeGrenade_ColorBase
{
	void M18SmokeGrenade_Red()
	{
		SetParticleSmokeStart(ParticleList.GRENADE_M18_RED_START);
		SetParticleSmokeLoop(ParticleList.GRENADE_M18_RED_LOOP);
		SetParticleSmokeEnd(ParticleList.GRENADE_M18_RED_END);
	}
}

// Used for calling in dynamic contaminated areas
class M18SmokeGrenade_Red_Contaminated extends M18SmokeGrenade_Red
{
	override void OnWorkStop()
	{
		super.OnWorkStop();
		
		if ( GetGame().IsServer() )
			GetGame().GetCEApi().SpawnGroup( "ContaminatedArea_Dynamic", GetPosition() );
	}
}

class M18SmokeGrenade_Green extends M18SmokeGrenade_ColorBase
{
	void M18SmokeGrenade_Green()
	{
		SetParticleSmokeStart(ParticleList.GRENADE_M18_GREEN_START);
		SetParticleSmokeLoop(ParticleList.GRENADE_M18_GREEN_LOOP);
		SetParticleSmokeEnd(ParticleList.GRENADE_M18_GREEN_END);
	}
}

class M18SmokeGrenade_Yellow extends M18SmokeGrenade_ColorBase
{
	void M18SmokeGrenade_Yellow()
	{
		SetParticleSmokeStart(ParticleList.GRENADE_M18_YELLOW_START);
		SetParticleSmokeLoop(ParticleList.GRENADE_M18_YELLOW_LOOP);
		SetParticleSmokeEnd(ParticleList.GRENADE_M18_YELLOW_END);
	}	
}

class M18SmokeGrenade_Purple extends M18SmokeGrenade_ColorBase
{
	void M18SmokeGrenade_Purple()
	{
		SetParticleSmokeStart(ParticleList.GRENADE_M18_PURPLE_START);
		SetParticleSmokeLoop(ParticleList.GRENADE_M18_PURPLE_LOOP);
		SetParticleSmokeEnd(ParticleList.GRENADE_M18_PURPLE_END);
	}
}

class M18SmokeGrenade_White extends M18SmokeGrenade_ColorBase
{
	void M18SmokeGrenade_White()
	{
		SetParticleSmokeStart(ParticleList.GRENADE_M18_WHITE_START);
		SetParticleSmokeLoop(ParticleList.GRENADE_M18_WHITE_LOOP);
		SetParticleSmokeEnd(ParticleList.GRENADE_M18_WHITE_END);
	}
}
