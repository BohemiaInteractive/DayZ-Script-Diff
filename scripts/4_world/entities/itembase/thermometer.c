class Thermometer extends ItemBase
{
	float GetTemperatureValue(PlayerBase player)
	{
		float value;
		if (player && player.IsSyncedModifierActive(eModifierSyncIDs.MODIFIER_SYNC_FEVER))
		{
			value = Math.RandomFloatInclusive(PlayerConstants.HIGH_TEMPERATURE_L, PlayerConstants.HIGH_TEMPERATURE_H);
		}
		else
		{
			value = Math.RandomFloatInclusive(PlayerConstants.NORMAL_TEMPERATURE_L, PlayerConstants.NORMAL_TEMPERATURE_H);
		}
		value = Math.Round(value * 10) / 10;
		return value;
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionMeasureTemperatureTarget);
		AddAction(ActionMeasureTemperatureSelf);
	}
}