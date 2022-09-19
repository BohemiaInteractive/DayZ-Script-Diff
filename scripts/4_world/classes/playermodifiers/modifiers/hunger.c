class HungerMdfr: ModifierBase
{
	protected float	m_EnergyDelta;
	protected float	m_LastEnergyLevel;
	ref HumanMovementState		m_MovementState	= new HumanMovementState();
		
	override void Init()
	{
		m_TrackActivatedTime = false;
		m_ID 					= eModifiers.MDF_HUNGER;
		m_TickIntervalInactive 	= DEFAULT_TICK_TIME_INACTIVE;
		m_TickIntervalActive 	= 1;
		DisableDeactivateCheck();
	}
	
	override bool ActivateCondition(PlayerBase player)
	{
		return true;
	}

	override bool DeactivateCondition(PlayerBase player)
	{
		return false;
	}
	
	override void OnReconnect(PlayerBase player)
	{

	}

	override void OnTick(PlayerBase player, float deltaT)
	{
		player.GetMovementState(m_MovementState);
		float energy = player.GetStatEnergy().Get();
		float metabolic_speed = MiscGameplayFunctions.GetEnergyMetabolicSpeed(m_MovementState.m_iMovement);
		
		float modifier = energy/PlayerConstants.SL_ENERGY_MAX + PlayerConstants.CONSUMPTION_MULTIPLIER_BASE; 
		metabolic_speed *= modifier; //non linear shaping for consumption curve (comment out to have it linear)
		
		player.GetStatEnergy().Add( -metabolic_speed * deltaT );
		if ( energy <= PlayerConstants.LOW_ENERGY_THRESHOLD )
		{
			player.SetMixedSoundState( eMixedSoundStates.HUNGRY );
			if( (player.GetStomach().GetDigestingType() & PlayerStomach.DIGESTING_ENERGY) == 0)
				player.AddHealth("GlobalHealth", "Health", -PlayerConstants.LOW_ENERGY_DAMAGE_PER_SEC * deltaT );
		}
		else
		{
			player.UnsetMixedSoundState( eMixedSoundStates.HUNGRY );
		}
	}
};