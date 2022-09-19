class BloodRegenMdfr: ModifierBase
{
	override void Init()
	{
		m_TrackActivatedTime = false;
		m_ID 					= eModifiers.MDF_BLOOD_REGEN;
		m_TickIntervalInactive 	= DEFAULT_TICK_TIME_INACTIVE;
		m_TickIntervalActive 	= DEFAULT_TICK_TIME_ACTIVE;
	}

	override bool ActivateCondition(PlayerBase player)
	{
		float m_MaxBlood = player.GetMaxHealth("GlobalHealth", "Blood");
		if ( player.GetHealth("GlobalHealth", "Blood") <= m_MaxBlood )
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	override void OnActivate(PlayerBase player)
	{
	}


	override bool DeactivateCondition(PlayerBase player)
	{
		float m_MaxBlood = player.GetMaxHealth("GlobalHealth", "Blood");
		if ( player.GetHealth("GlobalHealth", "Blood") >= m_MaxBlood )
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	override void OnTick(PlayerBase player, float deltaT)
	{
		float regen_modifier_water = GetRegenModifierWater( player.GetStatWater().Get() );
		float regen_modifier_energy = GetRegenModifierEnergy( player.GetStatEnergy().Get() );
		float blood_regen_speed = PlayerConstants.BLOOD_REGEN_RATE_PER_SEC * regen_modifier_water * regen_modifier_energy;
		/*
		PrintString("regen_modifier_water" + regen_modifier_water);
		PrintString("regen_modifier_energy" + regen_modifier_energy);
		PrintString("blood_regen_speed" + blood_regen_speed);
		*/
		player.AddHealth("","Blood", blood_regen_speed * deltaT );
	}
	
	float GetRegenModifierWater(float water)
	{
		float modifier = PlayerConstants.BLOOD_REGEN_MODIFIER_WATER_HIGH;
		int water_level = m_Player.GetStatLevelWater();
		
		/*
		if( water < PlayerConstants.BLOOD_REGEN_THRESHOLD_WATER_MID ) {modifier = PlayerConstants.BLOOD_REGEN_MODIFIER_WATER_MID}
		if( water < PlayerConstants.BLOOD_REGEN_THRESHOLD_WATER_LOW ) {modifier = PlayerConstants.BLOOD_REGEN_MODIFIER_WATER_LOW}
		*/
		
		if( water_level == EStatLevels.MEDIUM ) {modifier = PlayerConstants.BLOOD_REGEN_MODIFIER_WATER_MID}
		if( water_level >= EStatLevels.LOW ) {modifier = PlayerConstants.BLOOD_REGEN_MODIFIER_WATER_LOW}
		
		return modifier;
	}
	
	float GetRegenModifierEnergy(float energy)
	{
		float modifier = PlayerConstants.BLOOD_REGEN_MODIFIER_ENERGY_HIGH;
		int energy_level = m_Player.GetStatLevelEnergy();
				
		//if( energy < PlayerConstants.BLOOD_REGEN_THRESHOLD_ENERGY_MID ) {modifier = PlayerConstants.BLOOD_REGEN_MODIFIER_ENERGY_MID}
		//if( energy < PlayerConstants.BLOOD_REGEN_THRESHOLD_ENERGY_LOW ) {modifier = PlayerConstants.BLOOD_REGEN_MODIFIER_ENERGY_LOW}
		
		if( energy_level == EStatLevels.MEDIUM ) {modifier = PlayerConstants.BLOOD_REGEN_MODIFIER_ENERGY_MID}
		if( energy_level >= EStatLevels.LOW ) {modifier = PlayerConstants.BLOOD_REGEN_MODIFIER_ENERGY_LOW}
		
		return modifier;
	}
	
	
};