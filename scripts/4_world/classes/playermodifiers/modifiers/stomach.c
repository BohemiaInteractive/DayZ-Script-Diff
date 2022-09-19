class StomachMdfr: ModifierBase
{
	override void Init()
	{
		m_TrackActivatedTime = false;
		m_ID 					= eModifiers.MDF_STOMACH;
		m_TickIntervalInactive 	= DEFAULT_TICK_TIME_INACTIVE;
		m_TickIntervalActive 	= DEFAULT_TICK_TIME_ACTIVE;
		
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
		player.m_PlayerStomach.Update(deltaT);
		/*
		float stomachs_combined_before = player.GetStatStomachWater().Get() + player.GetStatStomachEnergy().Get();
		if(stomachs_combined_before <=0) // takes care of division by 0 also there is no point in doing anything if there is nothing to transfer
		{
			player.GetStatStomachVolume().Set(0);
			return;
		}
		
		// Energy
		if ( player.GetStatStomachEnergy().Get() >= PlayerConstants.STOMACH_ENERGY_TRANSFERED_PER_SEC*deltaT )
		{
			player.GetStatEnergy().Add(PlayerConstants.STOMACH_ENERGY_TRANSFERED_PER_SEC*deltaT);
			player.GetStatStomachEnergy().Add(-PlayerConstants.STOMACH_ENERGY_TRANSFERED_PER_SEC*deltaT);
		}
		else
		{
			player.GetStatEnergy().Add(player.GetStatStomachEnergy().Get());
			player.GetStatStomachEnergy().Set(0);
		}
		//Water
		if (  player.GetStatStomachWater().Get() >= PlayerConstants.STOMACH_WATER_TRANSFERED_PER_SEC*deltaT )
		{
			player.GetStatWater().Add(PlayerConstants.STOMACH_WATER_TRANSFERED_PER_SEC*deltaT);
			player.GetStatStomachWater().Add(-PlayerConstants.STOMACH_WATER_TRANSFERED_PER_SEC*deltaT);
			
		}
		else
		{
			player.GetStatWater().Add(player.GetStatStomachWater().Get());
			player.GetStatStomachWater().Set(0);
			
		}
		
		float stomachs_combined_after = player.GetStatStomachWater().Get() + player.GetStatStomachEnergy().Get();
		float percentage = stomachs_combined_after / stomachs_combined_before;
		
		player.GetStatStomachVolume().Set( player.GetStatStomachVolume().Get() * percentage );
		*/
	}
};