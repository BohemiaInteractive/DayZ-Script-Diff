class SardinesCan : Edible_Base
{
	override void Open()
	{
		ReplaceEdibleWithNew("SardinesCan_Opened");
	}
	
	override bool IsOpen()
	{
		return false;
	}

	override float GetQuantityNormalizedScripted()
	{
		return 1.0;
	}
}
