class GeyserTrigger : EffectTrigger
{
	const float 	MOUTH_ADJUST_RADIUS 		= 0.2; // maxium radius geyser effect can move when using randomized position adjustment
	const string 	SOUND_BUBBLING 				= "Geyser_bubbling_loop_SoundSet";
	const string 	SOUND_ERUPTION 				= "Geyser_eruption_loop_SoundSet";
	const string 	SOUND_ERUPTION_START 		= "Geyser_eruption_start_SoundSet";
	const string 	SOUND_ERUPTION_TALL 		= "Geyser_eruption_tall_loop_SoundSet";
	const string 	SOUND_ERUPTION_TALL_START 	= "Geyser_eruption_tall_start_SoundSet";
	const string 	SOUND_ERUPTION_TALL_END  	= "Geyser_eruption_tall_splash_SoundSet";
		
	protected bool 				m_bIsDormant;
	protected bool 				m_bIsEruptingSoon;
	protected bool 				m_bIsErupting;
	protected bool 				m_bIsEruptingTall;
	protected float				m_AdjustedX;							//deprecated
	protected float 			m_AdjustedY;							//deprecated
	protected vector 			m_DefaultPosition;
	protected EGeyserState 		m_GeyserState = EGeyserState.DORMANT;	// synchronized state
	
	protected ParticleSource 	m_GeyserBubblesParticle;
	protected ParticleSource 	m_GeyserParticle;
	protected ParticleSource 	m_GeyserTallParticle;
	protected ParticleSource 	m_GeyserSplashParticle;
	protected EffectSound		m_SoundBubbling;
	protected EffectSound 		m_SoundEruption;
	protected EffectSound 		m_SoundEruptionStart;
	protected EffectSound 		m_SoundEruptionSecondary;
	protected EffectSound 		m_SoundEruptionSecondaryStart;
	protected EffectSound 		m_SoundEruptionSecondaryEnd;

	void GeyserTrigger()
	{
		RegisterNetSyncVariableInt("m_GeyserState", 0, 32);
	}
	
	override void DeferredInit()
	{
		super.DeferredInit();
		
		m_DefaultPosition = GetAdjustedPosition();
		
		if (IsSubmerged())
			RandomizeMouthPos();
		
		if (!g_Game.IsDedicatedServer())
			UpdateGeyserState();	
	}
	
	override string GetDisplayName()
	{
		return "#STR_geyser";
	}
	
	override void EEDelete( EntityAI parent )
	{
		StopEffects();
		
		super.EEDelete(parent);
	}
	
	override void OnEnterServerEvent( TriggerInsider insider )
	{
		super.OnEnterServerEvent(insider);
				
		if (insider && (m_GeyserState & EGeyserState.ERUPTING_PRIMARY))
		{
			EntityAI entity = EntityAI.Cast(insider.GetObject());
			if (entity)
				entity.ProcessDirectDamage(DamageType.CUSTOM, this, "", "HeatDamage", "0 0 0", 1000);
		}
	}
	
	override void OnLeaveServerEvent( TriggerInsider insider )
	{
		super.OnLeaveServerEvent(insider);
	}
	
	override void OnEnterClientEvent( TriggerInsider insider )
	{
		super.OnEnterClientEvent(insider);
	}
	
