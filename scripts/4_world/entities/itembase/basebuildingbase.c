//BASE BUILDING BASE
class BaseBuildingBase extends ItemBase
{
	const string 		ANIMATION_DEPLOYED			= "Deployed";
	
	float 				m_ConstructionKitHealth;			//stored health value for used construction kit

	ref Construction 	m_Construction;
	
	bool 				m_HasBase;
	//variables for synchronization of base building parts (2x31 is the current limit)
	int 				m_SyncParts01;								//synchronization for already built parts (31 parts)
	int 				m_SyncParts02;								//synchronization for already built parts (+31 parts)
	int 				m_SyncParts03;								//synchronization for already built parts (+31 parts)
	int 				m_InteractedPartId;							//construction part id that an action was performed on
	int					m_PerformedActionId;						//action id that was performed on a construction part
	
	//Sounds
	//build
	const string SOUND_BUILD_WOOD_LOG 			= "putDown_WoodLog_SoundSet";
	const string SOUND_BUILD_WOOD_PLANK			= "putDown_WoodPlank_SoundSet";
	const string SOUND_BUILD_WOOD_STAIRS		= "putDown_WoodStairs_SoundSet";
	const string SOUND_BUILD_METAL				= "putDown_MetalPlank_SoundSet";
	const string SOUND_BUILD_WIRE				= "putDown_BarbedWire_SoundSet";
	//dismantle
	const string SOUND_DISMANTLE_WOOD_LOG 		= "Crash_WoodPlank_SoundSet";
	const string SOUND_DISMANTLE_WOOD_PLANK		= "Crash_WoodPlank_SoundSet";
	const string SOUND_DISMANTLE_WOOD_STAIRS	= "Crash_WoodPlank_SoundSet";
	const string SOUND_DISMANTLE_METAL			= "Crash_MetalPlank_SoundSet";
	const string SOUND_DISMANTLE_WIRE			= "putDown_BarbedWire_SoundSet";
	
	protected EffectSound m_Sound;
	
	ref map<string, ref AreaDamageManager> m_DamageTriggers;
	ref array<string> m_HybridAttachments;
	ref array<string> m_Mountables;
	
