class LandAnimalYieldItemBase : YieldItemBase {}

//------------------------------------//
//generic animal with basic info only //
//------------------------------------//
class YieldItemGenericSmallAnimal : LandAnimalYieldItemBase
{
	void YieldItemGenericSmallAnimal(int baseWeight, string type, int enviroMask, int methodMask)
	{
		m_Type = type;
		m_EnviroMask = enviroMask;
		m_MethodMask = methodMask;
	}
}

//mammals
class YieldItemDeadRabbit : LandAnimalYieldItemBase
{
	override void Init()
	{
		super.Init();
		
		m_Type = "DeadRabbit";
		m_EnviroMask = AnimalCatchingConstants.MASK_ENVIRO_FOREST;
		m_MethodMask = AnimalCatchingConstants.MASK_METHOD_LANDTRAP_SNARE;
	}
	
	override float GetBaitTypeSensitivity(ECatchingBaitCategories type)
	{
		switch (type)
		{
			case ECatchingBaitCategories.BAIT_TYPE_EMPTY:
			case ECatchingBaitCategories.BAIT_TYPE_PLANT:
				return 1.0;
			
			/*
			//We can define 'offputing' baits the animal would actually AVOID
			case ECatchingBaitCategories.BAIT_TYPE_MEAT_SMALL:
			case ECatchingBaitCategories.BAIT_TYPE_MEAT_LARGE:
				return -1;
			*/
		}
		
		return 0.0;
	}
	
	override string GetCatchDeathSoundset()
	{
		return "BrownHareDeath_SoundSet";
	}
	
	override string GetCatchAINoise()
	{
		return "WolfRoarNoise";
	}
	
	override float GetCatchAINoiseBaseStrength()
	{
		return 1.0;
	}
}

class YieldItemDeadFox : LandAnimalYieldItemBase
{
	override void Init()
	{
		super.Init();
		
		m_Type = "DeadFox";
		m_EnviroMask = AnimalCatchingConstants.MASK_ENVIRO_FOREST;
		m_MethodMask = AnimalCatchingConstants.MASK_METHOD_LANDTRAP_SNARE;
	}
	
	override float GetBaitTypeSensitivity(ECatchingBaitCategories type)
	{
		switch (type)
		{
			case ECatchingBaitCategories.BAIT_TYPE_EMPTY:
			case ECatchingBaitCategories.BAIT_TYPE_MUSHROOM:
			case ECatchingBaitCategories.BAIT_TYPE_MEAT_SMALL:
			case ECatchingBaitCategories.BAIT_TYPE_MEAT_LARGE:
				return 1.0;
		}
		
		return 0.0;
	}
	
	override string GetCatchDeathSoundset()
	{
		return "Red_Fox_Death_SoundSet";
	}
	
	override string GetCatchAINoise()
	{
		return "WolfRoarNoise";
	}
	
	override float GetCatchAINoiseBaseStrength()
	{
		return 1.0;
	}
}

//poultry
class YieldItemCapraHircusBase : LandAnimalYieldItemBase
{
	override void Init()
	{
		super.Init();
		
		m_EnviroMask = AnimalCatchingConstants.MASK_ENVIRO_FIELD;
		m_MethodMask = AnimalCatchingConstants.MASK_METHOD_LANDTRAP_SNARE;
	}
	
	override float GetBaitTypeSensitivity(ECatchingBaitCategories type)
	{
		switch (type)
		{
			case ECatchingBaitCategories.BAIT_TYPE_EMPTY:
			case ECatchingBaitCategories.BAIT_TYPE_PLANT:
			case ECatchingBaitCategories.BAIT_TYPE_SEED:
			case ECatchingBaitCategories.BAIT_TYPE_MUSHROOM:
			case ECatchingBaitCategories.BAIT_TYPE_MEAT_SMALL:
				return 1.0;
		}
		
		return 0.0;
	}
	
	override string GetCatchDeathSoundset()
	{
		return "HenScream_SoundSet";
	}
	
	override string GetCatchAINoise()
	{
		return "WolfRoarNoise";
	}
	
	override float GetCatchAINoiseBaseStrength()
	{
		return 1.0;
	}
}

class YieldItemDeadRooster : YieldItemCapraHircusBase
{
	override void Init()
	{
		super.Init();
		
		m_Type = "DeadRooster";
	}
}
class YieldItemDeadChicken_White : YieldItemCapraHircusBase
{
	override void Init()
	{
		super.Init();
		
		m_Type = "DeadChicken_White";
	}
}
class YieldItemDeadChicken_Spotted : YieldItemCapraHircusBase
{
	override void Init()
	{
		super.Init();
		
		m_Type = "DeadChicken_Spotted";
	}
}
class YieldItemDeadChicken_Brown : YieldItemCapraHircusBase
{
	override void Init()
	{
		super.Init();
		
		m_Type = "DeadChicken_Brown";
	}
}