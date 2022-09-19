class SpaghettiCan : Edible_Base
{
	override void Open()
	{
		ReplaceEdibleWithNew("SpaghettiCan_Opened");
	}
	
	override bool IsOpen()
	{
		return false;
	}
}
