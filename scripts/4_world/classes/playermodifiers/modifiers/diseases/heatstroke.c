class HeatStrokeMdfr : ModifierBase
{
	const int NUMBER_OF_STAGES = 4;
	const float STAGE_THRESHOLDS[NUMBER_OF_STAGES] = {0.0, 0.4, 0.6, 0.85};
	
	//! Fever setup
	protected static const int WATER_DRAIN_FROM_FEVER_ST1 	= 0.35; //! per tick
	protected static const int WATER_DRAIN_FROM_FEVER_ST2 	= 0.5; //! per tick
	protected static const int WATER_DRAIN_FROM_FEVER_ST3 	= 0.75; //! per tick
	protected static const int ENERGY_DRAIN_FROM_FEVER 	= 1; //! per tick
	
	//! Vomit setup
	protected static const int VOMIT_OCCURRENCES_MIN 	= 1;
	protected static const int VOMIT_OCCURRENCES_MAX 	= 2;
	protected static const int WATER_DRAIN_FROM_VOMIT 	= 70; //! one time
	protected static const int ENERGY_DRAIN_FROM_VOMIT 	= 55; //! one time
	
	//! Unconsciousness
	protected static const float UNCONSCIOUSNESS_LENGTH_MIN = 5.0;
	protected static const float UNCONSCIOUSNESS_LENGTH_MAX = 10.0;
	protected static const float UNCONSCIOUSNESS_EVENT_INTERVAL_MIN = 60.0;
	protected static const float UNCONSCIOUSNESS_EVENT_INTERVAL_MAX = 120.0;
	
	protected static const float SHOCK_ACTIVATION_THRESHOLD_MIN = 90.0;
	
	//! Health damage
	protected static const float HEALTH_DAMAGE_MIN = 0.01;
	protected static const float HEALTH_DAMAGE_MAX = 0.35;

	override void Init()
	{
		super.Init();

		m_TrackActivatedTime	= false;
		m_AnalyticsStatsEnabled = true;
		m_TickIntervalInactive 	= DEFAULT_TICK_TIME_INACTIVE;
		m_TickIntervalActive 	= DEFAULT_TICK_TIME_ACTIVE;
		m_SyncID				= eModifierSyncIDs.MODIFIER_SYNC_HEAT_STROKE;
	}
}

class HeatStrokePhase1Mdfr : HeatStrokeMdfr
{
	private static const float THRESHOLD_ACTIVATE = HeatStrokeMdfr.STAGE_THRESHOLDS[0];
	
	override void Init()
	{
		super.Init();

		m_ID = eModifiers.MDF_HEAT_STROKE1;
	}

	override string GetDebugText()
	{
		return ("Activate threshold >: "+ THRESHOLD_ACTIVATE + "| Deativate threshold: ==" + THRESHOLD_ACTIVATE);
	}
	
	override protected bool ActivateCondition(PlayerBase player)
	{
		return player.GetThermalBiasHandler().Get() > THRESHOLD_ACTIVATE;
	}

	override protected bool DeactivateCondition(PlayerBase player)
	{
		return player.GetThermalBiasHandler().Get() <= THRESHOLD_ACTIVATE;
	}
	
	override protected void OnActivate(PlayerBase player)
	{
	}
	
	override protected void OnDeactivate(PlayerBase player)
	{
		player.DecreaseDiseaseCount();
	}
	
	override protected void OnTick(PlayerBase player, float deltaT)
	{
		PlayerStat<float> waterStat 	= player.GetStatWater();
		waterStat.Add(-WATER_DRAIN_FROM_FEVER_ST1 * deltaT);
	}
}

class HeatStrokePhase2Mdfr : HeatStrokeMdfr
{
	private static const float THRESHOLD_ACTIVATE = HeatStrokeMdfr.STAGE_THRESHOLDS[1];
	
	override void Init()
	{
		super.Init();

		m_ID = eModifiers.MDF_HEAT_STROKE2;
	}

	override string GetDebugText()
	{
		return ("Activate threshold >: "+ THRESHOLD_ACTIVATE + "| Deativate threshold: ==" + THRESHOLD_ACTIVATE);
	}
	
	override protected bool ActivateCondition(PlayerBase player)
	{
		return player.GetThermalBiasHandler().Get() > THRESHOLD_ACTIVATE;
	}

	override protected bool DeactivateCondition(PlayerBase player)
	{
		return player.GetThermalBiasHandler().Get() <= THRESHOLD_ACTIVATE;
	}
	
