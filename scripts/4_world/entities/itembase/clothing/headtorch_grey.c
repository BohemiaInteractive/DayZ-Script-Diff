class Headtorch_Grey : Headtorch_ColorBase 
{
	override void OnDebugSpawn()
	{
		Battery9V.Cast(GetInventory().CreateInInventory("Battery9V"));
	}
};