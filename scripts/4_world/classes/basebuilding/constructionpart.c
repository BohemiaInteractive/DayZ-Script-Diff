
//! Instanced construction part data classes for dynamic data, statics now stored on EntityType level in ConstructionPartTypeData
class ConstructionPart
{
	//TODO: start with 2 bits per part, and iterate into differentiated simple/complex part structure
	static const int BITWISE_SYNCINFO_SIZE_BASE = 2; //! no. of bits to store synced info, per part. System not ready for variable size parts yet.
	protected const int ANIMATION_EFFECT_DELAY_BUILD = 350; //ms
	
	protected ConstructionPartTypeData m_PartTypeData; //! Entity-type object storing non-instanced data
	protected int 	m_LocalSyncBitMask = 0;
	bool 	m_IsBuilt;			//defines part build state
	protected ref TConstructionPartInsiderBoxData m_InsiderBoxData; //Grouped modelspace positions of min/max insidebox points
	
	void ConstructionPart( string name, string part_name, string main_part_name, int id, bool is_built, bool is_base, bool is_gate, array<string> required_parts )
	{
	}
	
	//////////////
	//Public Inits
	//////////////
	
	//! intended to fill in additional INSTANCE info AFTER basic info is filled (which is tied to constructor parameters from ye olden times). Override as needed.
	void InitAdditionalPartInstanceInfo(PartInstanceInitData initData);
	
	//! Init stuff reliant on entity position or other stuff that can't be done in constructor
	void PartInstanceDeferredInit(EntityAI parent)
	{
		InitInsiderBoxData(parent);
	}
	
	/////////////////
	//Protected Inits
	/////////////////
	
	//! 'inside' box data to help determine whether the character is inside. Modelspace.
	protected void InitInsiderBoxData(EntityAI parent)
	{
		array<string> tmp = m_PartTypeData.GetInsiderBoxPairNames(); //if data exists, should be validated by this point
		if (tmp)
		{
			//now construct the vector array for the instance
			m_InsiderBoxData = new TConstructionPartInsiderBoxData();
			string nameMin, nameMax;
			int count = tmp.Count();
			for (int i; i < count / 2; ++i)
			{
				nameMin = tmp[i*2];
				nameMax = tmp[i*2+1];
				if (!parent.MemoryPointExists(nameMin) || !parent.MemoryPointExists(nameMax))
					continue;
				
				vector min = parent.GetMemoryPointPos(nameMin);
				vector max = parent.GetMemoryPointPos(nameMax);
				
				vector.SanitizeMinsMaxs(min, max);
				m_InsiderBoxData.Insert({min, max});
			}
		}
	}
	
	////////////////////////////////////////////////////////
	//Syncing - calculation only, actual sync on component//
	////////////////////////////////////////////////////////
	//! Not ready for overrides yet. All in good time.
	int GetSyncDataSize()
	{
		return BITWISE_SYNCINFO_SIZE_BASE;
	}
	
	void ComposeSyncDataOnBuild(inout int syncData, int offset)
	{
		//clear first
		ClearPartBitMask(syncData, offset);
		
		int mask = CalculateBitMaskOnBuild(offset);
		syncData |= mask;
	}
	
	void ComposeSyncDataOnDismantle(inout int syncData, int offset)
	{
		//clear first
		ClearPartBitMask(syncData, offset);
		
		int mask = CalculateBitMaskOnDismantle(offset);
		syncData |= mask;
	}
	
	void ComposeSyncDataOnDestroy(inout int syncData, int offset)
	{
		//clear first
		ClearPartBitMask(syncData, offset);
		
		int mask = CalculateBitMaskOnDestroy(offset);
		syncData |= mask;
	}
	
	protected void ClearPartBitMask(inout int syncData, int offset)
	{
		int mask = 0;
		
		int count = GetSyncDataSize();
		for (int i = 0; i < count; ++i)
		{
			mask |= 1 << (offset + i);
		}
		
		syncData &= ~mask;
	}
	
	//! default behavior for simple parts
	protected int CalculateBitMaskOnBuild(int offset)
	{
		int mask = 0;
		mask |= 1 << offset;
		
		return mask;
	}
	
	//! default behavior for simple parts
	protected int CalculateBitMaskOnDismantle(int offset)
	{
		return 0;
	}
	
	//! default behavior for simple parts
	protected int CalculateBitMaskOnDestroy(int offset)
	{
		return 0;
	}
	
	/////////////////////////////////
	//Syncing - local part bitmasks//
	/////////////////////////////////
	//! Translates the bitmask value from the main syncVariable into simple int format of 'GetSyncDataSize' size (currently 2 bits)
	int CalculateLocalSyncBitMask(int syncData, int offset)
	{
		int res = 0;
		int count = GetSyncDataSize();
		for (int i = 0; i < count; ++i)
		{
			res |= (syncData >> (offset + i) & 1) << i;
		}
		
		return res;
	}
	