	override protected void OnActivate(PlayerBase player)
	{
		player.GetSymptomManager().QueueUpSecondarySymptom(SymptomIDs.SYMPTOM_FEVERBLUR);
		player.GetStaminaHandler().ActivateRecoveryModifier(EStaminaMultiplierTypes.DISEASE_PNEUMONIA);
		player.GetStaminaHandler().ActivateDepletionModifier(EStaminaMultiplierTypes.DISEASE_PNEUMONIA);
		player.IncreaseDiseaseCount();
	}
	
	override protected void OnDeactivate(PlayerBase player)
	{
		player.GetSymptomManager().RemoveSecondarySymptom(SymptomIDs.SYMPTOM_FEVERBLUR);
		player.GetStaminaHandler().DeactivateRecoveryModifier(EStaminaMultiplierTypes.DISEASE_PNEUMONIA);
		player.GetStaminaHandler().DeactivateDepletionModifier(EStaminaMultiplierTypes.DISEASE_PNEUMONIA);
	}
	
	override protected void OnTick(PlayerBase player, float deltaT)
	{
		float thermalBias	= player.GetThermalBiasHandler().Get();	
	
		float chance = Math.Lerp(2, 10, thermalBias);
		chance = (chance / 500) * deltaT;
		if (Math.RandomFloat01() < chance)
			player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_HOT);
		
		PlayerStat<float> waterStat 	= player.GetStatWater();
		waterStat.Add(-WATER_DRAIN_FROM_FEVER_ST2 * deltaT);
	}
}

//! phase3 stays active together with phase4
class HeatStrokePhase3Mdfr : HeatStrokeMdfr
{
	private static const float THRESHOLD_ACTIVATE = HeatStrokeMdfr.STAGE_THRESHOLDS[2];

	protected bool m_Exhaustion;
	protected float m_ExhaustionTimer;

	override void Init()
	{
		super.Init();

		m_ID = eModifiers.MDF_HEAT_STROKE3;
	}
		
	override string GetDebugText()
	{
		return ("Activate threshold >: "+ THRESHOLD_ACTIVATE + "| Deativate threshold: <" + THRESHOLD_ACTIVATE);
	}
	
	override protected bool ActivateCondition(PlayerBase player)
	{
		return player.GetThermalBiasHandler().Get() > THRESHOLD_ACTIVATE;
	}

	override protected bool DeactivateCondition(PlayerBase player)
	{
		return player.GetThermalBiasHandler().Get() < THRESHOLD_ACTIVATE;
	}
	
	override protected void OnActivate(PlayerBase player)
	{
		player.GetStaminaHandler().ActivateRecoveryModifier(EStaminaMultiplierTypes.DISEASE_PNEUMONIA);
		player.GetStaminaHandler().ActivateDepletionModifier(EStaminaMultiplierTypes.DISEASE_PNEUMONIA);
	}
	
	override void OnReconnect(PlayerBase player)
	{
		this.OnActivate(player);
	}
	
	override void OnDeactivate(PlayerBase player)
	{
		player.GetStaminaHandler().DeactivateRecoveryModifier(EStaminaMultiplierTypes.DISEASE_PNEUMONIA);
		player.GetStaminaHandler().DeactivateDepletionModifier(EStaminaMultiplierTypes.DISEASE_PNEUMONIA);
	}
	
	override protected void OnTick(PlayerBase player, float deltaT)
	{
		PlayerStat<float> waterStat 	= player.GetStatWater();
		PlayerStat<float> energyStat 	= player.GetStatEnergy();
		waterStat.Add(-WATER_DRAIN_FROM_FEVER_ST3 * deltaT);
		
		float thermalBias = player.GetThermalBiasHandler().Get();

		float thermalBiasRemapped = Math.Remap(THRESHOLD_ACTIVATE, 1.0, 0.0, 1.0, thermalBias);
		float chance = Math.Lerp(VOMIT_OCCURRENCES_MIN, VOMIT_OCCURRENCES_MAX, thermalBiasRemapped);
		chance = (chance / 900) * deltaT;
		
		if (Math.RandomFloat01() < chance)
		{
			SymptomBase symptom = player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_VOMIT);
			if (symptom)
			{
				CachedObjectsParams.PARAM1_FLOAT.param1 = 10.0;
				symptom.SetParam(CachedObjectsParams.PARAM1_FLOAT);
				
				if (waterStat.Get() > WATER_DRAIN_FROM_VOMIT) 
					waterStat.Add(-1 * WATER_DRAIN_FROM_VOMIT);
				if (energyStat.Get() > ENERGY_DRAIN_FROM_VOMIT)
					energyStat.Add(-1 * ENERGY_DRAIN_FROM_VOMIT);
				
				player.GetStaminaHandler().ActivateRecoveryModifier(EStaminaMultiplierTypes.VOMIT_EXHAUSTION);
				player.GetStaminaHandler().ActivateDepletionModifier(EStaminaMultiplierTypes.VOMIT_EXHAUSTION);
					
				m_Exhaustion = true;
				m_ExhaustionTimer = 0;
			}
		}
		
