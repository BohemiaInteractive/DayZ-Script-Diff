//BASE BUILDING BASE
class BaseBuildingBase extends ItemBase
{
	const string 		ANIMATION_DEPLOYED			= "Deployed";
	
	float 				m_ConstructionKitHealth;			//stored health value for used construction kit

	ref Construction 	m_Construction;
	
	bool 				m_HasBase = false;
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
		m_DamageTriggers = new ref map<string, ref AreaDamageManager>;
		
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
			ConfigGetTextArray("hybridAttachments",m_HybridAttachments);
		}
		if (ConfigIsExisting("mountables"))
		{
			m_Mountables = new array<string>;
			ConfigGetTextArray("mountables",m_Mountables);
		}
		
		ProcessInvulnerabilityCheck(GetInvulnerabilityTypeString());
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
		bsbDebugPrint("[bsb] " + GetDebugName(this) + " OnVariablesSynchronized");
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
				bsbDebugPrint("[bsb] " + GetDebugName(this) + " SetPartsFromSyncData +++ " + key);
				GetConstruction().AddToConstructedParts( key );
				GetConstruction().ShowConstructionPartPhysics(part.GetPartName());
				
				if (is_base)
				{
					bsbDebugPrint("[bsb] " + GetDebugName(this) + ANIMATION_DEPLOYED + " RM");
					RemoveProxyPhysics( ANIMATION_DEPLOYED );
				}
			}
		}
		else
		{
			if ( part.IsBuilt() )
			{
				bsbDebugPrint("[bsb] " + GetDebugName(this) + " SetPartsFromSyncData --- " + key);
				GetConstruction().RemoveFromConstructedParts( key );
				GetConstruction().HideConstructionPartPhysics(part.GetPartName());
			
				if (is_base)
				{
					bsbDebugPrint("[bsb] " + GetDebugName(this) + ANIMATION_DEPLOYED + " ADD");
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
		bsbDebugPrint("[bsb] " + GetDebugName(this) + " DestroyConstruction");
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
		bsbDebugPrint( "inv: OnSetSlotLock " + GetDebugName( this ) + " slot=" + slot_name + " locked=" + locked + " was_locked=" + was_locked );

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
	void OnPartBuiltServer( notnull Man player, string part_name, int action_id )
	{
		ConstructionPart constrution_part = GetConstruction().GetConstructionPart( part_name );
		
		//check base state
		if ( constrution_part.IsBase() )
		{
			SetBaseState( true );
			
			//spawn kit
			CreateConstructionKit();
		}
			
		//register constructed parts for synchronization
		RegisterPartForSync( constrution_part.GetId() );
		
		//register action that was performed on part
		RegisterActionForSync( constrution_part.GetId(), action_id );
		
		//synchronize
		SynchronizeBaseState();
		
		//if (GetGame().IsMultiplayer() && GetGame().IsServer())
			SetPartFromSyncData(constrution_part); // server part of sync, client will be synced from SetPartsFromSyncData
		
		//update visuals
		UpdateVisuals();
		
		//reset action sync data
		GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( ResetActionSyncData, 100, false, this );
	}
	
	void OnPartBuiltClient( string part_name, int action_id )
	{
		//play sound
		SoundBuildStart( part_name );
	}	
	
	//Dismantle
	void OnPartDismantledServer( notnull Man player, string part_name, int action_id )
	{
		bsbDebugPrint("[bsb] " + GetDebugName(this) + " OnPartDismantledServer " + part_name);
		ConstructionPart constrution_part = GetConstruction().GetConstructionPart( part_name );
					
		//register constructed parts for synchronization
		UnregisterPartForSync( constrution_part.GetId() );
		
		//register action that was performed on part
		RegisterActionForSync( constrution_part.GetId(), action_id );
		
		//synchronize
		SynchronizeBaseState();

		// server part of sync, client will be synced from SetPartsFromSyncData
		SetPartFromSyncData( constrution_part );
		
		//update visuals
		UpdateVisuals();
		
		//reset action sync data
		GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( ResetActionSyncData, 100, false, this );
		
		//check base state
		if ( constrution_part.IsBase() )
		{
			//Destroy construction
			GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( DestroyConstruction, 200, false, this );
		}		
	}
	
	void OnPartDismantledClient( string part_name, int action_id )
	{
		//play sound
		SoundDismantleStart( part_name );
	}	
	
	//Destroy
	void OnPartDestroyedServer( Man player, string part_name, int action_id, bool destroyed_by_connected_part = false )
	{
		bsbDebugPrint("[bsb] " + GetDebugName(this) + " OnPartDestroyedServer " + part_name);
		ConstructionPart constrution_part = GetConstruction().GetConstructionPart( part_name );
					
		//register constructed parts for synchronization
		UnregisterPartForSync( constrution_part.GetId() );
		
		//register action that was performed on part
		RegisterActionForSync( constrution_part.GetId(), action_id );
		
		//synchronize
		SynchronizeBaseState();
		
		// server part of sync, client will be synced from SetPartsFromSyncData
		SetPartFromSyncData( constrution_part );
		
		//update visuals
		UpdateVisuals();
		
		//reset action sync data
		GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( ResetActionSyncData, 100, false, this );
		
		//check base state
		if ( constrution_part.IsBase() )
		{
			//Destroy construction
			GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( DestroyConstruction, 200, false, this );
		}			
	}
	
	void OnPartDestroyedClient( string part_name, int action_id )
	{
		//play sound
		SoundDestroyStart( part_name );
	}	
	
	// --- UPDATE
	void InitBaseState()
	{
		bsbDebugPrint("[bsb] " + GetDebugName(this) + " BaseBuildingBase::InitBaseState ");

		InitVisuals();
		UpdateNavmesh(); //regenerate navmesh
		GetConstruction().InitBaseState();
	}	
	
	void InitVisuals()
	{
		bsbDebugPrint("[bsb] " + GetDebugName(this) + " InitVisuals");
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
		//update attachments visuals
		ref array<string> attachment_slots = new ref array<string>;
		GetAttachmentSlots( this, attachment_slots );
		for ( int i = 0; i < attachment_slots.Count(); i++ )
		{
			string slot_name = attachment_slots.Get( i );
			UpdateAttachmentVisuals( slot_name, IsAttachmentSlotLocked( slot_name ) );
		}
		
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
	
	void UpdateAttachmentVisuals( string slot_name, bool is_locked )
	{
		string slot_name_mounted = slot_name + "_Mounted";
		EntityAI attachment = FindAttachmentBySlotName( slot_name );
		
		if ( attachment )
		{
			//manage damage area trigger
			if ( attachment.IsInherited( BarbedWire ) )
			{
				BarbedWire barbed_wire = BarbedWire.Cast( attachment );
				if ( barbed_wire.IsMounted() )
				{
					CreateAreaDamage( slot_name_mounted );			//create damage trigger if barbed wire is mounted
				}
				else
				{
					DestroyAreaDamage( slot_name_mounted );			//destroy damage trigger if barbed wire is not mounted
				}
				//Print("attachment.IsInherited( BarbedWire ): " + slot_name_mounted);
			}
			
			if ( is_locked )
			{
				SetAnimationPhase( slot_name_mounted, 0 );
				SetAnimationPhase( slot_name, 1 );
			}
			else
			{
				SetAnimationPhase( slot_name_mounted, 1 );
				SetAnimationPhase( slot_name, 0 );
			}
		}
		else
		{
			SetAnimationPhase( slot_name_mounted, 1 );
			SetAnimationPhase( slot_name, 1 );
			
			//remove area damage trigger
			DestroyAreaDamage( slot_name_mounted );			//try to destroy damage trigger if barbed wire is not present
			//Print("DestroyAreaDamage(slot_name_mounted): " + slot_name_mounted);
		}
	}
	
	// avoid calling this function on frequent occasions, it's a massive performance hit
	void UpdatePhysics()
	{
		//update attachments physics
		bsbDebugPrint("[bsb] " + GetDebugName(this) + " BaseBuildingBase::UpdatePhysics");
		
		ref array<string> attachment_slots = new ref array<string>;
		GetAttachmentSlots( this, attachment_slots );
		bsbDebugPrint("[bsb] " + GetDebugName(this) + " att_cnt=" + attachment_slots.Count());
		for ( int i = 0; i < attachment_slots.Count(); i++ )
		{
			string slot_name = attachment_slots.Get( i );
			UpdateAttachmentPhysics( slot_name, IsAttachmentSlotLocked( slot_name ) );
		}
		
		//check base
		if ( !HasBase() )
		{
			bsbDebugPrint("[bsb] " + GetDebugName(this) + ANIMATION_DEPLOYED + "  ADD");
			AddProxyPhysics( ANIMATION_DEPLOYED );
		}
		else
		{
			bsbDebugPrint("[bsb] " + GetDebugName(this) + ANIMATION_DEPLOYED + " RM");
			RemoveProxyPhysics( ANIMATION_DEPLOYED );
		}
		
		GetConstruction().UpdatePhysics();
		
		//regenerate navmesh
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
		bsbDebugPrint("[bsb] " + GetDebugName(this) + " Removing ATT SLOT=" + slot_name + " RM / RM");
		RemoveProxyPhysics( slot_name_mounted );
		RemoveProxyPhysics( slot_name );
		
		if ( attachment )
		{
			bsbDebugPrint("[bsb] " + GetDebugName(this) + " Adding ATT=" + Object.GetDebugName(attachment));
			if ( is_locked )
			{
				bsbDebugPrint("[bsb] " + GetDebugName(this) + " RM / RM");
				AddProxyPhysics( slot_name_mounted );
			}
			else
			{
				bsbDebugPrint("[bsb] " + GetDebugName(this) + " ADD");
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
			AreaDamageLoopedDeferred_NoVehicle area_damage = new AreaDamageLoopedDeferred_NoVehicle( this );
			area_damage.SetDamageComponentType(AreaDamageComponentTypes.HITZONE);
			
			//Print("BBB | area_damage: " + area_damage + " | slot_name: " + slot_name);
			
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
			
			area_damage.SetExtents( extents[0], extents[1] );
			area_damage.SetAreaPosition( center );
			area_damage.SetAreaOrientation( orientation );
			area_damage.SetLoopInterval( 1.0 );
			area_damage.SetDeferDuration( 0.2 );
			area_damage.SetHitZones( { "Torso","LeftHand","LeftLeg","LeftFoot","RightHand","RightLeg","RightFoot" } );
			area_damage.SetAmmoName( "BarbedWireHit" );
			area_damage.Spawn();
			
			m_DamageTriggers.Insert( slot_name, area_damage );
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
		if ( GetGame() && GetGame().IsServer() )
		{
			AreaDamageLoopedDeferred_NoVehicle area_damage;
			if ( m_DamageTriggers.Find( slot_name, area_damage ) ) 
			{
				if ( area_damage )
				{
					//Print("DestroyAreaDamage: " + area_damage);
					area_damage.Destroy();
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
		//AddAction(ActionTakeHybridAttachment);
		//AddAction(ActionTakeHybridAttachmentToHands);
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
	//Print("" + s); // comment/uncomment to hide/see debug logs
}
void bsbDebugSpam (string s)
{
	//Print("" + s); // comment/uncomment to hide/see debug logs
}