	void StoreLocalSyncBitMask(int bitmask)
	{
		m_LocalSyncBitMask = bitmask;
	}
	
	int GetLocalSyncBitMask()
	{
		return m_LocalSyncBitMask;
	}
	
	///////////////////////
	//Getters - instance//
	/////////////////////
	bool IsBuilt()
	{
		return m_IsBuilt;
	}
	
	//! 'true' if synced as built already
	bool IsLocalSyncBuilt()
	{
		return GetLocalSyncBitMask() != 0;
	}
	
	//! translated MODELSPACE positions of min/max memory points (should be single points in memory LOD)
	TConstructionPartInsiderBoxData GetInsiderBoxData()
	{
		return m_InsiderBoxData;
	}
	
	bool CanBeRepaired(int zoneHealthLevel, ItemBase tool = null, PlayerBase player = null)
	{
		//default health level check
		return zoneHealthLevel > GetMaxRepairLevel(tool, player);
	}
	
	//! health state from 'GetHealthLevel', not the actual health float value!
	protected int GetMaxRepairLevel(ItemBase tool = null, PlayerBase player = null)
	{
		return GameConstants.STATE_WORN;
	}
	
	//////////////////
	//Getters - type//
	//////////////////
	
	string GetName()
	{
		return m_PartTypeData.GetNameLocalized();
	}
	
	string GetPartName()
	{
		return m_PartTypeData.GetPartName();
	}

	string GetMainPartName()
	{
		return m_PartTypeData.GetMainPartName();
	}
	
	int GetId()
	{
		return m_PartTypeData.GetId();
	}
	
	bool DamageToolOnBuild()
	{
		return m_PartTypeData.DamageToolOnBuild();
	}
	
	bool DamageHandsOnBuild()
	{
		return m_PartTypeData.DamageHandsOnBuild();
	}
	
	bool DamageToolOnDeconstruct()
	{
		return m_PartTypeData.DamageToolOnDeconstruct();;
	}
	
	bool DamageHandsOnDeconstruct()
	{
		return m_PartTypeData.DamageHandsOnDeconstruct();
	}
	
	bool IsUtilityPart()
	{
		return m_PartTypeData.IsUtilityPart();
	}
	
	bool CanPartDecay()
	{
		return m_PartTypeData.CanPartDecayConfig();
	}
	
	float GetPartDecayRate()
	{
		return m_PartTypeData.GetPartDecayRateConfig();
	}
	
	float GetPartDecayThreshold()
	{
		return m_PartTypeData.GetPartDecayThresholdConfig();
	}
	
	int GetHandBuildFBAnimOverride()
	{
		return m_PartTypeData.GetHandBuildFBAnimOverride();
	}
	
	bool CanSimulateItemFalling()
	{
		return m_PartTypeData.CanSimulateItemFalling();
	}
	
	bool HasEssentialAttachmentSlots()
	{
		return m_PartTypeData.HasEssentialAttachmentSlots();
	}
	
	bool HasRelatedGUICategories()
	{
		return m_PartTypeData.HasRelatedGUICategories();
	}
	
	bool IsBase()
	{
		return m_PartTypeData.IsBase();
	}
	
	bool IsGate()
	{
		return m_PartTypeData.IsGate();
	}
	
	array<string> GetRequiredParts()
	{
		return m_PartTypeData.GetRequiredPartTypes();
	}
	
	array<string> GetConflictedParts()
	{
		return m_PartTypeData.GetConflictedPartTypes();
	}
	
	//! strings of min/max selections (should be single points in memory LOD)
	array<string> GetCollisionData()
	{
		return m_PartTypeData.GetCollisionData();
	}
	
	//! strings of min/max selections (should be single points in memory LOD)
	array<string> GetItemFallingData()
	{
		return m_PartTypeData.GetItemFallingData();
	}
	
	array<string> GetEssentialAttachmentSlots()
	{
		return m_PartTypeData.GetEssentialAttachmentSlots();
	}
	
	array<string> GetRelatedGUICategoryNames()
	{
		return m_PartTypeData.GetRelatedGUICategoryNames();
	}
	
	string GetBuildTextOverride()
	{
		return m_PartTypeData.GetBuildTextOverride();
	}
	
	string GetDismantleTextOverride()
	{
		return m_PartTypeData.GetDismantleTextOverride();
	}
	
	//! rough material type, used for effect playback
	int GetMaterialType()
	{
		return m_PartTypeData.GetMaterialType();
	}
	
