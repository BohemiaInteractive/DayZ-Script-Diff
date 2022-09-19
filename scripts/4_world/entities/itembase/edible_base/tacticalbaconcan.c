class TacticalBaconCan : Edible_Base
{
	override void Open()
	{
		ReplaceEdibleWithNew("TacticalBaconCan_Opened");
	}
	
	
	override bool IsOpen()
	{
		return false;
	}
}
