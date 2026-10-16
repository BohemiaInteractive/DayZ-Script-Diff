class HeatComfortMdfr : ModifierBase
{
	override void Init()
	{
		m_TrackActivatedTime 	= false;
		m_ID 					= eModifiers.MDF_TEMPERATURE;
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
	
	override string GetDebugText()
	{
		return m_Player.GetStatHeatComfort().Get().ToString();
	}
	
	override string GetDebugTextSimple()
	{
		return m_Player.GetStatHeatComfort().Get().ToString();
	}

	
	override void OnTick(PlayerBase player, float deltaT)
	{
		float heatComfortValue = player.GetStatHeatComfort().Get();
		ThermalBiasHandler thermalBiasHandler = player.GetThermalBiasHandler();
		float thermalBiasValue = thermalBiasHandler.Get();
		PlayerStat<float> waterStat = player.GetStatWater();
		float waterValue = waterStat.Get();
		float valueNormalized;
		
		if (heatComfortValue > PlayerConstants.WATER_LOSS_THRESHOLD_HC_PLUS_LOW)
		{
			valueNormalized = Math.Lerp(PlayerConstants.WATER_LOSS_THRESHOLD_HC_PLUS_LOW, PlayerConstants.WATER_LOSS_THRESHOLD_HC_PLUS_HIGH, heatComfortValue);
			valueNormalized = Math.Clamp(valueNormalized, 0, 1);
			float waterLoss = deltaT * Math.Lerp(PlayerConstants.WATER_LOSS_HC_PLUS_LOW, PlayerConstants.WATER_LOSS_HC_PLUS_HIGH, valueNormalized);
			waterStat.Add(-waterLoss);
		}
		if (heatComfortValue < PlayerConstants.ENERGY_LOSS_THRESHOLD_HC_MINUS_LOW)
		{
			valueNormalized 	= Math.InverseLerp(PlayerConstants.ENERGY_LOSS_THRESHOLD_HC_MINUS_LOW, PlayerConstants.ENERGY_LOSS_THRESHOLD_HC_MINUS_HIGH, heatComfortValue);
			valueNormalized 	= Math.Clamp(valueNormalized, 0, 1);
			float energyLoss 	= deltaT * Math.Lerp(PlayerConstants.ENERGY_LOSS_HC_MINUS_LOW, PlayerConstants.ENERGY_LOSS_HC_MINUS_HIGH, valueNormalized);
			player.GetStatEnergy().Add(-energyLoss);

			if(heatComfortValue < PlayerConstants.ENERGY_LOSS_THRESHOLD_HC_MINUS_HIGH)
			{
				valueNormalized = Math.InverseLerp(PlayerConstants.ENERGY_LOSS_THRESHOLD_HC_MINUS_HIGH, -1.0, heatComfortValue);
				valueNormalized = Math.Clamp(valueNormalized, 0, 1);

				float healthLoss = deltaT * Math.Lerp(PlayerConstants.HEALTH_LOSS_HC_MINUS_LOW, PlayerConstants.HEALTH_LOSS_HC_MINUS_HIGH, Easing.EaseInQuad(valueNormalized));
				//! direct health loss handling
				if (healthLoss != 0.0)
					player.AddHealth("", "", -healthLoss);
			}
		}

		//! Thermal Bias value feeding (based on HeatStroke stages)	
		if (heatComfortValue > PlayerConstants.THRESHOLD_HEAT_COMFORT_PLUS_EMPTY)
		{
			thermalBiasHandler.Add(PlayerConstants.THERMAL_BIAS_POSITIVE_HC_HIGH_INCREMENT * deltaT);
		}
		else if (heatComfortValue > PlayerConstants.THRESHOLD_HEAT_COMFORT_PLUS_CRITICAL)
		{
			thermalBiasHandler.Add(PlayerConstants.THERMAL_BIAS_POSITIVE_HC_MEDIUM_INCREMENT * deltaT);
		}

		if (heatComfortValue > PlayerConstants.THRESHOLD_HEAT_COMFORT_PLUS_WARNING)
		{		
			if (waterValue < PlayerConstants.SL_WATER_NORMAL && !player.IsUnconscious())
			{
				if (thermalBiasValue > HeatStrokeMdfr.STAGE_THRESHOLDS[3])
					thermalBiasHandler.Add(PlayerConstants.THERMAL_BIAS_POSITIVE_HC_HIGH_INCREMENT * deltaT);
				else if (thermalBiasValue >= HeatStrokeMdfr.STAGE_THRESHOLDS[2])
					thermalBiasHandler.Add(PlayerConstants.THERMAL_BIAS_POSITIVE_HC_MEDIUM_INCREMENT * deltaT);
				else
					thermalBiasHandler.Add(PlayerConstants.THERMAL_BIAS_POSITIVE_HC_LOW_INCREMENT * deltaT);
			}
			else if (waterValue < PlayerConstants.SL_WATER_HIGH && !player.IsUnconscious())
			{
				if (thermalBiasValue < HeatStrokeMdfr.STAGE_THRESHOLDS[1])
				{
					thermalBiasHandler.Add(PlayerConstants.THERMAL_BIAS_POSITIVE_HC_LOW_INCREMENT * deltaT);
				}
				else
					thermalBiasHandler.Add(-PlayerConstants.THERMAL_BIAS_POSITIVE_HC_DECREMENT * deltaT);
			}
		}
		else
		{
			//! keep the value at 0.0 if we are bellow the positive yellow HC
			thermalBiasHandler.Add(-PlayerConstants.THERMAL_BIAS_POSITIVE_HC_DECREMENT * deltaT);
			if (thermalBiasValue <= 0.0)
				thermalBiasHandler.Set(0.0);
		}
	}
}