	int GetBuildActionTypeMask()
	{
		return m_PartTypeData.GetBuildActionTypeMask();
	}
	
	int GetBuildIndicationTypeMask()
	{
		return m_PartTypeData.GetBuildIndicationTypeMask();
	}
	
	int GetDismantleActionTypeMask()
	{
		return m_PartTypeData.GetDismantleActionTypeMask();
	}
	
	array<ref ConstructionMaterialData> GetMaterialsData()
	{
		return m_PartTypeData.GetMaterialsData();
	}
	
	bool IsReversible()
	{
		return false;
	}
	
	bool IsReversedLocal()
	{
		return false;
	}
	
	TConstructionPartVariantArray GetRequiredPartsVariants()
	{
		return m_PartTypeData.GetRequiredPartsTypeVariantArray();
	}
	
	ConstructionPartStaticsSupportData GetPartStaticsSupportData()
	{
		return m_PartTypeData.GetPartTypeStaticsSupportData();
	}
	
	int GetSupportProvided(int category)
	{
		return m_PartTypeData.GetPartTypeSupportProvided(category);
	}
	
	int GetSupportRequirement(int category)
	{
		return m_PartTypeData.GetPartTypeSupportRequirement(category);
	}
	
	set<int> GetConfiguredSupportCategories()
	{
		return m_PartTypeData.GetPartTypeConfiguredSupportCategories();
	}
	
	///////////
	//setters//
	///////////
	void SetBuiltState( bool is_built )
	{
		if (LogManager.IsBaseBuildingLogEnable()) bsbDebugPrint("[bsb] SetBuildState=" + is_built + " part=" + GetPartName());
		m_IsBuilt = is_built;
	}
	
	void SetPartTypeData(ConstructionPartTypeData ptd)
	{
		if (!m_PartTypeData)
			m_PartTypeData = ptd;
		else
			ErrorEx("cannot override m_PartTypeData during runtime!");
	}
	
	void PlayConstructionEffects(EntityAI parent);
	//! Plays sound of current construction state on parent; BaseBuildingBase uses its own class events for this..
	protected void PlayConstructionSound(EntityAI parent, vector effectPos);
	//! Plays particle of current construction state on parent; BaseBuildingBase uses its own class events for this..
	protected void PlayConstructionParticle(EntityAI parent, vector effectPos);
	
	//////////
	//events//
	//////////
	void HandleShowConstructionPart(EntityAI parent)
	{
		TryShowAnim(GetPartName(), parent);
	}
	
	void HandleHideConstructionPart(EntityAI parent)
	{
		TryHideAnim(GetPartName(), parent);
	}
	
	void HandleAddPhysics(EntityAI parent)
	{
		parent.AddProxyPhysics(GetPartName()); //keeping the proxy physics approach for the time being
	}
	
	void HandleRemovePhysics(EntityAI parent)
	{
		parent.RemoveProxyPhysics(GetPartName()); //keeping the proxy physics approach for the time being
	}
	
	void OnBeforeBuildServer(EntityAI parent, Man builder);
	void OnBeforeDismantleServer(EntityAI parent, Man dismantler);
	void OnBeforeDestroyServer(EntityAI parent, Man destroyer);
	
	void OnBuiltServer(EntityAI parent, Man builder);
	void OnDismantledServer(EntityAI parent, Man dismantler)
	{
		CloseDoorOnPartUnbuild(parent);
	}
	
	void OnDestroyedServer(EntityAI parent, Man destroyer)
	{
		CloseDoorOnPartUnbuild(parent);
	}
	
	protected void CloseDoorOnPartUnbuild(EntityAI parent)
	{
		BuildingBase buildingBase = BuildingBase.Cast(parent);
		if (buildingBase)
		{
			map<int, ref AdditionalDoorInfo> infoMap = buildingBase.GetDoorInfoHolder().GetDoorInfoMap();
			foreach (int idx, ref AdditionalDoorInfo info : infoMap)
			{
				if (info.m_DoorConstructionPartName == GetPartName() && !buildingBase.IsDoorClosed(idx))
					buildingBase.CloseDoor(idx);
			}
		}
	}
	
	//! Override-friendly
	protected void TryShowAnim(string animSourceName, notnull EntityAI parent)
	{
		PerformShowAnim(animSourceName, parent);
	}
	
	//! Override-friendly
	protected void TryHideAnim(string animSourceName, notnull EntityAI parent)
	{
		PerformHideAnim(animSourceName, parent);
	}
	
	//! Sealed on purpose
	sealed protected void PerformShowAnim(string animSourceName, notnull EntityAI parent)
	{
		parent.SetAnimationPhase(animSourceName, 0);
	}
	
