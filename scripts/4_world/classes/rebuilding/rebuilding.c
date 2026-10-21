class Rebuilding : ConstructionBase
{
	static int REBUILDING_STORAGE_VERSION = 1;
	
	protected int m_LastStorageVersion;
	
	protected int m_SyncParts1;
	protected int m_SyncParts2;
	protected int m_SyncParts3;
	protected int m_SyncParts4;
	protected int m_SyncParts5;
	protected int m_SyncParts6;
	protected int m_SyncParts7;
	protected int m_SyncParts8;
	protected int m_SyncParts9;
	protected int m_SyncParts10;
	protected int m_SyncParts1Local;
	protected int m_SyncParts2Local;
	protected int m_SyncParts3Local;
	protected int m_SyncParts4Local;
	protected int m_SyncParts5Local;
	protected int m_SyncParts6Local;
	protected int m_SyncParts7Local;
	protected int m_SyncParts8Local;
	protected int m_SyncParts9Local;
	protected int m_SyncParts10Local;
	
	protected ref map<int, int> m_SupportRequirement;
	protected ref map<int, int> m_SupportFulfillment;
	
	//BuildPart
	override void BuildPartServerEx(Man player, string part_name, int action_id)
	{
		super.BuildPartServerEx(player,part_name,action_id);
		
		ProcessOnPartBuiltServer(player, part_name, action_id);
	}
	
	//DismantlePart
	override void DismantlePartServerEx(Man player, string part_name, int action_id)
	{
		super.DismantlePartServerEx(player,part_name,action_id);
		
		ProcessOnPartDismantledServer(player, part_name, action_id);
	}
	
	//DestroyPart
	override void DestroyPartServer(Man player, string part_name, int action_id, bool destroyed_by_connected_part = false)
	{
		//set DamageZone health to zero
		string damage_zone;
		if (DamageSystem.GetDamageZoneFromComponentName(m_EntityParent, part_name, damage_zone) && m_EntityParent.GetHealth(damage_zone, "Health") > 0)
		{
			m_EntityParent.SetHealth(damage_zone, "Health", 0);
		}
		
		super.DestroyPartServer(player,part_name,action_id,destroyed_by_connected_part);
		
		ProcessOnPartDestroyedServer(player, part_name, action_id, destroyed_by_connected_part);
	}
	
	//============================================
	// Syncing
	//============================================
	
	override void OnConstructionVariablesSynchronized()
	{
		super.OnConstructionVariablesSynchronized();
		
		HandleSyncedParts();
	}
	
	protected void HandleSyncedParts()
	{
		bool changed = false;
		if (m_SyncParts1Local != m_SyncParts1)
		{
			m_SyncParts1Local = m_SyncParts1;
			changed = true;
		}
		if (m_SyncParts2Local != m_SyncParts2)
		{
			m_SyncParts2Local = m_SyncParts2;
			changed = true;
		}
		if (m_SyncParts3Local != m_SyncParts3)
		{
			m_SyncParts3Local = m_SyncParts3;
			changed = true;
		}
		if (m_SyncParts4Local != m_SyncParts4)
		{
			m_SyncParts4Local = m_SyncParts4;
			changed = true;
		}
		if (m_SyncParts5Local != m_SyncParts5)
		{
			m_SyncParts5Local = m_SyncParts5;
			changed = true;
		}
		if (m_SyncParts6Local != m_SyncParts6)
		{
			m_SyncParts6Local = m_SyncParts6;
			changed = true;
		}
		if (m_SyncParts7Local != m_SyncParts7)
		{
			m_SyncParts7Local = m_SyncParts7;
			changed = true;
		}
		if (m_SyncParts8Local != m_SyncParts8)
		{
			m_SyncParts8Local = m_SyncParts8;
			changed = true;
		}
		if (m_SyncParts9Local != m_SyncParts9)
		{
			m_SyncParts9Local = m_SyncParts9;
			changed = true;
		}
		if (m_SyncParts10Local != m_SyncParts10)
		{
			m_SyncParts10Local = m_SyncParts10;
			changed = true;
		}
		
		//do local client stuff here
		if (changed)
		{
			array<ConstructionPart> changedParts = new array<ConstructionPart>();
			if (SetAllPartsFromSyncData(changedParts))
			{
				QueueFullUpdateParts(changedParts);
			}
			changedParts = null;
		}
	}
	
	override void HandleSyncDataBuild(ConstructionPart part)
	{
		int partId = part.GetId();
		if (partId <= 0)
			return;
		
		//in case of variable part sync data sizes, prepare some map with info to be used in the switch. Sadly the syncVariables themselves can't be handled as such.
		int adjustedID = Math.Clamp((partId - 1), 0, int.MAX) * part.GetSyncDataSize();
		int syncIntIdx = adjustedID / BIT_INT_SIZE;
		int offset = adjustedID % BIT_INT_SIZE;
		
		switch (syncIntIdx)
		{
			case 0:
				part.ComposeSyncDataOnBuild(m_SyncParts1, offset);
			break;
			
			case 1:
				part.ComposeSyncDataOnBuild(m_SyncParts2, offset);
			break;
			
			case 2:
				part.ComposeSyncDataOnBuild(m_SyncParts3, offset);
			break;
			
			case 3:
				part.ComposeSyncDataOnBuild(m_SyncParts4, offset);
			break;
			
			case 4:
				part.ComposeSyncDataOnBuild(m_SyncParts5, offset);
			break;
			
			case 5:
				part.ComposeSyncDataOnBuild(m_SyncParts6, offset);
			break;
			
			case 6:
				part.ComposeSyncDataOnBuild(m_SyncParts7, offset);
			break;
			
			case 7:
				part.ComposeSyncDataOnBuild(m_SyncParts8, offset);
			break;
			
			case 8:
				part.ComposeSyncDataOnBuild(m_SyncParts9, offset);
			break;
			
			case 9:
				part.ComposeSyncDataOnBuild(m_SyncParts10, offset);
			break;
		}
	}
	
	override void HandleSyncDataDismantle(ConstructionPart part)
	{
		int partId = part.GetId();
		if (partId <= 0)
			return;
		
		int adjustedID = Math.Clamp((partId - 1), 0, int.MAX) * part.GetSyncDataSize();
		int syncIntIdx = adjustedID / BIT_INT_SIZE;
		int offset = adjustedID % BIT_INT_SIZE;
		
		switch (syncIntIdx)
		{
			case 0:
				part.ComposeSyncDataOnDismantle(m_SyncParts1, offset);
			break;
			
			case 1:
				part.ComposeSyncDataOnDismantle(m_SyncParts2, offset);
			break;
			
			case 2:
				part.ComposeSyncDataOnDismantle(m_SyncParts3, offset);
			break;
			
			case 3:
				part.ComposeSyncDataOnDismantle(m_SyncParts4, offset);
			break;
			
			case 4:
				part.ComposeSyncDataOnDismantle(m_SyncParts5, offset);
			break;
			
			case 5:
				part.ComposeSyncDataOnDismantle(m_SyncParts6, offset);
			break;
			
			case 6:
				part.ComposeSyncDataOnDismantle(m_SyncParts7, offset);
			break;
			
			case 7:
				part.ComposeSyncDataOnDismantle(m_SyncParts8, offset);
			break;
			
			case 8:
				part.ComposeSyncDataOnDismantle(m_SyncParts9, offset);
			break;
			
			case 9:
				part.ComposeSyncDataOnDismantle(m_SyncParts10, offset);
			break;
		}
	}
	
	override void HandleSyncDataDestroy(ConstructionPart part)
	{
		int partId = part.GetId();
		if (partId <= 0)
			return;
		
		int adjustedID = Math.Clamp((partId - 1), 0, int.MAX) * part.GetSyncDataSize();
		int syncIntIdx = adjustedID / BIT_INT_SIZE;
		int offset = adjustedID % BIT_INT_SIZE;
		
		switch (syncIntIdx)
		{
			case 0:
				part.ComposeSyncDataOnDestroy(m_SyncParts1, offset);
			break;
			
			case 1:
				part.ComposeSyncDataOnDestroy(m_SyncParts2, offset);
			break;
			
			case 2:
				part.ComposeSyncDataOnDestroy(m_SyncParts3, offset);
			break;
			
			case 3:
				part.ComposeSyncDataOnDestroy(m_SyncParts4, offset);
			break;
			
			case 4:
				part.ComposeSyncDataOnDestroy(m_SyncParts5, offset);
			break;
			
			case 5:
				part.ComposeSyncDataOnDestroy(m_SyncParts6, offset);
			break;
			
			case 6:
				part.ComposeSyncDataOnDestroy(m_SyncParts7, offset);
			break;
			
			case 7:
				part.ComposeSyncDataOnDestroy(m_SyncParts8, offset);
			break;
			
			case 8:
				part.ComposeSyncDataOnDestroy(m_SyncParts9, offset);
			break;
			
			case 9:
				part.ComposeSyncDataOnDestroy(m_SyncParts10, offset);
			break;
		}
	}
	
	//! Returns the translated value of 'GetSyncDataSize' size (currently 2 bits)
	int GetPartLocalSyncBitMaskValue(ConstructionPart part)
	{
		int partId = part.GetId();
		if (partId <= 0)
			return false;
		
		int adjustedID = Math.Clamp((partId - 1), 0, int.MAX) * part.GetSyncDataSize();
		int syncIntIdx = adjustedID / BIT_INT_SIZE;
		int offset = adjustedID % BIT_INT_SIZE;
		int tmp;
		
		switch (syncIntIdx)
		{
			case 0:
				tmp = m_SyncParts1;
			break;
			
			case 1:
				tmp = m_SyncParts2;
			break;
			
			case 2:
				tmp = m_SyncParts3;
			break;
			
			case 3:
				tmp = m_SyncParts4;
			break;
			
			case 4:
				tmp = m_SyncParts5;
			break;
			
			case 5:
				tmp = m_SyncParts6;
			break;
			
			case 6:
				tmp = m_SyncParts7;
			break;
			
			case 7:
				tmp = m_SyncParts8;
			break;
			
			case 8:
				tmp = m_SyncParts9;
			break;
			
			case 9:
				tmp = m_SyncParts10;
			break;
		}
		
		return part.CalculateLocalSyncBitMask(tmp, offset);
	}
	
	//! returns 'true' if any part state was changed
	bool SetAllPartsFromSyncData(out array<ConstructionPart> changedParts = null)
	{
		bool wasChanged = false;
		
		foreach (ConstructionPart part : m_ConstructionPartArray)
		{
			if (UpdatePartSyncStateLocal(part))
			{
				if (changedParts)
					changedParts.Insert(part);
				wasChanged = true;
			}
		}
		
		return wasChanged;
	}
	
	//! Sets local construction part state
	override bool UpdatePartSyncStateLocal(ConstructionPart part)
	{
		if (part.GetId() < 1)
		{
			if (!part.IsUtilityPart())
				ErrorEx("invalid or corrupted part ID for part " + part.GetPartName() + ". Part ID must be > 0");
			return false;
		}
		
		part.StoreLocalSyncBitMask(GetPartLocalSyncBitMaskValue(part)); //stores translated synced info on the local part instance
		return SetPartStateLocal(part);
	}
	
	protected bool SetPartStateLocal(ConstructionPart part)
	{
		bool changed = false;
		bool isSyncedAsBuilt = part.IsLocalSyncBuilt();
		bool isBuilt = part.IsBuilt();
		string name = part.GetPartName();
		if (isSyncedAsBuilt)
		{
			if (!isBuilt)
			{
				AddToConstructedParts(name);
				SetLockOnAttachedMaterials(name, true);
				changed = true;
			}
		}
		else
		{
			if (isBuilt)
			{
				RemoveFromConstructedParts(name);
				SetLockOnAttachedMaterials(name, false);
				changed = true;
			}
		}
		
		return changed;
	}
	
	override void AddToConstructedParts(string part_name)
	{
		AddSupportFulfillmentFromSourcePart(part_name);
		AddSupportRequirementFromSourcePart(part_name);
		
		super.AddToConstructedParts(part_name);
	}
	
	override void RemoveFromConstructedParts(string part_name)
	{
		RemoveSupportFulfillmentFromSourcePart(part_name);
		RemoveSupportRequirementFromSourcePart(part_name);
		
		super.RemoveFromConstructedParts(part_name);
	}
	
	//============================================
	// Events
	//============================================
	override void OnConstructionDoorOpenStart(DoorStartParams params)
	{
		super.OnConstructionDoorOpenStart(params);
		
		int doorIdx = (int)params.param1;
		BuildingBase building;
		AdditionalDoorInfo info;
		if (Class.CastTo(building, m_EntityParent) && Class.CastTo(info, building.GetDoorInfo(doorIdx)) && info.m_DoorConstructionPhysicsSource != string.Empty)
		{
			building.SetAnimationPhase(info.m_DoorConstructionPhysicsSource, 1.0);
		}
	}
	
	override void OnConstructionDoorCloseStart(DoorStartParams params)
	{
		super.OnConstructionDoorCloseStart(params);
		
		int doorIdx = (int)params.param1;
		BuildingBase building;
		AdditionalDoorInfo info;
		if (Class.CastTo(building, m_EntityParent) && Class.CastTo(info, building.GetDoorInfo(doorIdx)) && info.m_DoorConstructionPhysicsSource != string.Empty)
		{
			building.SetAnimationPhase(info.m_DoorConstructionPhysicsSource, 0.0);
		}
	}
	
	//============================================
	// Persistence
	//============================================
	override bool HandleStorageEvents()
	{
		return true;
	}
	
	override void HandleStoreSave(ParamsWriteContext ctx)
	{
		if (HandleStorageEvents())
		{
			super.HandleStoreSave(ctx);
			
			SerializeConstructionData(ctx);
		}
	}
	
	override void HandleStoreLoad(ParamsReadContext ctx, int version)
	{
		if (HandleStorageEvents())
		{
			super.HandleStoreLoad(ctx,version);
			
			if (!DeserializeConstructionData(ctx,version))
				ErrorEx("deserialization of construction data of " + m_EntityParent.GetType() + " FAILED, presumed corrupted data",ErrorExSeverity.INFO);
			
			//true if changed, we need to react to and sync the changes to clients
			if (SetAllPartsFromSyncData())
			{
				QueueFullUpdateParts(m_ConstructionPartArray);
			}
			m_EntityParent.SetSynchDirty();
		}
	}
	
	protected void SerializeConstructionData(ParamsWriteContext ctx)
	{
		ctx.Write(REBUILDING_STORAGE_VERSION);
		ctx.Write(m_SyncParts1);
		ctx.Write(m_SyncParts2);
		ctx.Write(m_SyncParts3);
		ctx.Write(m_SyncParts4);
		ctx.Write(m_SyncParts5);
		ctx.Write(m_SyncParts6);
		ctx.Write(m_SyncParts7);
		ctx.Write(m_SyncParts8);
		ctx.Write(m_SyncParts9);
		ctx.Write(m_SyncParts10);
	}
	
	protected bool DeserializeConstructionData(ParamsReadContext ctx, int version)
	{
		if (!ctx.Read(m_LastStorageVersion))
			return false;
		if (m_LastStorageVersion < REBUILDING_STORAGE_VERSION)
		{
			ErrorEx("Invalid rebuilding storage version | " + m_LastStorageVersion + "/" + REBUILDING_STORAGE_VERSION, ErrorExSeverity.INFO);
			return false;
		}
		
		if (!ctx.Read(m_SyncParts1))
			return false;
		if (!ctx.Read(m_SyncParts2))
			return false;
		if (!ctx.Read(m_SyncParts3))
			return false;
		if (!ctx.Read(m_SyncParts4))
			return false;
		if (!ctx.Read(m_SyncParts5))
			return false;
		if (!ctx.Read(m_SyncParts6))
			return false;
		if (!ctx.Read(m_SyncParts7))
			return false;
		if (!ctx.Read(m_SyncParts8))
			return false;
		if (!ctx.Read(m_SyncParts9))
			return false;
		if (!ctx.Read(m_SyncParts10))
			return false;
		
		return true;
	}
	
	//============================================
	// Inits
	//============================================
	override void Init()
	{
		super.Init();
		
		InitSupportMap();
	}
	
	protected void InitBaseStateDeferred()
	{
		QueueFullUpdateParts(m_ConstructionPartArray);
	}
	
	override void InitBaseState()
	{
		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).Call(InitBaseStateDeferred);
	}
	
	protected void InitSupportMap()
	{
		if (!m_SupportFulfillment)
			m_SupportFulfillment = new map<int, int>();
		
		if (!m_SupportRequirement)
			m_SupportRequirement = new map<int, int>();
	}
	
	override protected typename GetDefaultPartInstanceTypename()
	{
		return ConstructionPartRebuild;
	}
	
	//============================================
	// Parts
	//============================================
	protected ConstructionPartRebuild GetRebuildingPart(string part_name)
	{
		return ConstructionPartRebuild.Cast(GetConstructionPart(part_name));
	}
	
	//! Also checks for all listed variant categories (at least 1 from each is required)
	override protected bool HasRequiredPart(string part_name)
	{
		//simple requirements check
		if (!super.HasRequiredPart(part_name))
			return false;
		
		//now look into weighted variants
		TConstructionPartVariantArray tmp = GetRebuildingPart(part_name).GetRequiredPartsVariants();
		if (tmp)
		{
			bool anyVariantBuilt = false;
			foreach (array<string> variantGroup : tmp)
			{
				anyVariantBuilt = false;
				foreach (string partName : variantGroup)
				{
					anyVariantBuilt |= IsPartConstructed(partName);
				}
				
				if (!anyVariantBuilt)
					return false;
			}
		}
		
		return true;
	}
	
	////////////
	//supports//
	////////////
	
	override protected bool PartHasAnySupportFulfilled(string part_name)
	{
		if (!super.PartHasAnySupportFulfilled(part_name))
			return false;
		
		ConstructionPartRebuild rebPart = GetRebuildingPart(part_name);
		set<int> tmp = rebPart.GetConfiguredSupportCategories();
		if (tmp.Count() > 0) //can fail only if some REQUIREMENT support category is defined
		{
			bool validRequirementDefined;
			foreach (int category : tmp)
			{
				int requirement;
				if (rebPart.GetSupportRequirement(category, requirement))
				{
					validRequirementDefined = true;
					if (requirement <= m_SupportFulfillment.Get(category))
						return !rebPart.InvertSupportBehavior();
				}
			}
			
			if (validRequirementDefined)
				return rebPart.InvertSupportBehavior();
			
			return true;
		}
		
		return true;
	}
	
	override bool SupportRemovalCheck(string part_name)
	{
		ConstructionPartRebuild rebPart = GetRebuildingPart(part_name);
		set<int> tmp = rebPart.GetConfiguredSupportCategories();
		foreach (int category : tmp)
		{
			int provided;
			if (rebPart.GetSupportProvided(category, provided) && m_SupportRequirement.Get(category) > (m_SupportFulfillment.Get(category) - provided))
				return false;
		}
		
		return true;
	}
	
	protected void AddSupportFulfillmentFromSourcePart(string part_name)
	{
		ConstructionPartRebuild rebPart = GetRebuildingPart(part_name);
		set<int> tmp = rebPart.GetConfiguredSupportCategories();
		int provided;
		foreach (int category : tmp)
		{
			if (rebPart.GetSupportProvided(category, provided))
			{
				provided = Math.Clamp(m_SupportFulfillment.Get(category) + provided, 0, int.MAX);
				m_SupportFulfillment.Set(category, provided);
			}
		}
	}
	
	protected void RemoveSupportFulfillmentFromSourcePart(string part_name)
	{
		ConstructionPartRebuild rebPart = GetRebuildingPart(part_name);
		set<int> tmp = rebPart.GetConfiguredSupportCategories();
		int provided;
		foreach (int category : tmp)
		{
			if (rebPart.GetSupportProvided(category, provided))
			{
				provided = Math.Clamp(m_SupportFulfillment.Get(category) - provided, 0, int.MAX);
				m_SupportFulfillment.Set(category, provided);
			}
		}
	}
	
	protected void AddSupportRequirementFromSourcePart(string part_name)
	{
		ConstructionPartRebuild rebPart = GetRebuildingPart(part_name);
		//skipping utility part requirement propagation, fulfillment checked separately
		if (rebPart.IsUtilityPart())
			return;
		
		set<int> tmp = rebPart.GetConfiguredSupportCategories();
		int required;
		foreach (int category : tmp)
		{
			if (rebPart.GetSupportRequirement(category, required))
			{
				required = Math.Clamp(m_SupportRequirement.Get(category) + required, 0, int.MAX);
				m_SupportRequirement.Set(category, required);
			}
		}
	}
	
	protected void RemoveSupportRequirementFromSourcePart(string part_name)
	{
		ConstructionPartRebuild rebPart = GetRebuildingPart(part_name);
		//skipping utility part requirement propagation, fulfillment checked separately
		if (rebPart.IsUtilityPart())
			return;
		
		set<int> tmp = rebPart.GetConfiguredSupportCategories();
		int required;
		foreach (int category : tmp)
		{
			if (rebPart.GetSupportRequirement(category, required))
			{
				required = Math.Clamp(m_SupportRequirement.Get(category) - required, 0, int.MAX);
				m_SupportRequirement.Set(category, required);
			}
		}
	}
	
	override bool HasDependentPart(string part_name)
	{
		ConstructionPartRebuild rebPart;
		foreach (ConstructionPart part : m_BuiltConstructionPartArray)
		{
			if (Class.CastTo(rebPart, part) && !rebPart.IsUtilityPart())
			{
				if (rebPart.GetRequiredParts().Find(part_name) != -1)
					return true;
				
				TConstructionPartVariantArray tmpArr = rebPart.GetRequiredPartsVariants();
				if (tmpArr)
				{
					foreach (array<string> variantGroup : tmpArr)
					{
						foreach (string partName : variantGroup)
						{
							if (partName == part_name) //part_name needed
								return true;
						}
					}
				}
			}
		}
		
		return false;
	}
	
	override protected array<string> GetValidDepenentPartsArray(string part_name, array<string> recurs = null)
	{
		string name = "";
		array<string> dependentParts = new array<string>();
		
		ConstructionPartRebuild rebPart;
		foreach (ConstructionPart part : m_BuiltConstructionPartArray)
		{
			if (Class.CastTo(rebPart, part) && !rebPart.IsUtilityPart())
			{
				name = rebPart.GetPartName();
				array<string> reqPartsArray = rebPart.GetRequiredParts();
				if (reqPartsArray && reqPartsArray.Find(part_name) > -1)
				{
					if (!recurs || (recurs.Find(name) == -1))
					dependentParts.Insert(name);
				}
				
				TConstructionPartVariantArray tmpArr = rebPart.GetRequiredPartsVariants();
				if (tmpArr)
				{
					foreach (array<string> variantGroup : tmpArr)
					{
						foreach (string partName : variantGroup)
						{
							if (partName == part_name) //part_name needed
								dependentParts.Insert(name);
						}
					}
				}
				
				if (!PartHasAnySupportFulfilled(name) && dependentParts.Find(name) == -1)
				{
					dependentParts.Insert(name);
				}
			}
		}
		
		return dependentParts;
	}
	
	//! Houses are usually not mobile to become a dynamic problem; overlap with
	//! houses placed touching the rebuilt one is accepted by design
	override bool IgnoreHouseCollisions()
	{
		return true;
	}
	
	//============================================
	//UI-related getters
	//============================================
	//! client-side distance checks (valid memory point and configuration required)
	override bool CanDisplayAttachmentCategoryConstruction(string categoryName)
	{
		if (!super.CanDisplayAttachmentCategoryConstruction(categoryName))
			return false;
		
		array<ConstructionPartTypeData> GUIRequiredParts = m_TypeLevelHolder.GetGuiRequiredPartTypesMap().Get(categoryName);
		if (GUIRequiredParts.Count() > 0)
		{
			foreach (ConstructionPartTypeData partTypeData : GUIRequiredParts)
			{
				if (!GetConstructionPart(partTypeData.GetPartName()).IsBuilt())
					return false;
			}
		}
		
		ConstructionPartRebuild rebPart = GetRebuildingPart(categoryName);
		if (rebPart)
		{
			if (!rebPart.IsBuilt() || !HasRequiredPart(rebPart.GetPartName()))
				return false;
		}
		
		return true;
	}
}
