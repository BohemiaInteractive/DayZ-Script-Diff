class CatchingContextFishingRodAction : CatchingContextFishingBase
{
	protected PlayerBase m_Player;
	//chances + modifiers
	protected float m_BaitLossChanceMod;
	protected float m_HookLossChanceMod;
	//times
	protected float m_SignalDurationMin; //seconds
	protected float m_SignalDurationMax; //seconds
	protected float m_SignalStartTimeMin; //seconds
	protected float m_SignalStartTimeMax; //seconds
	//signal targets
	protected float m_SignalCycleTarget;
	protected float m_SignalCycleEndTarget;
	protected float m_SignalCycleTargetAdjustment;
	protected float m_SignalCycleTargetEndAdjustment;
	//signal targets - constant
	protected float m_SignalTargetProbability;
	protected float m_SignalTargetEndProbability;
	//misc
	protected int m_SignalCurrent;
	
	//important items
	protected EntityAI m_Hook;
	protected EntityAI m_Bait;
	protected EntityAI m_Rod;
	
	override protected void Init(Param par)
	{
		super.Init(par);
		
		m_Rod = m_MainItem; //only stable one, rest initialized on 'InitItemValues' periodically
		m_Player = PlayerBase.Cast(m_MainItem.GetHierarchyRootPlayer());
	}
	
	override protected void InitCatchMethodMask()
	{
		m_MethodMask = AnimalCatchingConstants.MASK_METHOD_ROD;
	}
	
	override protected void InitCatchEnviroMask()
	{
		if (m_IsSea)
			m_EnviroMask = AnimalCatchingConstants.MASK_ENVIRO_SEA;
		else
			m_EnviroMask = AnimalCatchingConstants.MASK_ENVIRO_POND;
	}
	
	override protected void CreateResultDataStructure()
	{
		m_Result = new CarchingResultFishingAction(m_MainItem);
		
		super.CreateResultDataStructure();
	}
	
	//! only clear stuff you need to update
	override protected void ClearCatchingItemData()
	{
		super.ClearCatchingItemData();
		
		ResetSignalCounter();
		m_HookLossChanceMod = 0.0;
		m_BaitLossChanceMod = 0.0;
		m_SignalDurationMin = -1;
		m_SignalDurationMax = -1;
		m_SignalStartTimeMin = -1;
		m_SignalStartTimeMax = -1;
		m_SignalCycleTarget = UAFishingConstants.SIGNAL_CYCLE_MEAN_DEFAULT - 1; //lowered by '1', configured value is count
		m_SignalCycleEndTarget = UAFishingConstants.SIGNAL_CYCLE_HARD_TARGET_DEFAULT - 1; //lowered by '1', configured value is count
		m_SignalTargetProbability = UAFishingConstants.SIGNAL_MEAN_CHANCE_DEFAULT;
		m_SignalTargetEndProbability = UAFishingConstants.SIGNAL_HARD_TARGET_CHANCE_DEFAULT;
		m_SignalCycleTargetAdjustment = 0.0;
		m_SignalCycleTargetEndAdjustment = 0.0;
	}
	
	override void InitCatchingItemData()
	{
		super.InitCatchingItemData();
		
		//sanitize fishing values
		if (m_SignalDurationMin == -1) //if not set by any item
			m_SignalDurationMin = UAFishingConstants.SIGNAL_DURATION_MIN_BASE;
		else
			m_SignalDurationMin = Math.Clamp(m_SignalDurationMin,UAFishingConstants.SIGNAL_DURATION_MIN_BASE,UAFishingConstants.SIGNAL_DURATION_MAX_BASE);
		
		if (m_SignalDurationMax == -1) //if not set by any item
			m_SignalDurationMax = UAFishingConstants.SIGNAL_DURATION_MAX_BASE;
		else
			m_SignalDurationMax = Math.Clamp(m_SignalDurationMax,UAFishingConstants.SIGNAL_DURATION_MIN_BASE,UAFishingConstants.SIGNAL_DURATION_MAX_BASE);
		
		if (m_SignalStartTimeMin == -1) //if not set by any item (else already clamped)
			m_SignalStartTimeMin = UAFishingConstants.SIGNAL_START_TIME_MIN_BASE;
		
		if (m_SignalStartTimeMax == -1) //if not set by any item (else already clamped)
			m_SignalStartTimeMax = UAFishingConstants.SIGNAL_START_TIME_MAX_BASE;
		
		m_SignalCycleTarget = Math.Clamp((m_SignalCycleTarget + m_SignalCycleTargetAdjustment),0,float.MAX);
		m_SignalCycleEndTarget = Math.Clamp((m_SignalCycleEndTarget + m_SignalCycleTargetEndAdjustment),(m_SignalCycleTarget + 1),float.MAX);
		
		#ifdef DEVELOPER
		if (IsCLIParam("fishingLogs"))
		{
			Debug.Log("---InitCatchingItemData---","Fishing");
			Debug.Log("m_SignalCycleTarget (adjusted): " + m_SignalCycleTarget,"Fishing");
			Debug.Log("m_SignalCycleTargetAdjustment: " + m_SignalCycleTargetAdjustment,"Fishing");
			Debug.Log("m_SignalTargetProbability: " + m_SignalTargetProbability,"Fishing");
			Debug.Log("m_SignalCycleEndTarget (adjusted): " + m_SignalCycleEndTarget,"Fishing");
			Debug.Log("m_SignalCycleTargetEndAdjustment: " + m_SignalCycleTargetEndAdjustment,"Fishing");
			Debug.Log("m_SignalTargetEndProbability: " + m_SignalTargetEndProbability,"Fishing");
			Debug.Log("m_SignalDurationMin: " + m_SignalDurationMin,"Fishing");
			Debug.Log("m_SignalDurationMax: " + m_SignalDurationMax,"Fishing");
			Debug.Log("m_SignalStartTimeMin: " + m_SignalStartTimeMin,"Fishing");
			Debug.Log("m_SignalStartTimeMax: " + m_SignalStartTimeMax,"Fishing");
		}
		#endif
	}
	
	override protected void InitItemValues(EntityAI item)
	{
		//skip ruined or deleted items entirely
		if (item.IsRuined() || item.IsSetForDeletion())
			return;
		
		string path = "" + CFG_VEHICLESPATH + " " + item.GetType() + " Fishing";
		if (GetGame().ConfigIsExisting(path))
		{
			if (GetGame().ConfigIsExisting(path + " resultQuantityBaseMod"))
				m_QualityBaseMod += GetGame().ConfigGetFloat(path + " resultQuantityBaseMod");
			if (GetGame().ConfigIsExisting(path + " resultQuantityDispersionMin"))
				m_QualityDispersionMinMod += GetGame().ConfigGetFloat(path + " resultQuantityDispersionMin");
			if (GetGame().ConfigIsExisting(path + " resultQuantityDispersionMax"))
				m_QualityDispersionMaxMod += GetGame().ConfigGetFloat(path + " resultQuantityDispersionMax");
			if (GetGame().ConfigIsExisting(path + " hookLossChanceMod"))
				m_HookLossChanceMod += GetGame().ConfigGetFloat(path + " hookLossChanceMod");
			if (GetGame().ConfigIsExisting(path + " baitLossChanceMod"))
				m_BaitLossChanceMod += GetGame().ConfigGetFloat(path + " baitLossChanceMod");
			
			if (GetGame().ConfigIsExisting(path + " signalDurationMin"))
			{
				if (m_SignalDurationMin == -1)
					m_SignalDurationMin = 0;
				m_SignalDurationMin += GetGame().ConfigGetFloat(path + " signalDurationMin");
			}
			if (GetGame().ConfigIsExisting(path + " signalDurationMax"))
			{
				if (m_SignalDurationMax == -1)
					m_SignalDurationMax = 0;
				m_SignalDurationMax += GetGame().ConfigGetFloat(path + " signalDurationMax");
			}
			
			if (GetGame().ConfigIsExisting(path + " signalCycleTargetAdjustment"))
				m_SignalCycleTargetAdjustment += GetGame().ConfigGetFloat(path + " signalCycleTargetAdjustment");
			if (GetGame().ConfigIsExisting(path + " signalCycleTargetEndAdjustment"))
				m_SignalCycleTargetEndAdjustment += GetGame().ConfigGetFloat(path + " signalCycleTargetEndAdjustment");
			
			int slotID;
			string slotName;
			if (item.GetInventory().GetCurrentAttachmentSlotInfo(slotID,slotName))
			{
				switch (slotName)
				{
					case "Bait":
						m_Bait = item;
						break;
					
					case "Hook":
						m_Hook = item;
						break;
				}
			}
		}
	}
	
	override bool ModifySignalProbability(inout float probability)
	{
		float easingTime;
		if ((float)m_SignalCurrent < m_SignalCycleTarget)
		{
			easingTime = Math.InverseLerp(0,m_SignalCycleTarget,(float)m_SignalCurrent);
			probability = Easing.EaseInExpo(easingTime) * m_SignalTargetProbability * GetChanceCoef();
		}
		else
		{
			easingTime = Math.InverseLerp(m_SignalCycleTarget,m_SignalCycleEndTarget,(float)m_SignalCurrent);
			probability = (m_SignalTargetProbability + (Easing.EaseInExpo(easingTime) * (m_SignalTargetEndProbability - m_SignalTargetProbability))) * GetChanceCoef();
		}
		
		#ifdef DEVELOPER
		if (IsCLIParam("fishingLogs"))
		{
			Debug.Log("---ModifySignalProbability---","Fishing");
			Debug.Log("m_SignalCurrent: " + m_SignalCurrent,"Fishing");
			Debug.Log("easingTime: " + easingTime,"Fishing");
			Debug.Log("probability: " + probability,"Fishing");
		}
		#endif
		
		return true;
	}
	
	float GetChanceCoef()
	{
		return UAFishingConstants.SIGNAL_FISHING_CHANCE_COEF;
	}
	
	override bool RollCatch()
	{
		bool ret = super.RollCatch();
		m_SignalCurrent++;
		return ret;
	}
	
	protected void ResetSignalCounter()
	{
		m_SignalCurrent = 0.0;
	}
	
	//! done locally on both sides, needs a synced random
	override void GenerateResult()
	{
		YieldItemBase yItem;
		int idx = m_Player.GetRandomGeneratorSyncManager().GetRandomInRange(RandomGeneratorSyncUsage.RGSAnimalCatching,0,m_ProbabilityArray.Count() - 1);
		Class.CastTo(yItem,m_YieldsMapAll.Get(m_ProbabilityArray[idx]));
		m_Result.SetYieldItem(yItem);
	}
	
	int GetResultParticleId()
	{
		if (m_Result)
			return m_Result.GetYieldItemParticleId();
		
		return ParticleList.INVALID;
	}
	
	override protected void RecalculateProcessingData()
	{
		m_Result.UpdateCatchChance(this);
	}
	
	float GetHookLossChanceModifierClamped()
	{
		return Math.Clamp(m_HookLossChanceMod,0.001,1);
	}
	
	float GetBaitLossChanceModifierClamped()
	{
		return Math.Clamp(m_BaitLossChanceMod,0,1);
	}
	
	float GetActionCycleTime()
	{
		return CarchingResultFishingAction.Cast(m_Result).GetCurrentCycleTime(this);
	}
	
	float RandomizeSignalDuration()
	{
		float res = m_Player.GetRandomGeneratorSyncManager().GetRandomInRange(RandomGeneratorSyncUsage.RGSAnimalCatching,m_SignalDurationMin,m_SignalDurationMax);
		#ifdef DEVELOPER
		if (IsCLIParam("fishingLogs"))
		{
			Debug.Log("---RandomizeSignalDuration---","Fishing");
			Debug.Log("next signal duration: " + res,"Fishing");
		}
		#endif
		
		return res;
	}
	
	float RandomizeSignalStartTime()
	{
		float res = m_Player.GetRandomGeneratorSyncManager().GetRandomInRange(RandomGeneratorSyncUsage.RGSAnimalCatching,m_SignalStartTimeMin,m_SignalStartTimeMax);
		#ifdef DEVELOPER
		if (IsCLIParam("fishingLogs"))
		{
			Debug.Log("---RandomizeSignalStartTime---","Fishing");
			Debug.Log("next signal start time: " + res,"Fishing");
		}
		#endif
		return res;
	}
	
	protected void TryHookLoss()
	{
		if (m_Hook && !m_Hook.IsSetForDeletion())
		{
			float lossChance = GetHookLossChanceModifierClamped();
			if (lossChance <= 0)
				return;
			
			float roll = m_Player.GetRandomGeneratorSyncManager().GetRandom01(RandomGeneratorSyncUsage.RGSAnimalCatching);
			
			if (lossChance >= 1 || roll < lossChance)
			{
				RemoveItemSafe(m_Hook);
			}
		}
	}
	
	protected void RemoveItemSafe(EntityAI item)
	{
		if (item && !m_Player.IsQuickFishing())
		{
			item.SetPrepareToDelete(); //should probably flag the bait too, but the action terminates anyway
			item.DeleteSafe();
		}
	}
	
	protected void TryBaitLoss()
	{
		if (m_Bait && !m_Bait.IsSetForDeletion())
		{
			float lossChance = GetBaitLossChanceModifierClamped();
			if (lossChance <= 0)
				return;
			
			float roll = m_Player.GetRandomGeneratorSyncManager().GetRandom01(RandomGeneratorSyncUsage.RGSAnimalCatching);
			
			if (lossChance >= 1 || roll < lossChance)
			{
				RemoveItemSafe(m_Bait);
			}
		}
	}
	
	protected void TryDamageItems()
	{
		if (!GetGame().IsMultiplayer() || GetGame().IsDedicatedServer())
		{
			if (m_Hook && !m_Hook.IsSetForDeletion())
				m_Hook.AddHealth("","Health",-UAFishingConstants.DAMAGE_HOOK);
		}
	}
	
	override EntityAI SpawnAndSetupCatch(out int yItemIdx, vector v = vector.Zero)
	{
		//hook check on catch
		if (m_Hook && !m_Hook.IsSetForDeletion())
			return super.SpawnAndSetupCatch(yItemIdx,v);
		return null;
	}
	
	//events
	void OnBeforeSpawnSignalHit()
	{
		TryHookLoss();
	}
	
	void OnAfterSpawnSignalHit()
	{
		RemoveItemSafe(m_Bait);
		TryDamageItems();
	}
	
	//! release without signal
	void OnSignalMiss()
	{
		TryHookLoss();
		TryBaitLoss();
	}
	
	void OnSignalPass()
	{
		RemoveItemSafe(m_Bait);
		TryDamageItems();
		UpdateCatchingItemData();
	}
	
	///////////////////////////////
	//Fish pen of deprecated code//
	///////////////////////////////
	protected float m_SignalPoissonMean = AnimalCatchingConstants.POISSON_CYCLE_MEAN_DEFAULT; //!Deprecated, left here due to inheritance change
	float GetSignalPoissonMean() {return m_SignalPoissonMean;} //!Deprecated, left here due to inheritance change
	int GetSignalMax() {return Math.Ceil(m_SignalPoissonMean) + Math.Ceil(m_SignalPoissonMean/5);} //!Deprecated, left here due to inheritance change
}