	//! Sealed on purpose
	sealed protected void PerformHideAnim(string animSourceName, notnull EntityAI parent)
	{
		parent.SetAnimationPhase(animSourceName, 1);
	}
	
	void TryHandleUtilityParts(EntityAI parent);
	
	/////////////
	//obsoletes//
	/////////////
	[Obsolete("1.30: data stored on ConstructionPartTypeData")]
	string 	m_Name;
	[Obsolete("1.30: data stored on ConstructionPartTypeData")]
	string 	m_PartName;
	[Obsolete("1.30: data stored on ConstructionPartTypeData")]
	string 	m_MainPartName;
	[Obsolete("1.30: data stored on ConstructionPartTypeData")]
	int		m_Id;
	[Obsolete("1.30: data stored on ConstructionPartTypeData")]
	bool 	m_IsBase;
	[Obsolete("1.30: data stored on ConstructionPartTypeData")]
	bool 	m_IsGate;
	ref array<string> m_RequiredParts; //!obsolete, stored on 'ConstructionPartTypeData'

	[Obsolete("1.30: Use 'SetBuiltState' directly")]
	void SetRequestBuiltState( bool req_built )
	{
		if (LogManager.IsBaseBuildingLogEnable())
			bsbDebugPrint("[bsb] SetRequestBuiltState=" + req_built + " part=" + GetPartName());
		
		if (g_Game.IsMultiplayer())
			SetBuiltState(req_built);
	}
}

class ConstructionPartRebuild : ConstructionPart
{
	//////////////
	//Public Inits
	//////////////
	
	override void InitAdditionalPartInstanceInfo(PartInstanceInitData initData)
	{
		super.InitAdditionalPartInstanceInfo(initData);
		
		InitCustomPartInstanceProperties(initData);
	}
	
	/////////////////
	//Protected Inits
	/////////////////
	
	//! Generic init method for custom part instance properties
	protected void InitCustomPartInstanceProperties(PartInstanceInitData initData);
	
	//////////
	//events//
	//////////
	override void HandleShowConstructionPart(EntityAI parent)
	{
		array<string> animSourceNameOverrides = m_PartTypeData.GetPartTypeAnimSourceNameOverrides();
		array<int> animSourceBehaviorInvetrtedOverrides = m_PartTypeData.GetPartTypeAnimSourceBehaviorInverts();
		int count = animSourceNameOverrides.Count();
		if (count > 0)
		{
			string animSource;
			for (int i = 0; i < count; ++i)
			{
				animSource = animSourceNameOverrides[i];
				if (!animSourceBehaviorInvetrtedOverrides[i])
					TryShowAnim(animSource, parent);
				else
					TryHideAnim(animSource, parent);
			}
		}
		else
		{
			TryShowAnim(GetPartName(), parent);
		}
	}
	
	override void HandleHideConstructionPart(EntityAI parent)
	{
		array<string> animSourceNameOverrides = m_PartTypeData.GetPartTypeAnimSourceNameOverrides();
		array<int> animSourceBehaviorInvetrtedOverrides = m_PartTypeData.GetPartTypeAnimSourceBehaviorInverts();
		int count = animSourceNameOverrides.Count();
		if (count > 0)
		{
			string animSource;
			for (int i = 0; i < count; ++i)
			{
				animSource = animSourceNameOverrides[i];
				if (!animSourceBehaviorInvetrtedOverrides[i])
					TryHideAnim(animSource, parent);
				else
					TryShowAnim(animSource, parent);
			}
		}
		else
		{
			TryHideAnim(GetPartName(), parent);
		}
	}	
	
	override void HandleAddPhysics(EntityAI parent)
	{
		array<string> phxGeomNames = m_PartTypeData.GetPartTypePhxGeomNames();
		array<int> phxGeomBehaviorInvets = m_PartTypeData.GetPartTypePhxGeomBehaviorInverts();
		int count = phxGeomNames.Count();
		if (count > 0)
		{
			string physName;
			for (int i = 0; i < count; ++i)
			{
				physName = phxGeomNames[i];
				if (!phxGeomBehaviorInvets[i])
				{
					ProcessPhysicsAdd(physName, parent);
				}
				else
				{
					ProcessPhysicsRemove(physName, parent);
				}
			}
		}
		
		//! added here since it's essentially a supplementary physics operation
		HandleAddProxyRoadway(parent);
	}
	
