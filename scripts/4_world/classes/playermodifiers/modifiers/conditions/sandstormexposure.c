class SandstormZoneSmall_TESTONLY : ContaminatedArea_Static
{
	override void OnPlayerEnterServer(PlayerBase player, EffectTrigger trigger)
	{
		player.GetModifiersManager().ActivateModifier(eModifiers.MDF_SANDSTORM_EXPOSURE_STATIC);
	}
	
	override void OnPlayerExitServer(PlayerBase player, EffectTrigger trigger)
	{
		player.GetModifiersManager().DeactivateModifier(eModifiers.MDF_SANDSTORM_EXPOSURE_STATIC);
	}
}

class SandstormExposureMdfrBase : ModifierBase
{
	protected const float SILICOSIS_AGENTS_PER_SEC 			= 1.0;
	protected const float EYES_IRRITATION_AGENTS_PER_SEC 	= 1.0;

	private ref PluginTransmissionAgents m_AgentTransmision;
	private ref SymptomBase m_SquintSymptom;

	override void Init()
	{
		m_TrackActivatedTime 	= false;
		m_AnalyticsStatsEnabled = true;
		m_TickIntervalInactive 	= DEFAULT_TICK_TIME_INACTIVE;
		m_TickIntervalActive 	= DEFAULT_TICK_TIME_ACTIVE_SHORT;
		
		m_AgentTransmision 		= PluginTransmissionAgents.Cast(GetPlugin(PluginTransmissionAgents));
	}
	
	override void OnActivate(PlayerBase player)
	{
		m_SquintSymptom = player.GetSymptomManager().QueueUpSecondarySymptomEx(SymptomIDs.SYMPTOM_SQUINT);

		TransmitAgents(eAgents.SILICOSIS, player, 1);
		TransmitAgents(eAgents.EYES_IRRITATION, player, 1);
	}
	
	override void OnDeactivate(PlayerBase player)
	{
		player.GetSymptomManager().RemoveSecondarySymptom(SymptomIDs.SYMPTOM_SQUINT);
	}

	protected float TransmitAgents(int type, PlayerBase player, float count)
	{
		if (player.IsUnconscious())
			return 0.0;

		return m_AgentTransmision.TransmitAgentsEx(null, player, AGT_AIRBOURNE_SOLID, count, type);
	}
}

class SandstormExposureStaticMdfr : SandstormExposureMdfrBase
{
	override void Init()
	{
		super.Init();

		m_ID = eModifiers.MDF_SANDSTORM_EXPOSURE_STATIC;

		DisableActivateCheck();
		DisableDeactivateCheck();
	}
	
	override bool ActivateCondition(PlayerBase player)
	{
		return false;
	}

	override bool DeactivateCondition(PlayerBase player)
	{
		return false;
	}
	
	override void OnTick(PlayerBase player, float deltaT)
	{
		TransmitAgents(eAgents.SILICOSIS, m_Player, SILICOSIS_AGENTS_PER_SEC * deltaT);
		TransmitAgents(eAgents.EYES_IRRITATION, m_Player, EYES_IRRITATION_AGENTS_PER_SEC * deltaT);
	}
}

class SandstormExposureDynamicMdfr : SandstormExposureMdfrBase
{
	private const float COUGH_INTERVAL_MIN = 3.0; //! seconds
	private const float COUGH_INTERVAL_MAX = 25.0; //! seconds

	private ScriptedSandstormController m_SandstormController;
	
	protected float m_CoughTimer;

	override void Init()
	{
		super.Init();
		
		m_ID = eModifiers.MDF_SANDSTORM_EXPOSURE_DYNAMIC;

		m_SandstormController = ScriptedSandstormController.Cast(g_Game.GetWeather().GetSandstorm());
	}

	override bool ActivateCondition(PlayerBase player)
	{
		return (player && m_SandstormController.IsActive() && m_SandstormController.GetIntensityForPlayer(player) > 0.0);
	}

	override bool DeactivateCondition(PlayerBase player)
	{
		return (player && m_SandstormController.IsActive() && m_SandstormController.GetIntensityForPlayer(player) <= 0.0);
	}
	
	override void OnTick(PlayerBase player, float deltaT)
	{
		if (!m_SandstormController.IsActive())
			return;
		
		float sandstormIntensity = m_SandstormController.GetIntensityForPlayer(player);
		
		HandleCoughing(player, sandstormIntensity, deltaT);
		
		TransmitAgents(eAgents.SILICOSIS, player, sandstormIntensity * SILICOSIS_AGENTS_PER_SEC * deltaT);
		TransmitAgents(eAgents.EYES_IRRITATION, player, sandstormIntensity * EYES_IRRITATION_AGENTS_PER_SEC * deltaT);
	}
	
	protected void HandleCoughing(PlayerBase player, float sandstormIntensity, float deltaTime)
	{
		CachedEquipmentStorageQuery query = new CachedEquipmentStorageQuery();
		query.m_Category = ECachedEquipmentItemCategory.EYEWEAR|ECachedEquipmentItemCategory.MASK;
		query.m_Placement = ECachedEquipmentPlacement.ATTACHMENT;
		query.m_MaximumDepth = 2;

		float protectionLevel = 0.0;
		ItemBase coverItem = null;
		array<Entity> coverItems = player.GetCachedEquipment().GetEntitiesByCategory(query);
		int coverItemsCount = coverItems.Count();
		for (int i = 0; i < coverItemsCount; ++i)
		{
			coverItem = ItemBase.Cast(coverItems[i]);
			protectionLevel = coverItem.GetProtectionLevel(DEF_DUST_PARTICLE_BREATH);
			if (protectionLevel > 0.0)
				break;
		}
		
		float coughIntensity = Math.Clamp(sandstormIntensity - protectionLevel, 0.0, 1.0);;
		float coughInterval = Math.Lerp(COUGH_INTERVAL_MAX, COUGH_INTERVAL_MIN, coughIntensity);
		
		if (coughIntensity > 0.0)
			m_CoughTimer += deltaTime;
		else
			m_CoughTimer = 0.0;

		if (m_CoughTimer >= coughInterval)
		{
			m_CoughTimer = 0.0;
			
			Math.Randomize(Math.RandomInt(1, 1000));
			float rand = Math.RandomFloat01();
			if (rand <= coughIntensity)
				player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_COUGH);
		}
	}
}
