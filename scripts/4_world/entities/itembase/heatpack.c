class Heatpack : ItemBase
{
	override bool CanHaveTemperature()
	{
		return true;
	}
	
	override void OnWorkStart()
	{
		if (GetGame().IsServer())
		{
			SetTemperature(60);
		}
	}
	
	override void OnWork( float consumed_energy )
	{
		if (GetGame().IsServer())
		{
			SetTemperature(60);
		}
	}
	
	override void OnWorkStop()
	{
		if (GetGame().IsServer())
		{
			SetHealth(0);
		}
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionTurnOnHeatpack);
	}
};