		if (m_Exhaustion)
		{
			m_ExhaustionTimer += deltaT;
			if (m_ExhaustionTimer >= 30 * thermalBiasRemapped)
			{
				player.GetStaminaHandler().DeactivateRecoveryModifier(EStaminaMultiplierTypes.VOMIT_EXHAUSTION);
				player.GetStaminaHandler().DeactivateDepletionModifier(EStaminaMultiplierTypes.VOMIT_EXHAUSTION);
				
				m_Exhaustion = false;
			}
		}
	}
}

class HeatStrokePhase4Mdfr : HeatStrokeMdfr
{
	private static const float THRESHOLD_ACTIVATE = HeatStrokeMdfr.STAGE_THRESHOLDS[3];
	
	protected bool m_CanFaintEffectFadeIn, m_CanFaintEffectFadeOut, m_FaintEffectActive;
	protected float m_FaintEffectTime, m_UnconciousnessActiveTimer, m_UnconciousnessInactiveTimer;
	protected float m_NextUnconsciousnessEvent;

	private ref SymptomBase m_FaintSymptom;

	override void Init()
	{
		super.Init();

		m_ID = eModifiers.MDF_HEAT_STROKE4;
	}
		
	override string GetDebugText()
	{
		return ("Activate threshold >: "+ THRESHOLD_ACTIVATE + "| Deativate threshold: <" + THRESHOLD_ACTIVATE);
	}
	
	override protected bool ActivateCondition(PlayerBase player)
	{
		return player.GetThermalBiasHandler().Get() > THRESHOLD_ACTIVATE;
	}

	override protected bool DeactivateCondition(PlayerBase player)
	{
		return player.GetThermalBiasHandler().Get() < THRESHOLD_ACTIVATE;
	}
	
	override protected void OnActivate(PlayerBase player)
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
		player.GetSymptomManager().RemoveSecondarySymptom(SymptomIDs.SYMPTOM_FAINT);
	}
	
	override protected void OnTick(PlayerBase player, float deltaT)
	{
		float thermalBias = player.GetThermalBiasHandler().Get();

		Param2<int, ref SymptomSyncData> params = new Param2<int, ref SymptomSyncData>(0, null);
		SymptomSyncData faintData = new SymptomSyncData();
		if (!player.IsUnconscious())
		{
			m_FaintEffectTime += deltaT;
		
			// HEALTH DAMAGE
			float health = Math.Remap(THRESHOLD_ACTIVATE, 1.0, HEALTH_DAMAGE_MIN, HEALTH_DAMAGE_MAX, thermalBias);
			player.AddHealth("", "", -health * deltaT);
	
			//! fainting
			float vignette = Math.Remap(STAGE_THRESHOLDS[3], 1.0, 1.0, 2.0, thermalBias);
			float fadeOut = Math.Remap(STAGE_THRESHOLDS[3], 1.0, 3.0, 5.0, thermalBias);
			float interval = Math.Remap(1.0, STAGE_THRESHOLDS[3], 2.0, 10.0, thermalBias) + fadeOut;

			if (m_FaintEffectTime > interval && m_CanFaintEffectFadeIn)
			{
				params.param1 = SymptomIDs.SYMPTOM_FAINT;
				faintData.m_FloatArray.Insert(vignette);
				faintData.m_FloatArray.Insert(fadeOut);
				faintData.m_FloatArray.Insert(interval);
				faintData.m_IntArray.Insert(1);
				params.param2 = faintData;
	
				m_FaintSymptom.SetParam(params);

				m_FaintEffectTime 		= 0.0;
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
				faintData.m_IntArray.Insert(1);
				params.param2 = faintData;

				m_FaintSymptom.SetParam(params);

				m_FaintEffectTime 		= 0.0;				
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

		m_UnconciousnessActiveTimer		+= deltaT;
		m_UnconciousnessInactiveTimer	+= deltaT;

		//! unconsciousness
		bool canTriggerUnconsciousness = !m_CanFaintEffectFadeOut && player.GetHealth("", "Shock") > SHOCK_ACTIVATION_THRESHOLD_MIN;
		if ((thermalBias > 0.75 && canTriggerUnconsciousness) || player.IsUnconscious())
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
}