	// Constructor
	void BaseBuildingBase() 
	{
		m_DamageTriggers = new map<string, ref AreaDamageManager>;
		
		//synchronized variables
		RegisterNetSyncVariableInt( "m_SyncParts01" );
		RegisterNetSyncVariableInt( "m_SyncParts02" );
		RegisterNetSyncVariableInt( "m_SyncParts03" );
		RegisterNetSyncVariableInt( "m_InteractedPartId" );
		RegisterNetSyncVariableInt( "m_PerformedActionId" );
		RegisterNetSyncVariableBool( "m_HasBase" );
		
		//Construction init
		ConstructionInit();
		
		if (ConfigIsExisting("hybridAttachments"))
		{
			m_HybridAttachments = new array<string>;
			ConfigGetTextArray("hybridAttachments", m_HybridAttachments);
		}
		if (ConfigIsExisting("mountables"))
		{
			m_Mountables = new array<string>;
			ConfigGetTextArray("mountables", m_Mountables);
		}
		
		ProcessInvulnerabilityCheck(GetInvulnerabilityTypeString());
	}
	
	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);

		foreach (AreaDamageManager areaDamage : m_DamageTriggers)
		{
			areaDamage.Destroy();
		}
		
	}
	
	override string GetInvulnerabilityTypeString()
	{
		return "disableBaseDamage";
	}
	
	override bool CanObstruct()
	{
		return true;
	}
	
	override int GetHideIconMask()
	{
		return EInventoryIconVisibility.HIDE_VICINITY;
	}
	
	override void InitItemSounds()
	{
		super.InitItemSounds();

		ItemSoundHandler handler = GetItemSoundHandler();
		SoundParameters params = new SoundParameters();
		params.m_Loop = true;

		if (GetFoldSoundset() != string.Empty)
			handler.AddSound(SoundConstants.ITEM_FOLD, GetFoldSoundset());
		if (GetLoopFoldSoundset() != string.Empty)
			handler.AddSound(SoundConstants.ITEM_FOLD_LOOP, GetLoopFoldSoundset(), params);
	}
	
	override string GetFoldSoundset()
	{
		return "putDown_FenceKit_SoundSet";
	}
	
	override string GetLoopFoldSoundset()
	{
		return "Shelter_Site_Build_Loop_SoundSet";
	}

	// --- SYNCHRONIZATION
	void SynchronizeBaseState()
	{
		if ( GetGame().IsServer() )
		{
			SetSynchDirty();
		}
	}

	override void OnVariablesSynchronized()
	{
		if (LogManager.IsBaseBuildingLogEnable()) bsbDebugPrint("[bsb] " + GetDebugName(this) + " OnVariablesSynchronized");
		super.OnVariablesSynchronized();

		GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( OnSynchronizedClient, 100, false );
	}
	
	protected void OnSynchronizedClient()
	{
		//update parts
		SetPartsFromSyncData();
		
		//update action on part
		SetActionFromSyncData();
		
		//update visuals (client)
		UpdateVisuals();
	}
	
	//parts synchronization
	void RegisterPartForSync( int part_id )
	{
		//part_id must starts from index = 1
		int offset;
		int mask;
		
		if ( part_id >= 1 && part_id <= 31 )		//<1,31> (31 parts)
		{
			offset = part_id - 1;
			mask = 1 << offset;
			
			m_SyncParts01 = m_SyncParts01 | mask;				
		}
		else if ( part_id >= 32 && part_id <= 62  )	//<32,62> (31 parts)
		{
			offset = ( part_id % 32 );
			mask = 1 << offset;
			
			m_SyncParts02 = m_SyncParts02 | mask;				
		}
		else if ( part_id >= 63 && part_id <= 93  )	//<63,93> (31 parts)
		{
			offset = ( part_id % 63 );
			mask = 1 << offset;
			
			m_SyncParts03 = m_SyncParts03 | mask;				
		}
	}
	
	void UnregisterPartForSync( int part_id )
	{
		//part_id must starts from index = 1
		int offset;
		int mask;
		
		if ( part_id >= 1 && part_id <= 31 )		//<1,31> (31 parts)
		{
			offset = part_id - 1;
			mask = 1 << offset;
			
			m_SyncParts01 = m_SyncParts01 & ~mask;			
		}
		else if ( part_id >= 32 && part_id <= 62  )	//<32,62> (31 parts)
		{
			offset = ( part_id % 32 );
			mask = 1 << offset;
			
			m_SyncParts02 = m_SyncParts02 & ~mask;			
		}
		else if ( part_id >= 63 && part_id <= 93  )	//<63,93> (31 parts)
		{
			offset = ( part_id % 63 );
			mask = 1 << offset;
			
			m_SyncParts03 = m_SyncParts03 & ~mask;			
		}
	}	
	
	bool IsPartBuildInSyncData( int part_id )
	{
		//part_id must starts from index = 1
		int offset;
		int mask;
		
		if ( part_id >= 1 && part_id <= 31 )		//<1,31> (31 parts)
		{
			offset = part_id - 1;
			mask = 1 << offset;
			
			if ( ( m_SyncParts01 & mask ) > 0 )
			{
				return true;
			}
		}
		else if ( part_id >= 32 && part_id <= 62  )	//<32,62> (31 parts)
		{
			offset = ( part_id % 32 );
			mask = 1 << offset;
			
			if ( ( m_SyncParts02 & mask ) > 0 )
			{
				return true;
			}
		}
		else if ( part_id >= 63 && part_id <= 93  )	//<63,93> (31 parts)
		{
			offset = ( part_id % 63 );
			mask = 1 << offset;
			
			if ( ( m_SyncParts03 & mask ) > 0 )
			{
				return true;
			}
		}				
	
		return false;
	}

	protected void RegisterActionForSync( int part_id, int action_id )
	{
		m_InteractedPartId 	= part_id;
		m_PerformedActionId = action_id;
	}
	
	protected void ResetActionSyncData()
	{
		//reset data
		m_InteractedPartId 	= -1;
		m_PerformedActionId = -1;
	}
	
	protected void SetActionFromSyncData()
	{
		if ( m_InteractedPartId > -1 && m_PerformedActionId > -1 )
		{
			ConstructionPart constrution_part = GetConstructionPartById( m_InteractedPartId );
			int build_action_id = m_PerformedActionId;
			
			switch( build_action_id )
			{
				case AT_BUILD_PART		: OnPartBuiltClient( constrution_part.GetPartName(), build_action_id ); break;
				case AT_DISMANTLE_PART	: OnPartDismantledClient( constrution_part.GetPartName(), build_action_id ); break;
				case AT_DESTROY_PART	: OnPartDestroyedClient( constrution_part.GetPartName(), build_action_id ); break;
			}
		}
	}
	//------
	
	void SetPartFromSyncData( ConstructionPart part )
	{
		string key = part.m_PartName;
		bool is_base = part.IsBase();
		bool is_part_built_sync = IsPartBuildInSyncData( part.GetId() );
		bsbDebugSpam("[bsb] " + GetDebugName(this) + " SetPartFromSyncData try to sync: built=" + is_part_built_sync + " key=" + key + " part=" + part.GetPartName() + " part_built=" + part.IsBuilt());
		if ( is_part_built_sync )
		{
			if ( !part.IsBuilt() )
			{
				if (LogManager.IsBaseBuildingLogEnable()) bsbDebugPrint("[bsb] " + GetDebugName(this) + " SetPartsFromSyncData +++ " + key);
				GetConstruction().AddToConstructedParts( key );
				GetConstruction().ShowConstructionPartPhysics(part.GetPartName());
				
				if (is_base)
				{
					if (LogManager.IsBaseBuildingLogEnable()) bsbDebugPrint("[bsb] " + GetDebugName(this) + ANIMATION_DEPLOYED + " RM");
					RemoveProxyPhysics( ANIMATION_DEPLOYED );
				}
			}
		}
		else
		{
			if ( part.IsBuilt() )
			{
				if (LogManager.IsBaseBuildingLogEnable()) bsbDebugPrint("[bsb] " + GetDebugName(this) + " SetPartsFromSyncData --- " + key);
				GetConstruction().RemoveFromConstructedParts( key );
				GetConstruction().HideConstructionPartPhysics(part.GetPartName());
			
				if (is_base)
				{
					if (LogManager.IsBaseBuildingLogEnable()) bsbDebugPrint("[bsb] " + GetDebugName(this) + ANIMATION_DEPLOYED + " ADD");
					AddProxyPhysics( ANIMATION_DEPLOYED );
				}
			}
		}

		//check slot lock for material attachments	
		GetConstruction().SetLockOnAttachedMaterials( part.GetPartName(), part.IsBuilt() );		//failsafe for corrupted sync/storage data			
	}
	
	//set construction parts based on synchronized data
	void SetPartsFromSyncData()
	{
		Construction construction = GetConstruction();
		map<string, ref ConstructionPart> construction_parts = construction.GetConstructionParts();
		
		for ( int i = 0; i < construction_parts.Count(); ++i )
		{
			string key = construction_parts.GetKey( i );
			ConstructionPart value = construction_parts.Get( key );
			SetPartFromSyncData(value);
		}
		
		//regenerate navmesh
		UpdateNavmesh();		
	}
	
	protected ConstructionPart GetConstructionPartById( int id )
	{
		Construction construction = GetConstruction();
		map<string, ref ConstructionPart> construction_parts = construction.GetConstructionParts();
		
		for ( int i = 0; i < construction_parts.Count(); ++i )
		{
			string key = construction_parts.GetKey( i );
			ConstructionPart value = construction_parts.Get( key );
		
			if ( value.GetId() == id )
			{
				return value;
			}
		}
		
		return NULL;
	}
	//
	
	//Base
	bool HasBase()
	{
		return m_HasBase;
	}
	
	void SetBaseState( bool has_base )
	{
		m_HasBase = has_base;
	}
	
	override bool IsDeployable()
	{
		return true;
	}
	
	bool IsOpened()
	{
		return false;
	}
	
	//--- CONSTRUCTION KIT
	ItemBase CreateConstructionKit()
	{
		ItemBase construction_kit = ItemBase.Cast( GetGame().CreateObjectEx( GetConstructionKitType(), GetKitSpawnPosition(), ECE_PLACE_ON_SURFACE ) );
		if ( m_ConstructionKitHealth > 0 )
		{
			construction_kit.SetHealth( m_ConstructionKitHealth );
		}
		
		return construction_kit;
	}
	
	void CreateConstructionKitInHands(notnull PlayerBase player)
	{
		ItemBase construction_kit = ItemBase.Cast(player.GetHumanInventory().CreateInHands(GetConstructionKitType()));
		if ( m_ConstructionKitHealth > 0 )
		{
			construction_kit.SetHealth( m_ConstructionKitHealth );
		}
	}
	
	protected vector GetKitSpawnPosition()
	{
		return GetPosition();
	}
	
	protected string GetConstructionKitType()
	{
		return "";
	}
	
	void DestroyConstructionKit( ItemBase construction_kit )
	{
		m_ConstructionKitHealth = construction_kit.GetHealth();
		GetGame().ObjectDelete( construction_kit );
	}
	
	//--- CONSTRUCTION
	void DestroyConstruction()
	{
		if (LogManager.IsBaseBuildingLogEnable()) bsbDebugPrint("[bsb] " + GetDebugName(this) + " DestroyConstruction");
		GetGame().ObjectDelete( this );
	}	
	
	// --- EVENTS
	override void OnStoreSave( ParamsWriteContext ctx )
	{   
		super.OnStoreSave( ctx );
		
		//sync parts 01
		ctx.Write( m_SyncParts01 );
		ctx.Write( m_SyncParts02 );
		ctx.Write( m_SyncParts03 );
		
		ctx.Write( m_HasBase );
	}
	
	override bool OnStoreLoad( ParamsReadContext ctx, int version )
	{
		if ( !super.OnStoreLoad( ctx, version ) )
			return false;
		
		//--- Base building data ---
		//Restore synced parts data
		if ( !ctx.Read( m_SyncParts01 ) )
		{
			m_SyncParts01 = 0;		//set default
			return false;
		}
		if ( !ctx.Read( m_SyncParts02 ) )
		{
			m_SyncParts02 = 0;		//set default
			return false;
		}
		if ( !ctx.Read( m_SyncParts03 ) )
		{
			m_SyncParts03 = 0;		//set default
			return false;
		}
		
		//has base
		if ( !ctx.Read( m_HasBase ) )
		{
			m_HasBase = false;
			return false;
		}
		//---

		return true;
	}
	
	override void AfterStoreLoad()
	{	
		super.AfterStoreLoad();		
		
		if (!m_FixDamageSystemInit)
		{
			SetPartsAfterStoreLoad();
		}
	}
	
	void SetPartsAfterStoreLoad()
	{
		//update server data
		SetPartsFromSyncData();
		
		//set base state
		ConstructionPart construction_part = GetConstruction().GetBaseConstructionPart();
		SetBaseState( construction_part.IsBuilt() ) ;
			
		//synchronize after load
		SynchronizeBaseState();
	}
	
	override void OnCreatePhysics()
	{
		super.OnCreatePhysics();
		ConstructionInit();
		SetPartsAfterStoreLoad();
	}
	
	override void EEHealthLevelChanged(int oldLevel, int newLevel, string zone)
	{
		if (m_FixDamageSystemInit)
			return;
		
		super.EEHealthLevelChanged(oldLevel,newLevel,zone);
		
		if (GetGame().IsMultiplayer() && !GetGame().IsServer())
			return;
		
		Construction construction = GetConstruction();
		string part_name = zone;
		part_name.ToLower();
		
		if ( newLevel == GameConstants.STATE_RUINED )
		{
			ConstructionPart construction_part = construction.GetConstructionPart( part_name );
			
			if ( construction_part && construction.IsPartConstructed( part_name ) )
			{
				construction.DestroyPartServer( null, part_name, AT_DESTROY_PART );
				construction.DestroyConnectedParts(part_name);
			}
			
			//barbed wire handling (hack-ish)
			if ( part_name.Contains("barbed") )
			{
				BarbedWire barbed_wire = BarbedWire.Cast( FindAttachmentBySlotName( zone ) );
				if (barbed_wire)
					barbed_wire.SetMountedState( false );
			}
		}
	}
	
	override void EEOnAfterLoad()
	{
		if (m_FixDamageSystemInit)
		{
			GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( SetPartsAfterStoreLoad, 500, false, this );
		}
		
		super.EEOnAfterLoad();
	}
	
	override void EEInit()
	{
		super.EEInit();
		
		// init visuals and physics
		InitBaseState();
		
		//debug
		#ifdef DEVELOPER
		DebugCustomState();
		#endif
	}

	override void EEItemAttached( EntityAI item, string slot_name )
	{
		super.EEItemAttached( item, slot_name );
		
		CheckForHybridAttachments( item, slot_name );
		UpdateVisuals();
		UpdateAttachmentPhysics( slot_name, false );
	}
	
	override void EEItemDetached( EntityAI item, string slot_name )
	{
		super.EEItemDetached( item, slot_name );

		UpdateVisuals();
		UpdateAttachmentPhysics( slot_name, false );
	}
	
	protected void OnSetSlotLock( int slotId, bool locked, bool was_locked )
	{
		string slot_name = InventorySlots.GetSlotName( slotId );
		if (LogManager.IsBaseBuildingLogEnable()) bsbDebugPrint( "inv: OnSetSlotLock " + GetDebugName( this ) + " slot=" + slot_name + " locked=" + locked + " was_locked=" + was_locked );

		UpdateAttachmentVisuals( slot_name, locked );
		UpdateAttachmentPhysics( slot_name, locked );
	}
	
	//ignore out of reach condition
	override bool IgnoreOutOfReachCondition()
	{
		return true;
	}
	
	//CONSTRUCTION EVENTS
	//Build
	void OnPartBuiltServer(notnull Man player, string part_name, int action_id)
	{
		ConstructionPart construtionPart = GetConstruction().GetConstructionPart(part_name);
		
		//check base state
		if (construtionPart.IsBase())
		{
			SetBaseState(true);

			//spawn kit
			CreateConstructionKit();
		}
			
		//register constructed parts for synchronization
		RegisterPartForSync(construtionPart.GetId());
		
		//register action that was performed on part
		RegisterActionForSync(construtionPart.GetId(), action_id);
		
		//synchronize
		SynchronizeBaseState();

		SetPartFromSyncData(construtionPart); // server part of sync, client will be synced from SetPartsFromSyncData
		
		UpdateNavmesh();
		
		//update visuals
		UpdateVisuals();
		
		//reset action sync data
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(ResetActionSyncData, 100, false, this);
	}
	
	void OnPartBuiltClient(string part_name, int action_id)
	{
		//play sound
		SoundBuildStart( part_name );
	}	
	
	//Dismantle
	void OnPartDismantledServer(notnull Man player, string part_name, int action_id)
	{
		if (LogManager.IsBaseBuildingLogEnable()) bsbDebugPrint("[bsb] " + GetDebugName(this) + " OnPartDismantledServer " + part_name);
		ConstructionPart construtionPart = GetConstruction().GetConstructionPart(part_name);
					
		//register constructed parts for synchronization
		UnregisterPartForSync(construtionPart.GetId());
		
		//register action that was performed on part
		RegisterActionForSync(construtionPart.GetId(), action_id);
		
		//synchronize
		SynchronizeBaseState();

		// server part of sync, client will be synced from SetPartsFromSyncData
		SetPartFromSyncData(construtionPart);
		
		UpdateNavmesh();
		
		//update visuals
		UpdateVisuals();
		
		//reset action sync data
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(ResetActionSyncData, 100, false, this);
		
		//check base state
		if (construtionPart.IsBase())
		{
			//Destroy construction
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(DestroyConstruction, 200, false, this);
		}
	}
	
	void OnPartDismantledClient( string part_name, int action_id )
	{
		//play sound
		SoundDismantleStart( part_name );
	}	
	
	//Destroy
	void OnPartDestroyedServer(Man player, string part_name, int action_id, bool destroyed_by_connected_part = false)
	{
		if (LogManager.IsBaseBuildingLogEnable()) bsbDebugPrint("[bsb] " + GetDebugName(this) + " OnPartDestroyedServer " + part_name);
		ConstructionPart construtionPart = GetConstruction().GetConstructionPart(part_name);
					
		//register constructed parts for synchronization
		UnregisterPartForSync(construtionPart.GetId());
		
		//register action that was performed on part
		RegisterActionForSync(construtionPart.GetId(), action_id);
		
		//synchronize
		SynchronizeBaseState();
		
		// server part of sync, client will be synced from SetPartsFromSyncData
		SetPartFromSyncData(construtionPart);
		
		UpdateNavmesh();
		
		//update visuals
		UpdateVisuals();
		
		//reset action sync data
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(ResetActionSyncData, 100, false, this);
		
		//check base state
		if (construtionPart.IsBase())
		{
			//Destroy construction
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(DestroyConstruction, 200, false, this);
		}
	}
	
	void OnPartDestroyedClient( string part_name, int action_id )
	{
		//play sound
		SoundDestroyStart( part_name );
	}
	
	//! Disconnected temporarily
	protected void HandleItemFalling(ConstructionPart part)
	{
		bool process = false;
		
		//TODO: add a parameter to parts' config classes?
		process |= part.m_PartName.Contains("_roof");
		process |= part.m_PartName.Contains("_platform");
		process |= part.m_PartName.Contains("_stair");
		
		if (process)
		{
			if (!MemoryPointExists(part.m_PartName + "_min") || !MemoryPointExists(part.m_PartName + "_max"))
			{
				Debug.Log("min/max memory points do not exist for part: " + part.m_PartName);
				return;
			}
			
			vector mins, maxs;
			mins = ModelToWorld(GetMemoryPointPos(part.m_PartName + "_min"));
			maxs = ModelToWorld(GetMemoryPointPos(part.m_PartName + "_max"));
			
			//sanitize minmaxs
			vector minTmp, maxTmp;
			minTmp[0] = Math.Min(mins[0],maxs[0]);
			maxTmp[0] = Math.Max(mins[0],maxs[0]);
			minTmp[1] = Math.Min(mins[1],maxs[1]);
			maxTmp[1] = Math.Max(mins[1],maxs[1]);
			minTmp[2] = Math.Min(mins[2],maxs[2]);
			maxTmp[2] = Math.Max(mins[2],maxs[2]);
			mins = minTmp;
			maxs = maxTmp;
			
			maxs[1] = maxs[1] + 0.35; //reach a little above..
			
			ItemFall(mins,maxs);
		}
	}
	
	//! Disconnected temporarily
	protected void ItemFall(vector min, vector max)
	{
		array<EntityAI> foundEntities = new array<EntityAI>();
		DayZPlayerUtils.PhysicsGetEntitiesInBox(min,max,foundEntities);
		
		//filtering
		ItemBase item;
		foreach (EntityAI entity : foundEntities)
		{
			if (Class.CastTo(item,entity) && !item.CanUseConstruction()) //BB items?
				item.ThrowPhysically(null,vector.Zero);
		}
	}
	
	// --- UPDATE
	void InitBaseState()
	{
		if (LogManager.IsBaseBuildingLogEnable()) bsbDebugPrint("[bsb] " + GetDebugName(this) + " BaseBuildingBase::InitBaseState ");

		InitVisuals();
		UpdateNavmesh(); //regenerate navmesh
		GetConstruction().InitBaseState();
	}	
	
	void InitVisuals()
	{
		if (LogManager.IsBaseBuildingLogEnable()) bsbDebugPrint("[bsb] " + GetDebugName(this) + " InitVisuals");
		//check base
		if ( !HasBase() )
		{
			SetAnimationPhase( ANIMATION_DEPLOYED, 0 );
		}
		else
		{
			SetAnimationPhase( ANIMATION_DEPLOYED, 1 );
		}
		
		GetConstruction().UpdateVisuals();
	}

	void UpdateVisuals()
	{
		array<string> attachmentSlots = new array<string>;

		GetAttachmentSlots(this, attachmentSlots);
		foreach (string slotName : attachmentSlots)
		{
			UpdateAttachmentVisuals(slotName, IsAttachmentSlotLocked(slotName));
		}
		
		//check base
		if (!HasBase())
			SetAnimationPhase(ANIMATION_DEPLOYED, 0);
		else
			SetAnimationPhase(ANIMATION_DEPLOYED, 1);
		
		GetConstruction().UpdateVisuals();
	}
	
	void UpdateAttachmentVisuals(string slot_name, bool is_locked)
	{
		string slotNameMounted = slot_name + "_Mounted";
		EntityAI attachment = FindAttachmentBySlotName(slot_name);
		
		if (attachment)
		{
			BarbedWire barbedWire = BarbedWire.Cast(attachment);
			if (barbedWire && barbedWire.IsMounted())
				CreateAreaDamage(slotNameMounted);
			else
				DestroyAreaDamage(slotNameMounted);
			
			if (is_locked)
			{
				SetAnimationPhase(slotNameMounted, 0);
				SetAnimationPhase(slot_name, 1);
			}
			else
			{
				SetAnimationPhase(slotNameMounted, 1);
				SetAnimationPhase(slot_name, 0);
			}
		}
		else
		{
			SetAnimationPhase(slotNameMounted, 1);
			SetAnimationPhase(slot_name, 1);
			
			DestroyAreaDamage(slotNameMounted);
		}
	}
	
	// avoid calling this function on frequent occasions, it's a massive performance hit
	void UpdatePhysics()
	{
		if (LogManager.IsBaseBuildingLogEnable())
			bsbDebugPrint("[bsb] " + GetDebugName(this) + " BaseBuildingBase::UpdatePhysics");
		
		array<string> attachmentSlots = new array<string>;
		GetAttachmentSlots(this, attachmentSlots);

		if (LogManager.IsBaseBuildingLogEnable())
			bsbDebugPrint("[bsb] " + GetDebugName(this) + " att_cnt=" + attachmentSlots.Count());

		foreach (string slotName : attachmentSlots)
		{
			UpdateAttachmentPhysics(slotName, IsAttachmentSlotLocked(slotName));
		}
		
		//check base
		if (!HasBase())
		{
			if (LogManager.IsBaseBuildingLogEnable())
				bsbDebugPrint("[bsb] " + GetDebugName(this) + ANIMATION_DEPLOYED + "  ADD");

			AddProxyPhysics(ANIMATION_DEPLOYED);
		}
		else
		{
			if (LogManager.IsBaseBuildingLogEnable())
				bsbDebugPrint("[bsb] " + GetDebugName(this) + ANIMATION_DEPLOYED + " RM");

			RemoveProxyPhysics(ANIMATION_DEPLOYED);
		}
		
		GetConstruction().UpdatePhysics();
		UpdateNavmesh();
	}
	
	void UpdateAttachmentPhysics( string slot_name, bool is_locked )
	{
		//checks for invalid appends; hotfix
		if ( !m_Mountables || m_Mountables.Find(slot_name) == -1 )
			return;
		//----------------------------------
		string slot_name_mounted = slot_name + "_Mounted";
		EntityAI attachment = FindAttachmentBySlotName( slot_name );
		
		//remove proxy physics
		if (LogManager.IsBaseBuildingLogEnable()) bsbDebugPrint("[bsb] " + GetDebugName(this) + " Removing ATT SLOT=" + slot_name + " RM / RM");
		RemoveProxyPhysics( slot_name_mounted );
		RemoveProxyPhysics( slot_name );
		
		if ( attachment )
		{
			if (LogManager.IsBaseBuildingLogEnable()) bsbDebugPrint("[bsb] " + GetDebugName(this) + " Adding ATT=" + Object.GetDebugName(attachment));
			if ( is_locked )
			{
				if (LogManager.IsBaseBuildingLogEnable()) bsbDebugPrint("[bsb] " + GetDebugName(this) + " RM / RM");
				AddProxyPhysics( slot_name_mounted );
			}
			else
			{
				if (LogManager.IsBaseBuildingLogEnable()) bsbDebugPrint("[bsb] " + GetDebugName(this) + " ADD");
				AddProxyPhysics( slot_name );
			}
		}
	}
	
	protected void UpdateNavmesh()
	{
		SetAffectPathgraph( true, false );
		GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater( GetGame().UpdatePathgraphRegionByObject, 100, false, this );
	}
	
	override bool CanUseConstruction()
	{
		return true;
	}
	
	override bool CanUseConstructionBuild()
	{
		return true;
	}

	protected bool IsAttachmentSlotLocked( EntityAI attachment )
	{
		if ( attachment )
		{
			InventoryLocation inventory_location = new InventoryLocation;
			attachment.GetInventory().GetCurrentInventoryLocation( inventory_location );
			
			return GetInventory().GetSlotLock( inventory_location.GetSlot() );
		}
			
		return false;
	}
	
	protected bool IsAttachmentSlotLocked( string slot_name )
	{
		return GetInventory().GetSlotLock( InventorySlots.GetSlotIdFromString( slot_name ) );
	}	
	
	//--- ATTACHMENT SLOTS
	void GetAttachmentSlots( EntityAI entity, out array<string> attachment_slots )
	{
		string config_path = "CfgVehicles" + " " + entity.GetType() + " " + "attachments";
		if ( GetGame().ConfigIsExisting( config_path ) )
		{
			GetGame().ConfigGetTextArray( config_path, attachment_slots );
		}
	}

	bool CheckSlotVerticalDistance( int slot_id, PlayerBase player )
	{
		return true;
	}
		
	protected bool CheckMemoryPointVerticalDistance( float max_dist, string selection, PlayerBase player )	
	{
		return true;
	}
	
	protected bool CheckLevelVerticalDistance( float max_dist, string selection, PlayerBase player )	
	{
		return true;
	}
	
	// --- INIT
	void ConstructionInit()
	{
		if ( !m_Construction )
		{
			m_Construction = new Construction( this );
		}
		
		GetConstruction().Init();
	}
	
	Construction GetConstruction()
	{
		return m_Construction;
	}
	
	//--- INVENTORY/ATTACHMENTS CONDITIONS
	//attachments
	override bool CanReceiveAttachment( EntityAI attachment, int slotId )
	{
		return super.CanReceiveAttachment(attachment, slotId);
	}
	
	bool HasAttachmentsBesidesBase()
	{
		int attachment_count = GetInventory().AttachmentCount();
		if ( attachment_count > 0 )
		{
			if ( HasBase() && attachment_count == 1 )
			{
				return false;
			}
			
			return true;
		}
		
		return false;
	}
	
	override bool ShowZonesHealth()
	{
		return true;
	}
	
	override bool IsTakeable()
	{
		return false;
	}
	
	//this into/outo parent.Cargo
	override bool CanPutInCargo( EntityAI parent )
	{
		return false;
	}
	
	override bool CanRemoveFromCargo( EntityAI parent )
	{
		return false;
	}

	//hands
	override bool CanPutIntoHands( EntityAI parent )
	{
		return false;
	}
	
	//--- ACTION CONDITIONS
	//direction
	override bool IsFacingPlayer( PlayerBase player, string selection )
	{
		return true;
	}
	
	override bool IsPlayerInside( PlayerBase player, string selection )
	{
		return true;
	}
	
	//! Some buildings can only be built from outside
	bool MustBeBuiltFromOutside()
	{
		return false;
	}
	
	//camera direction check
	bool IsFacingCamera( string selection )
	{
		return true;
	}
	
	//roof check
	bool PerformRoofCheckForBase( string partName, PlayerBase player, out bool result )
	{
		return false;
	}
	
	//selection->player distance check
	bool HasProperDistance( string selection, PlayerBase player )
	{
		return true;
	}
		
	//folding
	bool CanFoldBaseBuildingObject()
	{
		if ( HasBase() || GetInventory().AttachmentCount() > 0 )
		{
			return false;
		}
		
		return true;
	}
	
	ItemBase FoldBaseBuildingObject()
	{
		ItemBase item = CreateConstructionKit();
		DestroyConstruction();

		return item;
	}
	
	//Damage triggers (barbed wire)
	void CreateAreaDamage( string slot_name, float rotation_angle = 0 )
	{
		if ( GetGame() && GetGame().IsServer() )
		{
			//destroy area damage if some already exists
			DestroyAreaDamage( slot_name );
			
			//create new area damage
			AreaDamageLoopedDeferred_NoVehicle areaDamage = new AreaDamageLoopedDeferred_NoVehicle( this );
			areaDamage.SetDamageComponentType(AreaDamageComponentTypes.HITZONE);
			
			vector min_max[2];
			if ( MemoryPointExists( slot_name + "_min" ) )
			{
				min_max[0] = GetMemoryPointPos( slot_name + "_min" );
			}
			if ( MemoryPointExists( slot_name + "_max" ) )
			{
				min_max[1] = GetMemoryPointPos( slot_name + "_max" );
			}
			
			//get proper trigger extents (min<max)
			vector extents[2];
			GetConstruction().GetTriggerExtents( min_max, extents );
			
			//get box center
			vector center;
			center = GetConstruction().GetBoxCenter( min_max );
			center = ModelToWorld( center );
			
			//rotate center if needed
			vector orientation = GetOrientation();;
			CalcDamageAreaRotation( rotation_angle, center, orientation );
			
			areaDamage.SetExtents( extents[0], extents[1] );
			areaDamage.SetAreaPosition( center );
			areaDamage.SetAreaOrientation( orientation );
			areaDamage.SetLoopInterval( 1.0 );
			areaDamage.SetDeferDuration( 0.2 );
			areaDamage.SetHitZones( { "Torso","LeftHand","LeftLeg","LeftFoot","RightHand","RightLeg","RightFoot" } );
			areaDamage.SetAmmoName( "BarbedWireHit" );
			areaDamage.Spawn();
			
			m_DamageTriggers.Insert( slot_name, areaDamage );
		}
	}
		
	void CalcDamageAreaRotation( float angle_deg, out vector center, out vector orientation )
	{
		if ( angle_deg != 0 )
		{
			//orientation
			orientation[0] = orientation[0] - angle_deg;
			
			//center
			vector rotate_axis;
			if ( MemoryPointExists( "rotate_axis" ) )
			{
				rotate_axis = ModelToWorld( GetMemoryPointPos( "rotate_axis" ) );
			}			
			float r_center_x = ( Math.Cos( angle_deg * Math.DEG2RAD ) * ( center[0] - rotate_axis[0] ) ) - ( Math.Sin( angle_deg * Math.DEG2RAD ) * ( center[2] - rotate_axis[2] ) ) + rotate_axis[0];
			float r_center_z = ( Math.Sin( angle_deg * Math.DEG2RAD ) * ( center[0] - rotate_axis[0] ) ) + ( Math.Cos( angle_deg * Math.DEG2RAD ) * ( center[2] - rotate_axis[2] ) ) + rotate_axis[2];
			center[0] = r_center_x;
			center[2] = r_center_z;
		}
	}
		
	void DestroyAreaDamage( string slot_name )
	{
		if (GetGame() && GetGame().IsServer())
		{
			AreaDamageLoopedDeferred_NoVehicle areaDamage;
			if (m_DamageTriggers.Find(slot_name, areaDamage))
			{
				if (areaDamage)
				{
					areaDamage.Destroy();
				}
				
				m_DamageTriggers.Remove( slot_name );
			}
		}
	}
	
	override bool IsIgnoredByConstruction()
	{
		return true;
	}
	
	//================================================================
	// SOUNDS
	//================================================================
	protected void SoundBuildStart( string part_name )
	{
		PlaySoundSet( m_Sound, GetBuildSoundByMaterial( part_name ), 0.1, 0.1 );
	}

	protected void SoundDismantleStart( string part_name )
	{
		PlaySoundSet( m_Sound, GetDismantleSoundByMaterial( part_name ), 0.1, 0.1 );
	}
	
	protected void SoundDestroyStart( string part_name )
	{
		PlaySoundSet( m_Sound, GetDismantleSoundByMaterial( part_name ), 0.1, 0.1 );
	}
	
	protected string GetBuildSoundByMaterial( string part_name )
	{
		ConstructionMaterialType material_type = GetConstruction().GetMaterialType( part_name );
		
		switch ( material_type )
		{
			case ConstructionMaterialType.MATERIAL_LOG: 	return SOUND_BUILD_WOOD_LOG;
			case ConstructionMaterialType.MATERIAL_WOOD: 	return SOUND_BUILD_WOOD_PLANK;
			case ConstructionMaterialType.MATERIAL_STAIRS: 	return SOUND_BUILD_WOOD_STAIRS;
			case ConstructionMaterialType.MATERIAL_METAL: 	return SOUND_BUILD_METAL;
			case ConstructionMaterialType.MATERIAL_WIRE:	return SOUND_BUILD_WIRE;
		}
		
		return "";
	}
	
	protected string GetDismantleSoundByMaterial( string part_name )
	{
		ConstructionMaterialType material_type = GetConstruction().GetMaterialType( part_name );
		
		switch ( material_type )
		{
			case ConstructionMaterialType.MATERIAL_LOG: 	return SOUND_DISMANTLE_WOOD_LOG;
			case ConstructionMaterialType.MATERIAL_WOOD: 	return SOUND_DISMANTLE_WOOD_PLANK;
			case ConstructionMaterialType.MATERIAL_STAIRS: 	return SOUND_DISMANTLE_WOOD_STAIRS;
			case ConstructionMaterialType.MATERIAL_METAL: 	return SOUND_DISMANTLE_METAL;
			case ConstructionMaterialType.MATERIAL_WIRE:	return SOUND_DISMANTLE_WIRE;
		}
		
		return "";
	}
	
	//misc
	void CheckForHybridAttachments( EntityAI item, string slot_name )
	{
		if (!GetGame().IsMultiplayer() || GetGame().IsServer())
		{
			//if this is a hybrid attachment, set damage of appropriate damage zone. Zone name must match slot name...WIP
			if (m_HybridAttachments && m_HybridAttachments.Find(slot_name) != -1)
			{
				SetHealth(slot_name,"Health",item.GetHealth());
			}
		}
	}
	
	override int GetDamageSystemVersionChange()
	{
		return 111;
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionDetachFromTarget);
		RemoveAction(ActionTakeItem);
		RemoveAction(ActionTakeItemToHands);
	}
	
	//================================================================
	// DEBUG
	//================================================================	
	protected void DebugCustomState()
	{
	}
	
	//! Excludes certain parts from being built by OnDebugSpawn, uses Contains to compare
	array<string> OnDebugSpawnBuildExcludes()
	{
		return null;
	}
	
	override void OnDebugSpawn()
	{
		FullyBuild();
	}
	
	void FullyBuild()
	{
		array<string> excludes = OnDebugSpawnBuildExcludes();
		array<ConstructionPart> parts = GetConstruction().GetConstructionParts().GetValueArray();
		
		Man p;
		
		#ifdef SERVER
		array<Man> players = new array<Man>;
		GetGame().GetWorld().GetPlayerList(players);
		if (players.Count())
			p = players[0];
		#else
		p = GetGame().GetPlayer();
		#endif
		
		foreach (ConstructionPart part : parts)
		{
			bool excluded = false;
			string partName = part.GetPartName();
			if (excludes)
			{
				foreach (string exclude : excludes)
				{
					if (partName.Contains(exclude))
					{
						excluded = true;
						break;
					}
				}
			}
			
			if (!excluded)
			{
				OnPartBuiltServer(p, partName, AT_BUILD_PART);
			}
		}
		
		GetConstruction().UpdateVisuals();
	}
}

void bsbDebugPrint (string s)
{
#ifdef BSB_DEBUG
	PrintToRPT("" + s); // comment/uncomment to hide/see debug logs
#else
	//Print("" + s); // comment/uncomment to hide/see debug logs
#endif
}
void bsbDebugSpam (string s)
{
#ifdef BSB_DEBUG_SPAM
	PrintToRPT("" + s); // comment/uncomment to hide/see debug logs
#else
	//Print("" + s); // comment/uncomment to hide/see debug logs
#endif
}