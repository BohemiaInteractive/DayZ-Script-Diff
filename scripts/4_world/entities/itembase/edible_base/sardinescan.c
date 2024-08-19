class SardinesCan : Edible_Base
{
	override void Open()
	{
		//super.Open();
		ReplaceEdibleWithNew("SardinesCan_Opened");
	}
	
	override bool IsOpen()
	{
		return false;
	}
}
