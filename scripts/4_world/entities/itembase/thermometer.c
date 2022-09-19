class Thermometer extends ItemBase
{
	float GetTemperatureValue(PlayerBase player)
	{
		string temp;
		float value;
		if( player.GetModifiersManager() && player.GetModifiersManager().IsModifierActive(eModifiers.MDF_FEVER) )
		{
			value = Math.RandomFloatInclusive(PlayerConstants.HIGH_TEMPERATURE_L, PlayerConstants.HIGH_TEMPERATURE_H);
		}
		else
		{
			value = Math.RandomFloatInclusive(PlayerConstants.NORMAL_TEMPERATURE_L, PlayerConstants.NORMAL_TEMPERATURE_H);
		}
		value = Math.Round(value * 10) / 10;
		return value;
		//temp = value.ToString();
		//return temp + "C";
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionMeasureTemperatureTarget);
		AddAction(ActionMeasureTemperatureSelf);
	}
}