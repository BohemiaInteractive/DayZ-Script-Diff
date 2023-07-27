class Headtorch_Grey : Headtorch_ColorBase 
{
	override void OnDebugSpawn()
	{
		GetInventory().CreateInInventory("Battery9V");
	}
}
