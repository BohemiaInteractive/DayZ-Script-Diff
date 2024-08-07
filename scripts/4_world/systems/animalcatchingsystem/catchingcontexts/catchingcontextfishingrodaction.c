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
	
	//important items
	protected EntityAI m_Hook;
	protected EntityAI m_Bait;
	protected EntityAI m_Rod;
	
	override protected void Init(Param par)
	{
		super.Init(par);
		
		m_Rod = m_MainItem; //only stable one, rest initialized on 'InitItemValues' periodically
		m_Player = PlayerBase.Cast(m_MainItem.GetHierarchyRootPlayer());
		m_SignalDurationMin = UAFishingConstants.SIGNAL_DURATION_MIN_BASE;
		m_SignalDurationMax = UAFishingConstants.SIGNAL_DURATION_MAX_BASE;
		m_SignalStartTimeMin = UAFishingConstants.SIGNAL_START_TIME_MIN_BASE;
		m_SignalStartTimeMax = UAFishingConstants.SIGNAL_START_TIME_MAX_BASE;
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
		
		m_SignalPoissonMean = UAFishingConstants.SIGNAL_CYCLE_MEAN_DEFAULT;
		m_HookLossChanceMod = 0.0;
		m_BaitLossChanceMod = 0.0;
	}
	
	override protected void InitItemValues(EntityAI item)
	{
		//skip ruined or deleted items entirely
		if (item.IsRuined() || item.IsSetForDeletion())
			return;
		
		string path = "" + CFG_VEHICLESPATH + " " + item.GetType() + " Fishing";
		if (GetGame().ConfigIsExisting(path))
		{
			if (GetGame().ConfigIsExisting(path + " signalCycleTarget"))
				m_SignalPoissonMean = Math.Min(m_SignalPoissonMean,GetGame().ConfigGetFloat(path + " signalCycleTarget"));
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
		return m_Player.GetRandomGeneratorSyncManager().GetRandomInRange(RandomGeneratorSyncUsage.RGSAnimalCatching,m_SignalDurationMin,m_SignalDurationMax);
	}
	
	float RandomizeSignalStartTime()
	{
		return m_Player.GetRandomGeneratorSyncManager().GetRandomInRange(RandomGeneratorSyncUsage.RGSAnimalCatching,m_SignalStartTimeMin,m_SignalStartTimeMax);
	}
	
	override float GetChanceCoef()
	{
		return UAFishingConstants.SIGNAL_FISHING_CHANCE_COEF;
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
		UpdateCatchingItemData();
	}
	
	//! release without signal
	void OnSignalMiss()
	{
		TryHookLoss();
		TryBaitLoss();
		//RemoveItemSafe(m_Bait);
		//TryDamageItems();
		UpdateCatchingItemData();
	}
	
	void OnSignalPass()
	{
		//TryBaitLoss();
		RemoveItemSafe(m_Bait);
		TryDamageItems();
		UpdateCatchingItemData();
	}
}