	override void HandleRemovePhysics(EntityAI parent)
	{
		array<string> phxGeomNames = m_PartTypeData.GetPartTypePhxGeomNames();
		array<int> phxGeomBehaviorInvets = m_PartTypeData.GetPartTypePhxGeomBehaviorInverts();
		int count = phxGeomNames.Count();
		if (count > 0)
		{
			string physName;
			for (int i = 0; i < count; ++i)
			{
				physName = phxGeomNames[i];
				if (phxGeomBehaviorInvets[i])
				{
					ProcessPhysicsAdd(physName, parent);
				}
				else
				{
					ProcessPhysicsRemove(physName, parent);
				}
			}
		}
		
		//! added here since it's essentially a supplementary physics operation
		HandleRemoveProxyRoadway(parent);
	}
	
	protected void HandleAddProxyRoadway(EntityAI parent)
	{
		array<string> proxyRoadwayNames = m_PartTypeData.GetPartTypeProxyRoadwayNames();
		array<int> proxyRoadwayBehaviorInvets = m_PartTypeData.GetPartTypeProxyRoadwayBehaviorInverts();
		int count = proxyRoadwayNames.Count();
		if (count > 0)
		{
			string proxyRoadwayName;
			for (int i = 0; i < count; ++i)
			{
				proxyRoadwayName = proxyRoadwayNames[i];
				if (!proxyRoadwayBehaviorInvets[i])
				{
					parent.AddProxyPhysics(proxyRoadwayName);
				}
				else
				{
					parent.RemoveProxyPhysics(proxyRoadwayName);
				}
			}
		}
	}
	
	protected void HandleRemoveProxyRoadway(EntityAI parent)
	{
		array<string> proxyRoadwayNames = m_PartTypeData.GetPartTypeProxyRoadwayNames();
		array<int> proxyRoadwayBehaviorInvets = m_PartTypeData.GetPartTypeProxyRoadwayBehaviorInverts();
		int count = proxyRoadwayNames.Count();
		if (count > 0)
		{
			string proxyRoadwayName;
			for (int i = 0; i < count; ++i)
			{
				proxyRoadwayName = proxyRoadwayNames[i];
				if (proxyRoadwayBehaviorInvets[i])
				{
					parent.AddProxyPhysics(proxyRoadwayName);
				}
				else
				{
					parent.RemoveProxyPhysics(proxyRoadwayName);
				}
			}
		}
	}
	
	/**
	\brief Groups all actions needed to 'add physics'
	\note: 'name' is either animSource name, or boneName, depening on proxy physics usage. Match them when in doubt.
	*/
	protected void ProcessPhysicsAdd(string name, notnull EntityAI parent)
	{
		TryShowAnim(name, parent);
		AddPartGeom(parent, name); //and flags the geoms of the same name
	}
	
	/**
	\brief Groups all actions needed to 'remove physics'
	\note: 'name' is either animSource name, or boneName, depening on proxy physics usage. Match them when in doubt.
	*/
	protected void ProcessPhysicsRemove(string name, notnull EntityAI parent)
	{
		TryHideAnim(name, parent);
		RemovePartGeom(parent, name); //and flags the geoms of the same name
	}
	
	//! Non-proxy physics handling, ADDING
	protected void AddPartGeom(EntityAI parent, string physName)
	{
		Physics phx = parent.GetPhysics();
		if (!phx)
			return;
		/*
		int idx = phx.GetGeom(physName);
		int mask = phx.GetGeomInteractionLayer(idx);
		mask &= ~PhxInteractionLayers.NOCOLLISION;
		mask |= PhxInteractionLayers.BUILDING;
		phx.SetGeomInteractionLayer(idx, mask);
		*/
		int count = phx.GetNumGeoms();
		string name = physName;
		name.ToLower();
		for (int idx = 0; idx < count; ++idx)
		{
			if (phx.GetGeomName(idx) == name)
			{
				int mask = phx.GetGeomInteractionLayer(idx);
				mask &= ~PhxInteractionLayers.NOCOLLISION;
				mask |= PhxInteractionLayers.BUILDING;
				phx.SetGeomInteractionLayer(idx, mask);
			}
		}
	}
	
	//! Non-proxy physics handling, REMOVAL
	protected void RemovePartGeom(EntityAI parent, string physName)
	{
		Physics phx = parent.GetPhysics();
		if (!phx)
			return;
		/*
		int idx = phx.GetGeom(physName);
		int mask = phx.GetGeomInteractionLayer(idx);
		mask &= ~PhxInteractionLayers.BUILDING;
		mask |= PhxInteractionLayers.NOCOLLISION;
		phx.SetGeomInteractionLayer(idx, mask);
		*/
		int count = phx.GetNumGeoms();
		string name = physName;
		name.ToLower();
		for (int idx = 0; idx < count; ++idx)
		{
			if (phx.GetGeomName(idx) == name)
			{
				int mask = phx.GetGeomInteractionLayer(idx);
				mask &= ~PhxInteractionLayers.BUILDING;
				mask |= PhxInteractionLayers.NOCOLLISION;
				phx.SetGeomInteractionLayer(idx, mask);
			}
		}
	}
	
