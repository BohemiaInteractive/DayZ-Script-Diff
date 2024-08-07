class CholeraMdfr: ModifierBase
{
	static const int AGENT_THRESHOLD_ACTIVATE = 250;
	static const int AGENT_THRESHOLD_DEACTIVATE = 50;
	
	static const int CHANCE_OF_VOMIT = 10;
	static const int CHANCE_OF_VOMIT_AGENT = 30;
	static const int WATER_DRAIN_FROM_VOMIT = 450;
	static const int ENERGY_DRAIN_FROM_VOMIT = 310;
	static const float WATER_LOSS_CHOLERA = 0.5;
	static const float WATER_LOSS_CHOLERA_MIN = 0.1; 	// min multiplier of WATER_LOSS_CHOLERA
	static const float STOMACH_MIN_VOLUME = 200;		// for vomit symptom
	
	private float m_ExhaustionTimer;
	private bool m_Exhaustion;

	override void Init()
	{
		m_TrackActivatedTime 	= false;
		m_ID 					= eModifiers.MDF_CHOLERA;
		m_TickIntervalInactive 	= DEFAULT_TICK_TIME_INACTIVE;
		m_TickIntervalActive 	= DEFAULT_TICK_TIME_ACTIVE;
	}
	
	override string GetDebugText()
	{
		return ("Activate threshold: "+AGENT_THRESHOLD_ACTIVATE + "| " +"Deativate threshold: "+AGENT_THRESHOLD_DEACTIVATE);
	}
	
	override protected bool ActivateCondition(PlayerBase player)
	{
		return (player.GetSingleAgentCount(eAgents.CHOLERA) >= AGENT_THRESHOLD_ACTIVATE);
	}

	override protected void OnActivate(PlayerBase player)
	{
		player.IncreaseDiseaseCount();
		
		SymptomBase symptom = player.GetSymptomManager().QueueUpPrimarySymptom( SymptomIDs.SYMPTOM_VOMIT );
		if (symptom)
		{
			CachedObjectsParams.PARAM1_FLOAT.param1 = 65.0;
			symptom.SetParam(CachedObjectsParams.PARAM1_FLOAT);
			symptom.SetDuration(Math.RandomIntInclusive(4.0, 8.0));
		}
	}

	override protected void OnDeactivate(PlayerBase player)
	{		
		player.GetStaminaHandler().DeactivateRecoveryModifier(EStaminaMultiplierTypes.VOMIT_EXHAUSTION);
		player.GetStaminaHandler().DeactivateDepletionModifier(EStaminaMultiplierTypes.VOMIT_EXHAUSTION);
		m_Exhaustion = false;
		m_ExhaustionTimer = 0;
		player.DecreaseDiseaseCount();
	}

	override protected bool DeactivateCondition(PlayerBase player)
	{
		return (player.GetSingleAgentCount(eAgents.CHOLERA) <= AGENT_THRESHOLD_DEACTIVATE);
	}

	override protected void OnTick(PlayerBase player, float deltaT)
	{
		float waterLoss = ( deltaT * ( WATER_LOSS_CHOLERA * Math.Max( WATER_LOSS_CHOLERA_MIN, player.GetSingleAgentCountNormalized( eAgents.CHOLERA ) ) ) );
		player.GetStatWater().Add(-waterLoss);

		float stomachVolume = player.m_PlayerStomach.GetStomachVolume();
		if (stomachVolume >= STOMACH_MIN_VOLUME)
		{
			int roll = Math.RandomInt( 0, 100 );
			if( roll < ( CHANCE_OF_VOMIT + ( CHANCE_OF_VOMIT_AGENT * player.GetSingleAgentCountNormalized( eAgents.CHOLERA ) ) ) )
			{
				SymptomBase symptom = player.GetSymptomManager().QueueUpPrimarySymptom( SymptomIDs.SYMPTOM_VOMIT );
				if (symptom)
				{
					CachedObjectsParams.PARAM1_FLOAT.param1 = 65.0;
					symptom.SetParam(CachedObjectsParams.PARAM1_FLOAT);
					symptom.SetDuration(Math.RandomIntInclusive(4.0, 8.0));

					if (m_Player.GetStatWater().Get() > (WATER_DRAIN_FROM_VOMIT))
						m_Player.GetStatWater().Add(-1 * WATER_DRAIN_FROM_VOMIT);
					if (m_Player.GetStatEnergy().Get() > (ENERGY_DRAIN_FROM_VOMIT))
						m_Player.GetStatEnergy().Add(-1 * ENERGY_DRAIN_FROM_VOMIT);
				
					player.GetStaminaHandler().ActivateRecoveryModifier(EStaminaMultiplierTypes.VOMIT_EXHAUSTION);
					player.GetStaminaHandler().ActivateDepletionModifier(EStaminaMultiplierTypes.VOMIT_EXHAUSTION);
						
					m_Exhaustion = true;
					m_ExhaustionTimer = 0;
				}
			}
		}
		
		if (m_Exhaustion)
		{
			m_ExhaustionTimer += deltaT;
			if (m_ExhaustionTimer >= 30)
			{
				player.GetStaminaHandler().DeactivateRecoveryModifier(EStaminaMultiplierTypes.VOMIT_EXHAUSTION);
				player.GetStaminaHandler().DeactivateDepletionModifier(EStaminaMultiplierTypes.VOMIT_EXHAUSTION);
				
				m_Exhaustion = false;
			}
		}
	}
}
