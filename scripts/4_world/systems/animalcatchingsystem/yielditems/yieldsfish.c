class FishYieldItemBase : YieldItemBase
{
	//used for interpolation between min/max times (UAFishingConstants.CYCLE_LENGTH_MIN/UAFishingConstants.CYCLE_LENGTH_MAX)
	protected float m_HourlyCycleLengthCoefs[24]; //0 - 23h;
	
	override void Init()
	{
		super.Init();
		
		m_HourlyCycleLengthCoefs = {1,1,1,1,0.5,0.2,0,0,0,0.2,0.5,1,1/*12h*/,1,1,1,0.75,0.5,0.2,0,0,0,0.2,0.5};
		m_QualityBase = AnimalCatchingConstants.QUALITY_FISH_BASE; //base quality of the fish
	}
	
	float GetCycleTimeForYieldItem(CatchingContextFishingRodAction ctx)
	{
		int hour = Math.Floor(GetGame().GetDayTime());
		float res = Math.Lerp(UAFishingConstants.CYCLE_LENGTH_MIN,UAFishingConstants.CYCLE_LENGTH_MAX,m_HourlyCycleLengthCoefs[hour]);
		return res;
	}
	
	override float GetBaitTypeSensitivity(ECatchingBaitCategories type)
	{
		switch (type)
		{
			case ECatchingBaitCategories.BAIT_TYPE_EMPTY:
			case ECatchingBaitCategories.BAIT_TYPE_MEAT_SMALL:
				return 1;
		}
		
		return 0.0;
	}
	
	override string GetCatchDeathSoundset()
	{
		return "Fishing_splash_SoundSet";
	}
	
	override string GetCatchAINoise()
	{
		return "";
	}
	
	override int GetCatchParticleID()
	{
		return ParticleList.FISHING_SIGNAL_SPLASH;
	}
}

//----------------------------------//
//generic fish with basic info only //
//----------------------------------//
class YieldItemGenericFish : FishYieldItemBase
{
	void YieldItemGenericFish(int baseWeight, string type, int enviroMask, int methodMask)
	{
		m_Type = type;
		m_EnviroMask = enviroMask;
		m_MethodMask = methodMask;
	}
}

//------------------------------//
//detailed yield items over nyah//
//------------------------------//
//--------------//
//temperate fish//
//--------------//
class YieldItemMackerel : FishYieldItemBase
{
	override void Init()
	{
		super.Init();
		
		//m_HourlyCycleLengthCoefs = {1,1,1,1,1,1,1,1,1,1,1,1,1/*12h*/,1,1,1,1,1,1,1,1,1,1,1};
		//m_QualityBase = some custom value here;
		m_Type = "Mackerel";
		m_EnviroMask = AnimalCatchingConstants.MASK_ENVIRO_SEA;
		m_MethodMask = AnimalCatchingConstants.MASK_METHOD_ROD|AnimalCatchingConstants.MASK_METHOD_FISHTRAP_LARGE;
	}
}

class YieldItemCarp : FishYieldItemBase
{
	override void Init()
	{
		super.Init();
		
		//m_HourlyCycleLengthCoefs = {1,1,1,1,1,1,1,1,1,1,1,1,1/*12h*/,1,1,1,1,1,1,1,1,1,1,1};
		//m_QualityBase = some custom value here;
		m_Type = "Carp";
		m_EnviroMask = AnimalCatchingConstants.MASK_ENVIRO_POND;
		m_MethodMask = AnimalCatchingConstants.MASK_METHOD_ROD|AnimalCatchingConstants.MASK_METHOD_FISHTRAP_LARGE;
	}
}

class YieldItemSardines : FishYieldItemBase
{
	override void Init()
	{
		super.Init();
		
		//m_HourlyCycleLengthCoefs = {1,1,1,1,1,1,1,1,1,1,1,1,1/*12h*/,1,1,1,1,1,1,1,1,1,1,1};
		//m_QualityBase = some custom value here;
		m_Type = "Sardines";
		m_EnviroMask = AnimalCatchingConstants.MASK_ENVIRO_SEA;
		m_MethodMask = AnimalCatchingConstants.MASK_METHOD_FISHTRAP_SMALL;
	}
}

class YieldItemBitterlings : FishYieldItemBase
{
	override void Init()
	{
		super.Init();
		
		//m_HourlyCycleLengthCoefs = {1,1,1,1,1,1,1,1,1,1,1,1,1/*12h*/,1,1,1,1,1,1,1,1,1,1,1};
		//m_QualityBase = some custom value here;
		m_Type = "Bitterlings";
		m_EnviroMask = AnimalCatchingConstants.MASK_ENVIRO_POND;
		m_MethodMask = AnimalCatchingConstants.MASK_METHOD_FISHTRAP_SMALL;
	}
}

//---------//
//cool fish//
//---------//
class YieldItemWalleyePollock : FishYieldItemBase
{
	override void Init()
	{
		super.Init();
		
		m_HourlyCycleLengthCoefs = {1,1,1,1,1,1,1,0.8,0.7,0.5,0.2,0,0/*12h*/,0,0,0,0.2,0.5,0.7,0.8,1,1,1,1};
		//m_QualityBase = some custom value here;
		m_Type = "WalleyePollock";
		m_EnviroMask = AnimalCatchingConstants.MASK_ENVIRO_SEA;
		m_MethodMask = AnimalCatchingConstants.MASK_METHOD_ROD|AnimalCatchingConstants.MASK_METHOD_FISHTRAP_LARGE;
	}
}

class YieldItemSteelheadTrout : FishYieldItemBase
{
	override void Init()
	{
		super.Init();
		
		m_HourlyCycleLengthCoefs = {1,1,1,1,1,1,1,1,1,0.5,0.2,0,0/*12h*/,0,0,0.2,0.5,0.7,1,1,1,1,1,1};
		//m_QualityBase = some custom value here;
		m_Type = "SteelheadTrout";
		m_EnviroMask = AnimalCatchingConstants.MASK_ENVIRO_SEA;
		m_MethodMask = AnimalCatchingConstants.MASK_METHOD_ROD|AnimalCatchingConstants.MASK_METHOD_FISHTRAP_LARGE;
	}
}

class YieldItemShrimp : FishYieldItemBase
{
	override void Init()
	{
		super.Init();
		
		m_HourlyCycleLengthCoefs = {1,1,1,1,1,1,1,1,1,1,0,0,0/*12h*/,0,0,0,0,1,1,1,1,1,1,1};
		//m_QualityBase = some custom value here;
		m_Type = "Shrimp";
		m_EnviroMask = AnimalCatchingConstants.MASK_ENVIRO_SEA;
		m_MethodMask = AnimalCatchingConstants.MASK_METHOD_FISHTRAP_SMALL;
	}
}