	override void OnLeaveClientEvent( TriggerInsider insider )
	{
		super.OnLeaveClientEvent(insider);
	}
	
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		
		if (IsInitialized())
			UpdateGeyserState();
	}
	
	// Updated from OnVariablesSynchronized
	protected void UpdateGeyserState()
	{
		// Debug.Log("UpdateGeyserState, state: " + m_GeyserState);
		
		if (CheckGeyserState(EGeyserState.DORMANT) && !m_bIsDormant)
		{					
			m_bIsDormant = true;
		}
		else if (!CheckGeyserState(EGeyserState.DORMANT) && m_bIsDormant)
		{
			m_bIsDormant = false;
		}
		
		if (CheckGeyserState(EGeyserState.ERUPTION_SOON) && !m_bIsEruptingSoon)
		{								
			m_GeyserBubblesParticle = ParticleManager.GetInstance().PlayInWorld(ParticleList.GEYSER_BUBBLES, m_DefaultPosition);
			m_SoundBubbling = SEffectManager.PlaySound(SOUND_BUBBLING, m_DefaultPosition, 2, 2, true);
			
			m_bIsEruptingSoon = true;
		}
		else if (!CheckGeyserState(EGeyserState.ERUPTION_SOON) && m_bIsEruptingSoon)
		{
			if (m_GeyserBubblesParticle)
				m_GeyserBubblesParticle.StopParticle();
			if (m_SoundBubbling)
				SEffectManager.DestroyEffect(m_SoundBubbling);
			
			m_bIsEruptingSoon = false;
		}
		
		if (CheckGeyserState(EGeyserState.ERUPTING_PRIMARY) && !m_bIsErupting)
		{
			m_GeyserParticle = ParticleManager.GetInstance().PlayInWorld(ParticleList.GEYSER_NORMAL, m_DefaultPosition);
			
			m_SoundEruptionStart = SEffectManager.PlaySound(SOUND_ERUPTION_START, m_DefaultPosition, 0, 0, false);
			m_SoundEruption = SEffectManager.PlaySound(SOUND_ERUPTION, m_DefaultPosition, 2, 2, true);
			
			m_bIsErupting = true;
		}
		else if (!CheckGeyserState(EGeyserState.ERUPTING_PRIMARY) && m_bIsErupting)
		{
			if (m_GeyserParticle)
				m_GeyserParticle.StopParticle();
			
			if (m_SoundEruptionStart)
				SEffectManager.DestroyEffect(m_SoundEruptionStart);
			if (m_SoundEruption)
				SEffectManager.DestroyEffect(m_SoundEruption);
			
			m_bIsErupting = false;
		}
		
		if (CheckGeyserState(EGeyserState.ERUPTING_SECONDARY) && !m_bIsEruptingTall)
		{
			m_GeyserTallParticle = ParticleManager.GetInstance().PlayInWorld(ParticleList.GEYSER_STRONG, m_DefaultPosition);
			
			m_SoundEruptionSecondaryStart = SEffectManager.PlaySound(SOUND_ERUPTION_TALL_START, m_DefaultPosition, 0, 0, false);
			m_SoundEruptionSecondary = SEffectManager.PlaySound(SOUND_ERUPTION_TALL, m_DefaultPosition, 0, 0, false);
			
			m_bIsEruptingTall = true;
		}
		else if (!CheckGeyserState(EGeyserState.ERUPTING_SECONDARY) && m_bIsEruptingTall)
		{
			m_GeyserSplashParticle = ParticleManager.GetInstance().PlayInWorld(ParticleList.GEYSER_SPLASH, m_DefaultPosition);
			m_SoundEruptionSecondaryEnd = SEffectManager.PlaySound(SOUND_ERUPTION_TALL_END, m_DefaultPosition, 0, 0, false);
			
			if (m_GeyserTallParticle)
				m_GeyserTallParticle.StopParticle();
			if (m_SoundEruptionSecondaryStart)
				SEffectManager.DestroyEffect(m_SoundEruptionSecondaryStart);
			if (m_SoundEruptionSecondary)
				SEffectManager.DestroyEffect(m_SoundEruptionSecondary);
			
			m_bIsEruptingTall = false;
		}
	}
	
	// Slightly adjust position of geyser particles
	protected void RandomizeMouthPos()
	{
		m_DefaultPosition[0] = m_DefaultPosition[0] + Math.RandomFloat( -MOUTH_ADJUST_RADIUS, MOUTH_ADJUST_RADIUS);
		m_DefaultPosition[2] = m_DefaultPosition[2] + Math.RandomFloat( -MOUTH_ADJUST_RADIUS, MOUTH_ADJUST_RADIUS);
		
		//for backwards compatibility
		m_AdjustedX = m_DefaultPosition[0];							
		m_AdjustedY = m_DefaultPosition[2]; //typo, should be Z
	}
	
	// Get position on surface of water volume or terrain, add height offset
	protected vector GetAdjustedPosition(float height = 0)
	{
		vector pos = GetPosition();
		pos[1] = g_Game.SurfaceRoadY(pos[0], pos[2], RoadSurfaceDetection.UNDER) + height;
		
		return pos;
	}
	
	void StopEffects()
	{
		if (m_bIsEruptingSoon)
		{
			if (m_GeyserBubblesParticle)
				m_GeyserBubblesParticle.StopParticle();
			if (m_SoundBubbling)
				SEffectManager.DestroyEffect(m_SoundBubbling);
			m_bIsEruptingSoon = false;
		}
		
		if (m_bIsErupting)
		{
			if (m_GeyserParticle)
				m_GeyserParticle.StopParticle();
			if (m_SoundEruption)
				SEffectManager.DestroyEffect(m_SoundEruption);
			m_bIsErupting = false;
		}
		
		if (m_bIsEruptingTall)
		{
			if (m_GeyserTallParticle)
				m_GeyserTallParticle.StopParticle();
			if (m_SoundEruptionSecondary)
				SEffectManager.DestroyEffect(m_SoundEruptionSecondary);
			m_bIsEruptingTall = false;
		}
	}
	
	void AddGeyserState(EGeyserState state)
	{
		m_GeyserState |= state;
		SetSynchDirty();
	}
	
	void RemoveGeyserState(EGeyserState state)
	{
		m_GeyserState &= ~state;
		SetSynchDirty();
	}
	
	bool CheckGeyserState(EGeyserState state)
	{
		if (state == EGeyserState.DORMANT)
			return (m_GeyserState == state);
		else
			return (m_GeyserState & state);
	}

	EGeyserState GetGeyserState()
	{
		return m_GeyserState;
	}
	
	// override for differences in logic between land & submerged geysers
	bool IsSubmerged()
	{
		return true;
	}
	
	
}