	override protected void TryShowAnim(string animSourceName, notnull EntityAI parent)
	{
		ConstructionPartEffectsData effData = m_PartTypeData.GetPartTypeEffectsData();
		if (effData.IsValid() && effData.GetSoundsetBuild() != "") //delayed by a pre-defined time, waiting for a particular place in the sound
			g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(PerformShowAnim, ANIMATION_EFFECT_DELAY_BUILD, false, animSourceName, parent);
		else
			PerformShowAnim(animSourceName, parent);
	}
	
	///////////
	//effects//
	///////////
	
	override void PlayConstructionEffects(EntityAI parent)
	{
		ConstructionPartEffectsData effData = m_PartTypeData.GetPartTypeEffectsData();
		if (!effData.IsValid())
			return;
		
		ErrorExSeverity severity = ErrorExSeverity.INFO;
		#ifdef DIAG_DEVELOPER
		if (IsCLIParam("loudConstructionEffectWarnings"))
			severity = ErrorExSeverity.ERROR;
		#endif
		
		array<string> memPointNames;
		
		//sounds
		memPointNames = effData.GetSoundEffectMemoryPointNames();
		foreach (string soundPointName : memPointNames)
		{
			if (soundPointName == "" || !parent.MemoryPointExists(soundPointName))
			{
				ErrorEx("'" + soundPointName + "' memory point not found in the model: " + parent.GetShapeName() + ", skipping construction effects!", severity);
				return;
			}
			
			PlayConstructionSound(parent, g_Game.ObjectModelToWorld(parent, parent.GetMemoryPointPos(soundPointName)));
		}
		
		//particles
		//To be reconnected after 1.30
		/*memPointNames = effData.GetParticleEffectMemoryPointNames();
		foreach (string particlePointName : memPointNames)
		{
			if (particlePointName == "" || !parent.MemoryPointExists(particlePointName))
			{
				ErrorEx("'" + particlePointName + "' memory point not found in the model: " + parent.GetShapeName() + ", skipping construction effects!", severity);
				return;
			}
			
			PlayConstructionParticle(parent, g_Game.ObjectModelToWorld(parent, parent.GetMemoryPointPos(particlePointName)));
		}*/
	}
	
	override protected void PlayConstructionSound(EntityAI parent, vector effectPos)
	{	
		string soundsetName = "";	
		ConstructionPartEffectsData effData = m_PartTypeData.GetPartTypeEffectsData();
		if (!m_IsBuilt)
		{
			//HP check first
			string damageZone;
			DamageSystem.GetDamageZoneFromComponentName(parent, GetPartName(), damageZone);
			
			bool isDestruction;
			if (g_Game.IsMultiplayer())
				isDestruction = parent.GetHealthLevel(damageZone) == GameConstants.STATE_RUINED;
			else //SP solution below
				isDestruction = parent.GetHealth(damageZone, "Health") <= 0;
			
			if (!isDestruction)
			{
				//dismantle
				soundsetName = effData.GetSoundsetDismantle();
			}
			else
			{
				//destroy
				soundsetName = effData.GetSoundsetDestroy();
			}
		}
		else
		{
			//build
			soundsetName = effData.GetSoundsetBuild();
		}
		
		if (soundsetName != "")
			SEffectManager.PlaySoundEnviroment(soundsetName, effectPos);
	}
	
	override protected void PlayConstructionParticle(EntityAI parent, vector effectPos)
	{
		int particleID = ParticleList.INVALID;
		ConstructionPartEffectsData effData = m_PartTypeData.GetPartTypeEffectsData();
		if (!m_IsBuilt)
		{
			string damageZone;
			DamageSystem.GetDamageZoneFromComponentName(parent, GetPartName(), damageZone);
			if (g_Game.IsMultiplayer())
			{
				if (parent.GetHealthLevel(damageZone) == GameConstants.STATE_RUINED)
					particleID = effData.GetParticleDestroyID();
			}
			else //SP solution below
			{
				if (parent.GetHealth(damageZone, "Health") <= 0)
					particleID = effData.GetParticleDestroyID();
			}
			
		}
		
		if (particleID != ParticleList.INVALID)
			ParticleManager.GetInstance().PlayInWorld(particleID, effectPos);
	}
	
