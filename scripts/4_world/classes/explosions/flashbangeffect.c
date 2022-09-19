class FlashbangEffect
{
	protected const float 						ALPHA_MIN = 0.0;
	protected const float 						ALPHA_MAX = 1.0;
	
	protected const float 						SOUND_DEFER_TIME = 0.4;			//! SFX will be played ~0.5s AFTER VFX 

	protected float 							m_HitDuration;
	protected float 							m_BreakPoint;
	protected float 							m_TimeActive;
	protected float								m_DayTimeToggle;
	
	protected float								m_AlphaMaxActual; //actual max alpha of the effect
	protected float								m_SoundMaxActual; //actual max volume of the sound
	protected float								m_ProgressMultiplier; 
	
	protected bool								m_Visual;
	protected bool 								m_Initialized;
	
	protected PlayerBase 						m_Player;
	protected EffectSound 						m_FlashbangEffectSound;
	protected float 							m_SoundStopTime;
	
	protected ref Timer							m_DeferAttenuation;
	
	protected PPERequester_FlashbangEffects 	m_Requester;
	
	void FlashbangEffect(PlayerBase player, bool visual = true)
	{
		m_Player = player;
		m_Visual = visual;
		m_Initialized = false;

		m_HitDuration = 8.0;
		m_BreakPoint = 2.5;
		m_AlphaMaxActual = ALPHA_MAX;
		m_SoundMaxActual = 1.0;
		m_ProgressMultiplier = 1.0;
		
		m_FlashbangEffectSound = null;
		
		if (m_Visual)
		{
			Class.CastTo(m_Requester,PPERequesterBank.GetRequester(PPERequester_FlashbangEffects));
			m_Requester.Start();
		}
		
		m_DeferAttenuation = new ref Timer();
		m_DeferAttenuation.Run(SOUND_DEFER_TIME, this, "PlaySound", null, false);
		
		//! naive time of the day selector
		m_DayTimeToggle = 5; //! -1: night; 1: day
		if ( g_Game.GetDayTime() >= 22.0 || g_Game.GetDayTime() < 7.0 )
		{
			m_DayTimeToggle = 10;
		}
	}
	
	void ~FlashbangEffect()
	{
		if ( m_Visual )
		{
			ClearVisual();
		}
		
		if ( m_Player )
		{
			m_Player.OnPlayerReceiveFlashbangHitEnd();
		}

		if ( m_DeferAttenuation.IsRunning() )
		{
			m_DeferAttenuation.Stop();
		}
		
		m_DeferAttenuation = null;
		SEffectManager.DestroyEffect(m_FlashbangEffectSound);
	}
	
	void SetupFlashbangValues(float progress_mult = 1.0, float visual_value_max = 1.0, float sound_value_max = 1.0)
	{
		if ( !m_Initialized )
		{
			m_Initialized = true;
			m_ProgressMultiplier = progress_mult;
			m_AlphaMaxActual = visual_value_max;
			m_SoundMaxActual = sound_value_max;
			
			m_HitDuration *= m_ProgressMultiplier;
			m_BreakPoint *= m_ProgressMultiplier;
		}
	}
	
	protected void PlaySound()
	{
		if ( !m_Initialized )
		{
			Error("" + this + " not initialized");
			return;
		}
		
		vector pos;
		MiscGameplayFunctions.GetHeadBonePos(m_Player, pos);
		
		if (!m_FlashbangEffectSound)
		{
			m_FlashbangEffectSound = SEffectManager.CreateSound("Tinnitus_SoundSet", pos);
		}
		
		if (!m_FlashbangEffectSound.IsPlaying())
		{
			m_FlashbangEffectSound.SetParent(m_Player);
			m_FlashbangEffectSound.SetAttachedLocalPos(m_Player.WorldToModel(pos));
			m_FlashbangEffectSound.SetSoundWaveKind(WaveKind.WAVEEFFECTEX);
			m_FlashbangEffectSound.SetSoundFadeIn(4 * Math.Clamp(m_ProgressMultiplier,0.5,1.0)); //TODO
			m_SoundStopTime = 2 * Math.Clamp(m_ProgressMultiplier,0.5,1.0);
			m_FlashbangEffectSound.SetSoundFadeOut(m_SoundStopTime); //TODO
			m_FlashbangEffectSound.SetSoundMaxVolume(Math.Clamp(m_SoundMaxActual,0.1,1.0)); //TODO
			m_FlashbangEffectSound.SetSoundLoop(true);
			m_FlashbangEffectSound.SoundPlay();
			
			SetAttenuationFilter();
		}
	}

	protected void SetAttenuationFilter()
	{
		if ( !m_DeferAttenuation.IsRunning() || m_Player.GetMasterAttenuation() != "FlashbangAttenuation" )
		{
			m_Player.SetMasterAttenuation("FlashbangAttenuation");
		}
	}
	
	protected void ResetAttenuationFilter()
	{
		m_Player.SetMasterAttenuation("");
	}
	
	protected void StopSound()
	{
		if (m_FlashbangEffectSound)
		{
			m_FlashbangEffectSound.SoundStop();
		}
	}
	
	protected void ClearVisual()
	{
		if (m_Requester)
		{
			m_Requester.Stop();
		}
	}
	
	protected void SetVisual(float val)
	{
		if (m_Requester && m_Requester.IsRequesterRunning())
		{
			m_Requester.SetFlashbangIntensity(val, m_DayTimeToggle);
		}
	}
	
	void Stop()
	{
		StopSound();
	}
	
	void Update(float deltatime)
	{
		if ( !m_Initialized )
		{
			Error("" + this + " not initialized");
		}
		else if ( m_Visual )
		{
			float value;

			if ( m_TimeActive <= m_BreakPoint )
			{
				value = m_AlphaMaxActual;
				//Print("Flashbango | m_AlphaMaxActual: " + value);
			}
			else
			{
				value = Math.InverseLerp(m_HitDuration - m_BreakPoint, m_HitDuration, m_TimeActive);
				value = Math.Clamp(value,0.0,1.0);
				value = m_AlphaMaxActual - value * m_AlphaMaxActual;
				//Print("Flashbango | tmp_value: " + value);
			}
			SetVisual(value);
		}

		m_TimeActive += deltatime;

		if (m_TimeActive >= m_HitDuration - m_SoundStopTime)
		{
			StopSound();
		}
		
		if (m_TimeActive >= m_HitDuration)
		{
			ResetAttenuationFilter();
			delete this;
		}
	}
}