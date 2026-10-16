class SilicosisMdfr : ModifierBase
{
	private const int STAMINA_LIMIT_TRESHOLD 	= 600;
	private const int HEALTH_THRESHOLD 			= 700;
	private const int UNCON_THRESHOLD 			= 800;
	
	//! Unconsciousness
	protected static const float UNCONSCIOUSNESS_LENGTH_MIN = 5.0;
	protected static const float UNCONSCIOUSNESS_LENGTH_MAX = 10.0;
	protected static const float UNCONSCIOUSNESS_EVENT_INTERVAL_MIN = 60.0;
	protected static const float UNCONSCIOUSNESS_EVENT_INTERVAL_MAX = 120.0;
	
	private const float HEALTH_LOSS	= 2.0;
	
	private const int HEAVY_BREATH_TRESHOLD_MIN		= 200; //! min #num of agents
	private const float HEAVY_BREATH_INTERVAL_MIN 	= 3.0; //! seconds
	private const float HEAVY_BREATH_INTERVAL_MAX 	= 25.0; //! seconds
	
	protected bool m_CanFaintEffectFadeIn, m_CanFaintEffectFadeOut, m_FaintEffectActive;
	protected float m_FaintEffectTime, m_UnconciousnessActiveTimer, m_UnconciousnessInactiveTimer;
	protected float m_NextUnconsciousnessEvent;
	protected float m_HeavyBreathTimer;
	
	private float m_AgentsCountPrevious;
	private bool m_StaminaLimitation;
	private ref SymptomBase m_FaintSymptom;

	override void Init()
	{
		m_TrackActivatedTime	= false;
		m_ID 					= eModifiers.MDF_PARTICLES_BREATH;
		m_TickIntervalInactive 	= DEFAULT_TICK_TIME_INACTIVE_LONG;
		m_TickIntervalActive 	= 5;		
	}
	
	override protected bool ActivateCondition(PlayerBase player)
	{
		return (player.GetSingleAgentCount(eAgents.SILICOSIS) > 0);
	}

	override bool DeactivateCondition(PlayerBase player)
	{
		return (player.GetSingleAgentCount(eAgents.SILICOSIS) == 0);
	}

	override void OnActivate(PlayerBase player)
	{
		m_CanFaintEffectFadeIn 	= true;

		m_FaintEffectTime 				= 0.0;
		m_UnconciousnessActiveTimer 	= 0.0;
		m_UnconciousnessInactiveTimer 	= 0.0;
		
		m_NextUnconsciousnessEvent = Math.RandomFloatInclusive(UNCONSCIOUSNESS_EVENT_INTERVAL_MIN, UNCONSCIOUSNESS_EVENT_INTERVAL_MAX);
		
		m_FaintSymptom = player.GetSymptomManager().QueueUpSecondarySymptomEx(SymptomIDs.SYMPTOM_FAINT);
		if (m_FaintSymptom)
		{
			Param2<int, ref SymptomSyncData> params = new Param2<int, ref SymptomSyncData>(0, null);

			SymptomSyncData faintData = new SymptomSyncData();
			params.param1 = SymptomIDs.SYMPTOM_FAINT;
			faintData.m_FloatArray.Insert(0.0);
			faintData.m_FloatArray.Insert(0.0);
			faintData.m_FloatArray.Insert(0.0);
			faintData.m_IntArray.Insert(1);
			params.param2 = faintData;

			m_FaintSymptom.SetParam(params);
		}
	}

	override void OnDeactivate(PlayerBase player)
	{
		m_StaminaLimitation = false;
	}

	override void OnTick(PlayerBase player, float deltaT)
	{
		#ifdef DEVELOPER
		if(!player.GetCanBeDestroyed())
			return;
		#endif
		
		float agentsCount 		= player.GetSingleAgentCount(eAgents.SILICOSIS);
		float agentsCountDiff 	= agentsCount - m_AgentsCountPrevious;

		//! heavy breathing
		{
			float heavyBreathIntensity = Math.Remap(HEAVY_BREATH_TRESHOLD_MIN, 1000, 0.1, 1.0, agentsCount);
			float heavyBreathInterval = Math.Lerp(HEAVY_BREATH_INTERVAL_MAX, HEAVY_BREATH_INTERVAL_MIN, heavyBreathIntensity);
			
			if (heavyBreathIntensity > 0.0)
				m_HeavyBreathTimer += deltaT;
			else
				m_HeavyBreathTimer = 0.0;
	
			if (m_HeavyBreathTimer >= heavyBreathInterval)
			{
				m_HeavyBreathTimer = 0.0;
				
				Math.Randomize(Math.RandomInt(1, 1000));
				float rand = Math.RandomFloat01();
				if (rand <= heavyBreathIntensity)
					player.RequestSoundEventEx(EPlayerSoundEventID.HEAVY_BREATHING, true, EPlayerSoundEventParam.HIGHEST_PRIORITY);
			}
		}

		//! stamina limitation
		//! TODO@RR - change it to separate recovery/depletion modifier ID
		{
			if (agentsCount >= STAMINA_LIMIT_TRESHOLD && !m_StaminaLimitation)
			{
				player.GetStaminaHandler().ActivateRecoveryModifier(EStaminaMultiplierTypes.DISEASE_PNEUMONIA);
				player.GetStaminaHandler().ActivateDepletionModifier(EStaminaMultiplierTypes.DISEASE_PNEUMONIA);
				m_StaminaLimitation = true;
			}
			else if (agentsCount < STAMINA_LIMIT_TRESHOLD && m_StaminaLimitation)
			{
				player.GetStaminaHandler().DeactivateRecoveryModifier(EStaminaMultiplierTypes.DISEASE_PNEUMONIA);
				player.GetStaminaHandler().DeactivateDepletionModifier(EStaminaMultiplierTypes.DISEASE_PNEUMONIA);
				m_StaminaLimitation = false;
			}
		}
		
		Param2<int, ref SymptomSyncData> params = new Param2<int, ref SymptomSyncData>(0, null);
		SymptomSyncData faintData = new SymptomSyncData();
		if (!player.IsUnconscious())
		{
			m_FaintEffectTime += deltaT;
		
			//! health
			if (agentsCount > HEALTH_THRESHOLD && agentsCountDiff > 0)
			{
				float healthLoss = HEALTH_LOSS * Math.Remap(HEALTH_THRESHOLD, UNCON_THRESHOLD, 0.0, 1.0, agentsCount);
				player.AddHealth(-healthLoss);
			}
	
			//! fainting
			float vignette = Math.Remap(UNCON_THRESHOLD, 1000, 0.0, 2.0, agentsCount);
			float fadeOut = Math.Remap(UNCON_THRESHOLD, 1000, 0.0, 5.0, agentsCount);
			float interval = Math.Remap(1000, UNCON_THRESHOLD, 0.0, 10.0, agentsCount) + fadeOut;

			if (m_FaintEffectTime > interval && m_CanFaintEffectFadeIn)
			{
				params.param1 = SymptomIDs.SYMPTOM_FAINT;
				faintData.m_FloatArray.Insert(vignette);
				faintData.m_FloatArray.Insert(fadeOut);
				faintData.m_FloatArray.Insert(interval);
				faintData.m_IntArray.Insert(1);
				params.param2 = faintData;
	
				m_FaintSymptom.SetParam(params);

				m_FaintEffectTime 		= 0;
				m_FaintEffectActive 	= true;
				m_CanFaintEffectFadeIn 	= false;
				m_CanFaintEffectFadeOut = true;
			}
			else if (m_FaintEffectTime > interval && m_CanFaintEffectFadeOut)
			{
				params.param1 = SymptomIDs.SYMPTOM_FAINT;
				faintData.m_FloatArray.Insert(vignette);
				faintData.m_FloatArray.Insert(fadeOut);
				faintData.m_FloatArray.Insert(interval);
				faintData.m_IntArray.Insert(0);
				params.param2 = faintData;

				m_FaintSymptom.SetParam(params);

				m_FaintEffectTime 		= 0;				
				m_CanFaintEffectFadeIn 	= true;
				m_CanFaintEffectFadeOut = false;
			}
		}
		else
		{
			if (m_FaintEffectActive)
			{
				m_FaintEffectActive = false;

				params.param1 = SymptomIDs.SYMPTOM_FAINT;
				faintData.m_FloatArray.Insert(vignette);
				faintData.m_FloatArray.Insert(fadeOut);
				faintData.m_FloatArray.Insert(interval);
				faintData.m_IntArray.Insert(2);
				params.param2 = faintData;

				m_FaintSymptom.SetParam(params);
			}
		}

		//! unconsciousness
		{
			if (agentsCount > UNCON_THRESHOLD)
			{
				m_UnconciousnessActiveTimer		+= deltaT;
				m_UnconciousnessInactiveTimer	+= deltaT;
			}

			bool canTriggerUnconsciousness = !m_CanFaintEffectFadeOut;
			if ((agentsCount > UNCON_THRESHOLD && canTriggerUnconsciousness) || player.IsUnconscious())
			{
				float unconsciousnessLength = Math.RandomFloatInclusive(UNCONSCIOUSNESS_LENGTH_MIN, UNCONSCIOUSNESS_LENGTH_MAX);
				if (m_UnconciousnessInactiveTimer >= m_NextUnconsciousnessEvent && !player.IsUnconscious())
				{
					player.RequestUnconsciousness(true);
					player.SetHealth("", "Shock", 25.0);
					m_UnconciousnessInactiveTimer = 0;
					m_UnconciousnessActiveTimer = 0;
					m_NextUnconsciousnessEvent = Math.RandomFloatInclusive(UNCONSCIOUSNESS_EVENT_INTERVAL_MIN, UNCONSCIOUSNESS_EVENT_INTERVAL_MAX);
				}
				else if (m_UnconciousnessActiveTimer > unconsciousnessLength && player.IsUnconscious())
				{
					player.RequestUnconsciousness(false);
					m_UnconciousnessInactiveTimer = 0;
					m_UnconciousnessActiveTimer = 0;
				}
			}
		}

		m_AgentsCountPrevious = agentsCount;
	}
}
