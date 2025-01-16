////////////////////////////////////////////////////////
class YieldItemJunk : FishYieldItemBase
{
	void YieldItemJunk(int baseWeight, string type)
	{
		m_Type = type;
	}
	
	override void Init()
	{
		super.Init();
		
		m_EnviroMask = AnimalCatchingConstants.MASK_ENVIRO_WATER_ALL;
		m_MethodMask = AnimalCatchingConstants.MASK_METHOD_ROD;
	}
	
	override float GetChanceForYieldItem(CatchingContextBase ctx)
	{
		return 0.20; //not affected by equipment or time, just some flat number
	}
	
	override float GetCycleTimeForYieldItem(CatchingContextFishingRodAction ctx)
	{
		return UAFishingConstants.CYCLE_LENGTH_MAX;
	}
	
	override void OnEntityYieldSpawned(EntityAI spawn)
	{
		super.OnEntityYieldSpawned(spawn);
		
		spawn.SetHealthLevel(GameConstants.STATE_BADLY_DAMAGED,"");
	}
	
	override string GetCatchDeathSoundset()
	{
		return "";
	}
}

class YieldItemJunkEmpty : YieldItemJunk
{
	//empty liquid containers
	override float GetQualityForYieldItem(CatchingContextBase ctx)
	{
		return 0;
	}
}