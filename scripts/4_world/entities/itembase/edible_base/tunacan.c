class TunaCan : Edible_Base
{
	override void Open()
	{
		ReplaceEdibleWithNew("TunaCan_Opened");
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionOpen);
	}
	
	override bool IsOpen()
	{
		return false;
	}
}
