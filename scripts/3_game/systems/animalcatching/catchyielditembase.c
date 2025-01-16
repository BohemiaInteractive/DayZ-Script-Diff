class YieldItemBase
{
	protected string m_Type;
	protected int m_RegistrationIdx = -1; //mostly for sync purposes
	protected int m_BaseWeight; //occurence ratio in the enviroment
	protected float m_QualityBase = AnimalCatchingConstants.QUALITY_GENERIC_BASE;
	protected int m_EnviroMask; //which enviroment it is relevant for
	protected int m_MethodMask; //which catching method are we using
	//protected ref  m_BaitCompatibilityMask; //which baits are relevant for me
	
	void YieldItemBase(int baseWeight)
	{
		m_BaseWeight = baseWeight;
		
		Init();
	}
	
	void Init();
	
	string GetType()
	{
		return m_Type;
	}
	
	int GetRegistrationIdx()
	{
		return m_RegistrationIdx;
	}
	
	void SetRegistrationIdx(int idx)
	{
		if (m_RegistrationIdx == -1)
			m_RegistrationIdx = idx;
		else
			ErrorEx("m_RegistrationIdx already set for " + this);
	}
	
	int GetYieldWeight(CatchingContextBase ctx)
	{
		return m_BaseWeight;
	}
	
	int GetEnviroMask()
	{
		return m_EnviroMask;
	}
	
	int GetMethodMask()
	{
		return m_MethodMask;
	}
	
	float GetBaitTypeSensitivity(ECatchingBaitCategories type)
	{
		return 1.0;
	}
	
	float GetQualityForYieldItem(CatchingContextBase ctx)
	{
		float res = m_QualityBase;
		res += ctx.GetQualityModifier();
		
		return res;
	}
	
	//! called on item spawn
	void OnEntityYieldSpawned(EntityAI spawn);
	
	float GetChanceForYieldItem(CatchingContextBase ctx)
	{
		float ret = 1.0;
		//ctx.UpdateBaseProbability(this);
		ctx.ModifySignalProbability(ret);
		return ret;
	}
	
	////////////////
	//effects info
	//////////////
	
	string GetCatchDeathSoundset()
	{
		return "";
	}
	
	string GetCatchAINoise()
	{
		return "";
	}
	
	float GetCatchAINoiseBaseStrength()
	{
		return 1.0;
	}
	
	int GetCatchParticleID()
	{
		return ParticleList.INVALID;
	}
}