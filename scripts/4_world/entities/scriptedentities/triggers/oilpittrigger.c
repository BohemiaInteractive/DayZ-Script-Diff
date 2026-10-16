class OilPitTrigger : EffectTrigger
{
	protected const string 	SOUND_BURNING 		= "oil_pit_burn_SoundSet";
	protected const float 	SOUND_AUDIBLE_RANGE = 200.0;	// max distance (m) at which the burning sound is audible/active
	protected const float 	SOUND_CHECK_RATE = 2.0;	// seconds between distance re-checks, to avoid a per-frame distance calc for every pit

	protected const float 	DAMAGE_TICK_RATE 	= 1.0;	// seconds between fire damage ticks
	protected const float 	DAMAGE_MULTIPLIER	= 20.0;	// damage per tick

	protected bool				m_IsBurning;
	protected bool				m_PlayerInRange;
	protected float				m_SoundCheckAccu;
	protected vector			m_DefaultPosition;
	protected EOilPitState		m_OilPitState = EOilPitState.INACTIVE;	// synchronized state

	protected ParticleSource	m_FireParticle;
	protected ParticleSource	m_SmokeParticle;
	protected EffectSound		m_SoundBurning;
	
	#ifndef SERVER
	#ifndef NO_GUI
	protected BonfireLight m_MainLight;
	#endif
	#endif

	void OilPitTrigger()
	{
		RegisterNetSyncVariableInt("m_OilPitState", 0, 1);
	}

	override void DeferredInit()
	{
		super.DeferredInit();

		m_DefaultPosition = GetAdjustedPosition();

		if (!g_Game.IsHeadlessOrDedicatedServer())
			UpdateState();
	}

	override string GetDisplayName()
	{
		return "#STR_oilpit";
	}

	override void EEDelete( EntityAI parent )
	{
		StopEffects();

		super.EEDelete(parent);
	}

	override void OnStayStartServerEvent(int nrOfInsiders)
	{
		if (!CheckState(EOilPitState.BURNING))
			return;

		m_TimeAccuStay += m_DeltaTime;
		if (m_TimeAccuStay > DAMAGE_TICK_RATE)
			m_DealDamageFlag = true;
	}

	override void OnStayServerEvent(TriggerInsider insider, float deltaTime)
	{
		if (!m_DealDamageFlag)
			return;

		EntityAI entity = EntityAI.Cast(insider.GetObject());
		if (entity)
			entity.ProcessDirectDamage(DamageType.CUSTOM, this, "", "HeatDamage", "0 0 0", DAMAGE_MULTIPLIER);
	}

	override void OnStayFinishServerEvent()
	{
		if (m_DealDamageFlag)
		{
			m_TimeAccuStay = 0;
			m_DealDamageFlag = false;
		}
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();

		if (IsInitialized())
			UpdateState();
	}

	protected void UpdateState()
	{
		if (CheckState(EOilPitState.BURNING) && !m_IsBurning)
		{
			#ifndef SERVER
			#ifndef NO_GUI
			if (!m_MainLight)
			{
				m_MainLight = BonfireLight.Cast(ScriptedLightBase.CreateLight(BonfireLight, GetPosition() + "0 2.5 0", 2.0));
				m_MainLight.FadeBrightnessTo(m_MainLight.m_Brightness, 0.0);
				m_MainLight.FadeRadiusTo(m_MainLight.m_FireRadius, 0.0);
			}
			#endif
			#endif
			
			Math.Randomize(-1);
			int random = Math.RandomIntInclusive(1, 10);
			if (random % 2)
			{
				m_FireParticle = ParticleManager.GetInstance().PlayInWorld(ParticleList.OIL_FIRE1, m_DefaultPosition);
				m_SmokeParticle = ParticleManager.GetInstance().PlayInWorld(ParticleList.BONFIRE_SMOKE, m_DefaultPosition);
			}
			else
			{
				m_FireParticle = ParticleManager.GetInstance().PlayInWorld(ParticleList.OIL_FIRE2, m_DefaultPosition);
				m_SmokeParticle = ParticleManager.GetInstance().PlayInWorld(ParticleList.SMOKE_GENERIC_WRECK, m_DefaultPosition);
			}

			UpdateSound();

			m_IsBurning = true;
		}
		else if (!CheckState(EOilPitState.BURNING) && m_IsBurning)
		{
			StopEffects();
		}
	}

	protected vector GetAdjustedPosition(float height = 0)
	{
		vector pos = GetPosition();
		pos[1] = g_Game.SurfaceRoadY(pos[0], pos[2], RoadSurfaceDetection.UNDER) + height;

		return pos;
	}

	void StopEffects()
	{
		if (m_IsBurning)
		{
			if (m_FireParticle)
				m_FireParticle.StopParticle();
			
			if (m_SmokeParticle)
				m_SmokeParticle.StopParticle();

			if (m_SoundBurning)
			{
				SEffectManager.DestroyEffect(m_SoundBurning);
				m_SoundBurning = null;
			}

			m_IsBurning = false;
		}
		
		#ifndef SERVER
		#ifndef NO_GUI
		if (m_MainLight)
		{
			m_MainLight.Destroy();
			m_MainLight = null;
		}
		#endif
		#endif
	}

	void AddState(EOilPitState state)
	{
		m_OilPitState |= state;
		SetSynchDirty();
	}

	void RemoveState(EOilPitState state)
	{
		m_OilPitState &= ~state;
		SetSynchDirty();
	}

	bool CheckState(EOilPitState state)
	{
		if (state == EOilPitState.INACTIVE)
			return (m_OilPitState == state);
		else
			return (m_OilPitState & state) != 0;
	}

	override void EOnFrame(IEntity other, float timeSlice)
	{
		super.EOnFrame(other, timeSlice);

		if (!g_Game.IsHeadlessOrDedicatedServer())
		{
			m_SoundCheckAccu += timeSlice;
			if (m_SoundCheckAccu >= SOUND_CHECK_RATE)
			{
				m_SoundCheckAccu = 0;
				UpdateSoundRange();
			}
		}
	}

	protected void UpdateSoundRange()
	{
		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
		if (player)
			m_PlayerInRange = (vector.DistanceSq(player.GetPosition(), m_DefaultPosition) <= SOUND_AUDIBLE_RANGE * SOUND_AUDIBLE_RANGE);
		else
			m_PlayerInRange = false;

		UpdateSound();
	}

	protected void UpdateSound()
	{
		if (CheckState(EOilPitState.BURNING) && m_PlayerInRange)
		{
			if (!m_SoundBurning)
				m_SoundBurning = SEffectManager.PlaySound(SOUND_BURNING, m_DefaultPosition, 0, 0, true);
		}
		else if (m_SoundBurning)
		{
			SEffectManager.DestroyEffect(m_SoundBurning);
			m_SoundBurning = null;
		}
	}

	EOilPitState GetState()
	{
		return m_OilPitState;
	}
}
