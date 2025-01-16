class HeatBufferMdfr : ModifierBase
{
	const int NUMBER_OF_STAGES = 4;
	const float STAGE_THRESHOLDS[NUMBER_OF_STAGES] = {0.0, 0.60, 0.85, 1.0};

	protected int m_Stage;
	protected int m_StageLast;

	protected float m_Value;
	
	override void Init()
	{
		m_TrackActivatedTime 	= false;
		m_AnalyticsStatsEnabled = true;
		m_ID 					= eModifiers.MDF_HEATBUFFER;
		m_TickIntervalInactive 	= 1;
		m_TickIntervalActive 	= DEFAULT_TICK_TIME_ACTIVE;
	}
	
	override bool ActivateCondition(PlayerBase player)
	{
		m_Value = player.GetStatHeatBuffer().Get() / player.GetStatHeatBuffer().GetMax();
		if (m_Value > STAGE_THRESHOLDS[0])
			return true;
	
		return false;
	}	

	override bool DeactivateCondition(PlayerBase player)
	{
		m_Value = player.GetStatHeatBuffer().Get() / player.GetStatHeatBuffer().GetMax();;
		if (m_Value <= STAGE_THRESHOLDS[0])
			return true;

		return false;
	}

	override void OnActivate(PlayerBase player)
	{
		player.ToggleHeatBufferVisibility(GetHeatBufferStage());
	}

	override void OnDeactivate(PlayerBase player)
	{
		player.ToggleHeatBufferVisibility(GetHeatBufferStage());
		//Protection against influenza with duration
		if (player.GetTemporaryResistanceToAgent(eAgents.INFLUENZA) < 300)
			player.SetTemporaryResistanceToAgent(eAgents.INFLUENZA, 300);
	}
	
	override void Tick(float delta_time)
	{
		super.Tick(delta_time);
		
		m_Value = m_Player.GetStatHeatBuffer().Get() / m_Player.GetStatHeatBuffer().GetMax();
		m_Stage = GetHeatBufferStage();

		if (m_Stage != m_StageLast)
		{
			m_Player.ToggleHeatBufferVisibility(m_Stage);
			m_StageLast = m_Stage;
		}
	}
	
	protected int GetHeatBufferStage()
	{
		if (m_Value > STAGE_THRESHOLDS[0] && m_Value < STAGE_THRESHOLDS[1])
			return 1;
		else if (m_Value >= STAGE_THRESHOLDS[1] && m_Value < STAGE_THRESHOLDS[2])
			return 2;
		else if (m_Value >= STAGE_THRESHOLDS[2])
			return 3;
		
		return 0;
	}
	
	//! DEPRECATED
	protected const float HEATBUFFER_SHOW = 25.0;
	protected const float HEATBUFFER_HIDE = 20.0;
}
