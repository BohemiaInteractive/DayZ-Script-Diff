class CrashBase extends House
{
	Particle m_ParticleEfx;
	static bool m_Init = Init();
	
	static bool Init()
	{
		CrashSoundSets.RegisterSoundSet("HeliCrash_Distant_SoundSet");
		CrashSoundSets.RegisterSoundSet("SledgeCrash_Distant_SoundSet");
		return true;
	}
	
	override void EEOnCECreate()
	{
		super.EEOnCECreate();
		RequestSoundEvent();
	}
	
	void RequestSoundEvent()
	{
		Param3<bool, vector, int> playSound = new Param3<bool, vector, int>(true, GetPosition(), GetSoundSet().Hash());
		GetGame().RPCSingleParam( null, ERPCs.RPC_SOUND_HELICRASH, playSound, true );
	}
	
	// needs to have the soundset registered in CrashBase.Init()
	string GetSoundSet()
	{
		return "HeliCrash_Distant_SoundSet";
	}

	override void EEInit()
	{
		super.EEInit();
		//Setup for local sound tests
		#ifdef DEVELOPER
		if ( !GetGame().IsMultiplayer() )
		{
			EffectSound eff = SEffectManager.PlaySound( GetSoundSet(), GetPosition(), 0.1, 0.1 );
			eff.SetAutodestroy(true);
		}
		#endif
	}

	override void EEDelete(EntityAI parent)
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			if ( m_ParticleEfx )
				m_ParticleEfx.Stop();
		}
	}
};