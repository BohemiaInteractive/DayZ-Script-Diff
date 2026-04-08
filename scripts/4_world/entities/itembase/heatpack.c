class Heatpack : ItemBase
{
	override bool CanHaveTemperature()
	{
		return true;
	}
	
	override void OnWorkStart()
	{
		if (g_Game.IsServer())
		{
			SetTemperatureDirect(80);
		}
	}
	
	override void OnWork( float consumed_energy )
	{
		if (g_Game.IsServer())
		{
			SetTemperatureDirect(80);
		}
	}
	
	override void OnWorkStop()
	{
		if (g_Game.IsServer())
		{
			SetHealth(0);
		}
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionTurnOnHeatpack);
	}
	
	override float GetQuantityNormalizedScripted()
	{
		return 1.0;
	}
	
	override bool IsSelfAdjustingTemperature()
	{
		return GetCompEM().IsWorking();
	}
}
