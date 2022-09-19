class Headtorch_Black : Headtorch_ColorBase 
{
	override void OnLightCreated()
	{
		m_Light.SetColorToRed();
	}

	override void OnDebugSpawn()
	{
		Battery9V.Cast(GetInventory().CreateInInventory("Battery9V"));
	}
}