	/////////
	//utils//
	/////////
	//! Currently only local part state + animations, not a proper synced build!
	override void TryHandleUtilityParts(EntityAI parent)
	{
		super.TryHandleUtilityParts(parent);
		
		ConstructionBase con = ConstructionBase.Cast(parent.GetConstructionBasic());
		array<string> list = m_PartTypeData.GetPartTypeUtilityAutobuildPartsList();
		foreach (string partName : list)
		{
			ConstructionPart part = con.GetConstructionPart(partName);
			if (con.CanBuildUtilityPart(partName))
			{
				con.AddToConstructedParts(partName);
				ConstructionManagerStatic.AddPartToProcess(con, part);
			}
			else if (part.IsBuilt())
			{
				con.RemoveFromConstructedParts(partName);
				ConstructionManagerStatic.AddPartToProcess(con, part);
			}
		}
	}
}

//! Inverted show/hide functionality
class ConstructionPartRebuildInverted: ConstructionPartRebuild
{
	override void HandleShowConstructionPart(EntityAI parent)
	{
		super.HandleHideConstructionPart(parent);
	}
	
	override void HandleHideConstructionPart(EntityAI parent)
	{
		super.HandleShowConstructionPart(parent);
	}
	
	override void HandleAddPhysics(EntityAI parent)
	{
		super.HandleRemovePhysics(parent);
	}
	
	override void HandleRemovePhysics(EntityAI parent)
	{
		super.HandleAddPhysics(parent);
	}
}

//! special handling of received materials
class ConstructionPartRebuildRubble: ConstructionPartRebuild
{
	//! overriden to give materials
	override void OnBuiltServer(EntityAI parent, Man builder)
	{
		super.OnBuiltServer(parent,builder);
		
		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).Call(SpawnRubbleMaterials, parent, builder);
		
		if (RubbleRemovalCheck(parent))
			OnRubbleRemoved(parent);
	}
	
	protected void SpawnRubbleMaterials(EntityAI parent, Man builder)
	{
		PlayerBase player;
		ConstructionPartRebuildRubbleTypeData typeData;
		if (Class.CastTo(player, builder) && Class.CastTo(typeData, m_PartTypeData))
		{
			array<string> itemTypes = typeData.GetRubbleMaterialTypes();
			array<float> itemBaseQuantities = typeData.GetRubbleMaterialBaseQuantities();
			if (!itemTypes || !itemBaseQuantities)
				return;
			
			float quantityToolCoef = 1.0;
			ItemBase tool = player.GetItemInHands();
			if (tool)
			{
				//some modification of 'quantityToolCoef' here?
			}
			
			//position considerations
			vector posHead = MiscGameplayFunctions.GetPlayerHeadPosition(player);
			vector destination = player.GetPosition();
			string partName = m_PartTypeData.GetPartName();
			string mainPartName = m_PartTypeData.GetMainPartName();
			if (parent.MemoryPointExists("" + partName + "_materials"))
			{
				destination = parent.GetMemoryPointPos("" + partName + "_materials");
				destination = g_Game.ObjectModelToWorld(parent, destination);
			}
			else if (parent.MemoryPointExists("" + mainPartName + "_materials"))
			{
				destination = parent.GetMemoryPointPos("" + mainPartName + "_materials");
				destination = g_Game.ObjectModelToWorld(parent, destination);
			}
			
			//spawning
			int count = itemTypes.Count();
			for (int i = 0; i < count; ++i)
			{
				string itemType = itemTypes[i];
				float quantity = itemBaseQuantities[i];
				quantity *= quantityToolCoef;
				quantity = Math.Round(quantity); //just to make sure
				if (quantity <= 0)
				{
					continue;
				}
				
				MiscGameplayFunctions.CreateItemBasePilesDispersed(itemType, posHead, destination, UAItemsSpreadRadius.NARROW,quantity, MiscGameplayFunctions.GetTypeMaxGlobalHealth(itemType), player);
			}
		}
	}
	
	override protected void TryHideAnim(string animSourceName, notnull EntityAI parent)
	{
		ConstructionPartEffectsData effData = m_PartTypeData.GetPartTypeEffectsData();
		if (effData.IsValid() && effData.GetSoundsetBuild() != "") //delayed by a pre-defined time, waiting for a particular place in the sound
			g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(PerformHideAnim, ANIMATION_EFFECT_DELAY_BUILD, false, animSourceName, parent);
		else
			PerformHideAnim(animSourceName, parent);
	}
	
	override void HandleHideConstructionPart(EntityAI parent)
	{
		super.HandleHideConstructionPart(parent);
		
		OnRubbleCreated(parent);
	}
	
	//! Returns 'true' if all rubble piles are removed
	protected bool RubbleRemovalCheck(EntityAI parent)
	{
		Rebuilding reb;
		ConstructionPartRebuildRubbleTypeData typeData;
		if (Class.CastTo(reb, parent.GetConstructionBasic()) && Class.CastTo(typeData, m_PartTypeData))
		{
			array<string> namesMap = typeData.GetRelatedPartNames();
			foreach (string partName : namesMap)
			{
				ConstructionPart part = reb.GetConstructionPart(partName);
				if (!part.IsBuilt())
					return false;
			}
			return true;
		}
		return false;
	}
	
	//! animate defined animsources
	protected void OnRubbleRemoved(EntityAI parent)
	{
		ConstructionPartRebuildRubbleTypeData typeData = ConstructionPartRebuildRubbleTypeData.Cast(m_PartTypeData);
		map<string, bool> ASBMap = typeData.CustomAnimationSourceBehaviorMap();
		foreach (string animSource, bool hide : ASBMap)
		{
			parent.SetAnimationPhase(animSource, hide);
		}
	}
	
	protected void OnRubbleCreated(EntityAI parent)
	{
		ConstructionPartRebuildRubbleTypeData typeData = ConstructionPartRebuildRubbleTypeData.Cast(m_PartTypeData);
		map<string, bool> ASBMap = typeData.CustomAnimationSourceBehaviorMap();
		foreach (string animSource, bool hide : ASBMap)
		{
			parent.SetAnimationPhase(animSource, !hide);
		}
	}
}

