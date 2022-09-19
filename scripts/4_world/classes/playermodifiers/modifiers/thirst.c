class ThirstMdfr: ModifierBase
{
	float 			m_LastWaterLevel;
	ref HumanMovementState		m_MovementState	= new HumanMovementState();
	
	override void Init()
	{
		m_TrackActivatedTime = false;
		m_ID 					= eModifiers.MDF_THIRST;
		m_TickIntervalInactive 	= DEFAULT_TICK_TIME_INACTIVE;
		m_TickIntervalActive 	= 1;
		DisableDeactivateCheck();
	}		

	override bool ActivateCondition(PlayerBase player)
	{
		return true;
	}
	
	override void OnReconnect(PlayerBase player)
	{

	}

	override bool DeactivateCondition(PlayerBase player)
	{
		return false;
	}

	override void OnTick(PlayerBase player, float deltaT)
	{
		player.GetMovementState(m_MovementState);
		float water = player.GetStatWater().Get();
		float metabolic_speed = MiscGameplayFunctions.GetWaterMetabolicSpeed(m_MovementState.m_iMovement);
		
		float modifier = water/PlayerConstants.SL_WATER_MAX + PlayerConstants.CONSUMPTION_MULTIPLIER_BASE;
		metabolic_speed *= modifier; //non linear shaping for consumption curve (comment out to have it linear)
		
		player.GetStatWater().Add( (-metabolic_speed * deltaT) );
		
		if ( water <= PlayerConstants.LOW_WATER_THRESHOLD )
		{	
			if( !player.GetStomach().IsDigesting() )	
				player.AddHealth("GlobalHealth", "Health", -PlayerConstants.LOW_WATER_DAMAGE_PER_SEC * deltaT );
		}
	}
};