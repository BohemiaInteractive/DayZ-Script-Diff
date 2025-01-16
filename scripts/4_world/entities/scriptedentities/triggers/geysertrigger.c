class GeyserTrigger : EffectTrigger
{
	const float 	MOUTH_ADJUST_RADIUS = 0.2; // maxium radius geyser effect can move when using randomized position adjustment
	const string 	SOUND_BUBBLING 		= "Boiling_Done_SoundSet";
	const string 	SOUND_ERUPTION 		= "pour_Water_Pot_SoundSet";
	const string 	SOUND_ERUPTION_TALL = "pour_Water_WatterBottle_SoundSet";
		
	protected bool 				m_bIsDormant;
	protected bool 				m_bIsErupting;
	protected bool 				m_bIsEruptingTall;
	protected float				m_AdjustedX;
	protected float 			m_AdjustedY;
	protected vector 			m_DefaultPosition;
	protected EGeyserState 		m_GeyserState = EGeyserState.DORMANT;	// synchronized state
	
	protected ParticleSource 	m_GeyserBubblesParticle;
	protected ParticleSource 	m_GeyserParticle;
	protected ParticleSource 	m_GeyserTallParticle;
	protected EffectSound		m_SoundBubbling;
	protected EffectSound 		m_SoundEruption;
	protected EffectSound 		m_SoundEruptionSecondary;
		
	void GeyserTrigger()
	{
		RegisterNetSyncVariableInt("m_GeyserState", 0, 32);
	}
	
	override void EEInit()
	{
		super.EEInit();
		
		m_DefaultPosition = GetPosition();
		m_DefaultPosition[1] = GetGame().SurfaceY(m_DefaultPosition[0], m_DefaultPosition[2]);
		
		m_bIsDormant = false;
		m_bIsErupting = false;
		m_bIsEruptingTall = false;
		
		RandomizeMouthPos();
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
		
		UpdateGeyserState();
	}
	
	// Updated from OnVariablesSynchronized
	protected void UpdateGeyserState()
	{
		if (m_GeyserState == EGeyserState.DORMANT && !m_bIsDormant)
		{			
			if (IsSubmerged())
			{
				RandomizeMouthPos();
				m_GeyserBubblesParticle = ParticleManager.GetInstance().PlayInWorld(ParticleList.GEYSER_BUBBLES, GetAdjustedPosition());
				m_SoundBubbling = SEffectManager.PlaySound(SOUND_BUBBLING, GetAdjustedPosition(), 0, 0, true);
			}
			
			m_bIsDormant = true;
		}
		else if (m_GeyserState != EGeyserState.DORMANT && m_bIsDormant)
		{
			m_GeyserBubblesParticle.Stop();
			m_SoundBubbling.Stop();
			m_bIsDormant = false;
		}
		
		if ((m_GeyserState & EGeyserState.ERUPTING_PRIMARY) && !m_bIsErupting)
		{
			vector posErupt = GetAdjustedPosition(3);
			m_GeyserParticle = ParticleManager.GetInstance().PlayInWorld(ParticleList.GEYSER_NORMAL, posErupt);

			m_SoundEruption = SEffectManager.PlaySound(SOUND_ERUPTION, posErupt, 0, 0, true);
			
			m_bIsErupting = true;
		}
		else if (((m_GeyserState & EGeyserState.ERUPTING_PRIMARY) == 0) && m_bIsErupting)
		{
			m_GeyserParticle.StopParticle();
			m_SoundEruption.Stop();
			m_bIsErupting = false;
		}
		
		if ((m_GeyserState & EGeyserState.ERUPTING_SECONDARY) && !m_bIsEruptingTall)
		{
			vector posTall = GetAdjustedPosition(5);
			m_GeyserTallParticle = ParticleManager.GetInstance().PlayInWorld(ParticleList.GEYSER_STRONG, posTall);

			m_SoundEruptionSecondary = SEffectManager.PlaySound(SOUND_ERUPTION_TALL, posTall, 0, 0, true);
			
			m_bIsEruptingTall = true;
		}
		else if (((m_GeyserState & EGeyserState.ERUPTING_SECONDARY) == 0) && m_bIsEruptingTall)
		{
			m_GeyserTallParticle.StopParticle();
			m_SoundEruptionSecondary.Stop();
			m_bIsEruptingTall = false;
		}
	}
	
	// Slightly adjust position of geyser particles between eruptions
	protected void RandomizeMouthPos()
	{
		m_AdjustedX = Math.RandomFloat(m_DefaultPosition[0] - MOUTH_ADJUST_RADIUS, m_DefaultPosition[0] + MOUTH_ADJUST_RADIUS);
		m_AdjustedY = Math.RandomFloat(m_DefaultPosition[2] - MOUTH_ADJUST_RADIUS, m_DefaultPosition[2] + MOUTH_ADJUST_RADIUS);
	}
	
	protected vector GetAdjustedPosition(float height = 0)
	{
		vector pos = m_DefaultPosition;
		pos[1] = pos[1] + height;
		
		if (IsSubmerged())
		{
			pos[0] = m_AdjustedX;
			pos[2] = m_AdjustedY;
		}
		
		return pos;
	}
	
	void StopEffects()
	{
		if (m_bIsDormant)
		{
			m_GeyserBubblesParticle.StopParticle();
			m_SoundBubbling.Stop();
			m_bIsDormant = false;
		}
		
		if (m_bIsErupting)
		{
			m_GeyserParticle.StopParticle();
			m_SoundEruption.Stop();
			m_bIsErupting = false;
		}
		
		if (m_bIsEruptingTall)
		{
			m_GeyserTallParticle.StopParticle();
			m_SoundEruptionSecondary.Stop();
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