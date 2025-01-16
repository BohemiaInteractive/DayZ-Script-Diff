class CatchingContextTrapsBase : CatchingContextBase
{
	/**@var			m_CumulativeTrappingSuccess
	 * @brief		after N attempts, the chance to catch should be this. Only highest one applies.
	 * @NOTE:		Take care, only way to achieve guaranteed 1.0 chance at the end is to have EVERY chance at 1.0.
	 **/
	protected float m_CumulativeTrappingSuccess;
	protected int m_AttemptsCount;
	protected ItemBase m_Bait;
	
	override protected void DeserializeData(Param par)
	{
		Param2<EntityAI,int> p;
		if (Class.CastTo(p,par))
		{
			m_MainItem = p.param1;
			m_AttemptsCount = p.param2;
		}
	}
	
	override protected void CreateResultDataStructure()
	{
		m_Result = new CatchingResultTrapBase(m_MainItem);
		
		super.CreateResultDataStructure();
	}
	
	override protected void ClearCatchingItemData()
	{
		super.ClearCatchingItemData();
		
		m_CumulativeTrappingSuccess = 0.0;
		m_QualityBaseMod = 0.0;
		m_QualityDispersionMinMod = 0.0;
		m_QualityDispersionMaxMod = 0.0; 
	}
	
	override protected void InitItemValues(EntityAI item)
	{
		//skip ruined or deleted items entirely
		if (item.IsRuined() || item.IsSetForDeletion())
			return;
		
		string path = "" + CFG_VEHICLESPATH + " " + item.GetType() + " Trapping";
		if (GetGame().ConfigIsExisting(path))
		{
			if (GetGame().ConfigIsExisting(path + " baitTypes") && GetGame().ConfigIsExisting(path + " baitTypeChances"))
			{
				CachedObjectsArrays.ARRAY_INT.Clear();
				CachedObjectsArrays.ARRAY_FLOAT.Clear();
				GetGame().ConfigGetIntArray(path + " baitTypes",CachedObjectsArrays.ARRAY_INT);
				GetGame().ConfigGetFloatArray(path + " baitTypeChances",CachedObjectsArrays.ARRAY_FLOAT);
				int count = CachedObjectsArrays.ARRAY_INT.Count();
				if (count == CachedObjectsArrays.ARRAY_FLOAT.Count())
				{
					int key;
					float value;
					for (int i = 0; i < count; i++)
					{
						key = CachedObjectsArrays.ARRAY_INT[i];
						value = AdjustBaitItemChance(item) * CachedObjectsArrays.ARRAY_FLOAT[i];
						if (!m_BaitCompatibilityMap.Contains(key) || (m_BaitCompatibilityMap.Get(key).m_BaseProbability < value))
						{
							m_BaitCompatibilityMap.Set(key,new BaitData(value,item));
						}
					}
				}
				else
				{
					ErrorEx("'baitTypes' and 'baitTypeChances' arrray counts of " + item.GetType() + " do not match!",ErrorExSeverity.INFO);
				}
			}
			if (GetGame().ConfigIsExisting(path + " resultQuantityBaseMod"))
				m_QualityBaseMod += GetGame().ConfigGetFloat(path + " resultQuantityBaseMod");
			if (GetGame().ConfigIsExisting(path + " resultQuantityDispersionMin"))
				m_QualityDispersionMinMod += GetGame().ConfigGetFloat(path + " resultQuantityDispersionMin");
			if (GetGame().ConfigIsExisting(path + " resultQuantityDispersionMax"))
				m_QualityDispersionMaxMod += GetGame().ConfigGetFloat(path + " resultQuantityDispersionMax");
		}
	}
	
	//! Allows for adjustment of all catch probabilities from item qualities (damage, qty...)
	float AdjustBaitItemChance(EntityAI item)
	{
		float ret = 1.0;
		ItemBase ib;
		if (Class.CastTo(ib,item))
		{
			//ret *= ib.GetBaitEffectivity(); //disconnected for the Time Being
		}
		
		return ret;
	}
	
	override protected void InitCatchEnviroMask()
	{
		vector pos = m_MainItem.GetPosition();
		if (GetGame().SurfaceIsSea(pos[0], pos[2]))
			m_EnviroMask = AnimalCatchingConstants.MASK_ENVIRO_SEA;
		else if (GetGame().SurfaceIsPond( pos[0], pos[2]))
			m_EnviroMask = AnimalCatchingConstants.MASK_ENVIRO_POND;
		else
			m_EnviroMask = AnimalCatchingConstants.MASK_ENVIRO_LAND_ALL;
	}
	
	override protected void Init(Param par)
	{
		super.Init(par);
		
		GenerateResult();
	}
	
	override protected void SetupInitialTypes()
	{
		#ifdef DEVELOPER
		if (IsCLIParam("catchingLogs"))
		{
			Print("********************"); //just for debug purposes to track the start
			Print("dbgTrapz | START");
			Print("********************");
		}
		#endif
		
		super.SetupInitialTypes();
		
		#ifdef DEVELOPER
		if (IsCLIParam("catchingLogs"))
		{
			Print("***"); //just for debug purposes to track the start
		}
		#endif
	}
	
	void UpdateDataAndMasks()
	{
		InitCatchingItemData();
		InitCatchMethodMask(); //bait check
		//InitCatchEnviroMask(); //skipping, raycasts are unreliable outside network bubbles
		SetupInitialTypes();
		SetupProbabilityArray();
	}
	
	int UpdateTrapEnviroMask()
	{
		InitCatchEnviroMask();
		return GetCatchEnviroMask();
	}
	
	void SetTrapEnviroMask(int value)
	{
		m_EnviroMask = value;
	}
	
	//! if non-empty bait type is used, some 'Bait' attachment is picked as an active bait (currently no direct link between item and sensitivity-weighted target probability)
	void UpdateUsedBait(ECatchingBaitCategories type)
	{
		m_Bait = null;
		if (type != ECatchingBaitCategories.BAIT_TYPE_EMPTY)
		{
			BaitData dta = m_BaitCompatibilityMap.Get(type);
			if (dta)
			{
				ItemBase potentialBait = ItemBase.Cast(dta.m_Owner);
				if (potentialBait != m_MainItem)
					m_Bait = potentialBait;
			}
			else
				ErrorEx("failed to acquire BaitData from type: " + type);
		}
		
		#ifdef DEVELOPER
		if (IsCLIParam("catchingLogs"))
		{
			if (m_Bait)
				Print("dbgTrapz | UpdateUsedBait to: " + m_Bait + " | with base bait probability used: " + m_BaitCompatibilityMap.Get(type).m_BaseProbability);
			else
				Print("dbgTrapz | UpdateUsedBait to 'null'!");
		}
		#endif
	}
	
	override void UpdateBaseProbability(YieldItemBase yItem)
	{
		int baitType = ECatchingBaitCategories.BAIT_TYPE_EMPTY;
		int usedType = -1;
		float probability = -1;
		float highestProbability = 0.0;
		int count = m_BaitCompatibilityMap.Count();
		for (int i = 0; i < count; i++)
		{
			baitType = m_BaitCompatibilityMap.GetKey(i);
			probability = m_BaitCompatibilityMap.Get(baitType).m_BaseProbability * yItem.GetBaitTypeSensitivity(baitType);
			if (probability > highestProbability)
			{
				highestProbability = probability;
				usedType = baitType;
			}
		}
		
		m_CumulativeTrappingSuccess = Math.Clamp(highestProbability,0,1);
		#ifdef DEVELOPER
		if (IsCLIParam("catchingLogs"))
		{
			//Print("********************");
			Print("dbgTrapz | using bait type: " + baitType + " to catch: " + yItem);
		}
		#endif
		UpdateUsedBait(usedType);
		
		#ifdef DEVELOPER
		if (IsCLIParam("catchingLogs"))
		{
			float dbgProb;
			ModifySignalProbability(dbgProb);
			Print("dbgTrapz | starting catching of " + yItem + " | at probability: " + dbgProb + " | with target success: " + m_CumulativeTrappingSuccess);
		}
		#endif
	}
	
	override bool ModifySignalProbability(inout float probability)
	{
		probability = 1 - Math.Pow((1 - m_CumulativeTrappingSuccess),(1/m_AttemptsCount));
		
		return true;
	}
	
	void RemoveBait()
	{
		if (m_Bait)
			m_Bait.DeleteSafe();
	}
	
	void ReduceBaitQty(float qtyNorm)
	{
		if (m_Bait)
		{
			if (m_Bait.HasQuantity() && !m_Bait.IsSplitable())
				m_Bait.SetQuantityNormalized((m_Bait.GetQuantityNormalized() - qtyNorm));
			else
				m_Bait.DeleteSafe();
		}
	}
}

class CatchingContextTrapFishSmall : CatchingContextTrapsBase
{
	override protected void InitCatchMethodMask()
	{
		m_MethodMask = AnimalCatchingConstants.MASK_METHOD_FISHTRAP_SMALL;
	}
}

class CatchingContextTrapFishLarge : CatchingContextTrapsBase
{
	override protected void InitCatchMethodMask()
	{
		m_MethodMask = AnimalCatchingConstants.MASK_METHOD_FISHTRAP_LARGE;
	}
}

class CatchingContextTrapLandSnare : CatchingContextTrapsBase
{
	override protected void InitCatchMethodMask()
	{
		m_MethodMask = AnimalCatchingConstants.MASK_METHOD_LANDTRAP_SNARE;
	}
}
