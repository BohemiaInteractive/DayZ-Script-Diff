class CraterellusMushroom : MushroomBase
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionCreateIndoorFireplace);
		AddAction(ActionCreateIndoorOven);
	}
	
	override void EEOnCECreate()
	{
		int rand = Math.RandomInt(0,10);
		float baseTemp = GetGame().GetMission().GetWorldData().GetBaseEnvTemperature();

		if ( baseTemp <= GameConstants.COLD_AREA_TEMPERATURE_THRESHOLD )
		{
			ChangeFoodStage( FoodStageType.RAW );
			SetHealth( "", "", GetMaxHealth()*1.0 );
		}
	}	
}