class ConstructionPartRebuildReversibleBase: ConstructionPartRebuild
{
	protected bool m_TempReversed; //only used for briefly storing the info for later sync!
	
	override void OnBeforeBuildServer(EntityAI parent, Man builder)
	{
		super.OnBeforeBuildServer(parent,builder);
		
		//Check direction
		Rebuilding reb;
		if (Class.CastTo(reb, parent.GetConstructionBasic()))
		{
			m_TempReversed = reb.ConstructionDirectionCheck(PlayerBase.Cast(builder), this, EConstructionDirCheckType.REFERENCE_DIRECTION);
		}
	}
	
	override protected int CalculateBitMaskOnBuild(int offset)
	{
		int mask = 0;
		mask |= 1 << offset;
		mask |= m_TempReversed << (offset + 1);
		
		return mask;
	}
	
	override bool IsReversible()
	{
		return true;
	}
	
	override bool IsReversedLocal()
	{
		return (GetLocalSyncBitMask() & 2) != 0;
	}
	
	override void HandleShowConstructionPart(EntityAI parent)
	{
		super.HandleShowConstructionPart(parent);
		
		HandleSubvariantAnim(parent);
	}
	
	override void HandleHideConstructionPart(EntityAI parent)
	{
		super.HandleHideConstructionPart(parent);
		
		HandleSubvariantAnim(parent);
	}
	
	protected void HandleSubvariantAnim(EntityAI parent);
}

class ConstructionPartRebuildReversibleDoor: ConstructionPartRebuildReversibleBase
{
	override protected void HandleSubvariantAnim(EntityAI parent)
	{
		ConstructionPartRebuildReversibleDoorTypeData partTypeData = ConstructionPartRebuildReversibleDoorTypeData.Cast(m_PartTypeData);
		
		string defaultSourceName = partTypeData.GetDefaultSourceName();
		string reversedSourceName = partTypeData.GetReversedSourceName();
		if (defaultSourceName != "")
		{
			if (m_IsBuilt) //hide the unused subvariant
			{
				if (IsReversedLocal())
				{
					TryHideAnim(defaultSourceName, parent);
					TryShowAnim(reversedSourceName, parent);
				}
				else
				{
					TryShowAnim(defaultSourceName, parent);
					TryHideAnim(reversedSourceName, parent);
				}
			}
			else //hide both
			{
				TryHideAnim(defaultSourceName, parent);
				TryHideAnim(reversedSourceName, parent);
			}
		}
	}
}

class ConstructionPartRebuildReversibleBarricade: ConstructionPartRebuildReversibleBase
{
	//! barricade rotation
	override protected void HandleSubvariantAnim(EntityAI parent)
	{
		ConstructionPartRebuildReversibleBarricadeTypeData partTypeData = ConstructionPartRebuildReversibleBarricadeTypeData.Cast(m_PartTypeData);
		
		string barricadeName = partTypeData.GetSourceNameBarricade();
		if (barricadeName != "")
		{
			if (m_IsBuilt)
			{
				if (IsReversedLocal())
				{
					parent.SetAnimationPhase(barricadeName, 1);
				}
				else
				{
					parent.SetAnimationPhase(barricadeName, 0);
				}
			}
		}
	}
}

typedef array<ref array<string>> TConstructionPartVariantArray;
typedef array<ref array<vector>> TConstructionPartInsiderBoxData;
