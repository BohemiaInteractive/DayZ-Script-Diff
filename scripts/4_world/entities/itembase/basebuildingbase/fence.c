class Fence extends BaseBuildingBase
{
	const int GATE_STATE_NONE 				= 0;
	const int GATE_STATE_PARTIAL 			= 1;
	const int GATE_STATE_FULL 				= 2;
	
	const string ATTACHMENT_SLOT_COMBINATION_LOCK 	= "Att_CombinationLock";
	const string SOUND_GATE_OPEN_START				= "DoorWoodTowerOpen_SoundSet";
	const string SOUND_GATE_CLOSE_START				= "DoorWoodTowerClose_start_SoundSet";
	const string SOUND_GATE_CLOSE_END				= "DoorWoodTowerClose_end_SoundSet";

	//gate openining
	const float GATE_ROTATION_ANGLE_DEG 			= 100;
	const float GATE_ROTATION_TIME_APPROX			= 2000;		//ms
	
	const float MAX_ACTION_DETECTION_ANGLE_RAD 		= 1.3;		//1.3 RAD = ~75 DEG
	const float MAX_ACTION_DETECTION_DISTANCE 		= 2.0;		//meters
	
	typename ATTACHMENT_WOODEN_LOG			= WoodenLog;
	typename ATTACHMENT_COMBINATION_LOCK	= CombinationLock;
	
	string ATTSLOT_CAMONET				= "Wall_Camonet";
	string ATTSLOT_BARBEDWIRE_DOWN		= "Wall_Barbedwire_1";
	string ATTSLOT_BARBEDWIRE_UP		= "Wall_Barbedwire_2";
	
	//protected bool m_HasHinges				= false;
	//protected bool m_GateFullyConstructed 	= false;
	protected bool m_ToDiscard 				= false; //for legacy OnStoreLoad handling
	protected bool m_IsOpened 				= false;
	protected bool m_IsOpenedClient			= false;
	protected int m_GateState 				= 0;
	
	protected EffectSound m_SoundGate_Start;
	protected EffectSound m_SoundGate_End;
	
	void Fence()
	{
		//synchronized variables
		//RegisterNetSyncVariableBool( "m_HasHinges" );
		//RegisterNetSyncVariableBool( "m_GateFullyConstructed" );
		RegisterNetSyncVariableBool( "m_IsOpened" );
		RegisterNetSyncVariableInt( "m_GateState" );
	}
	
	override string GetConstructionKitType()
	{
		return "FenceKit";
	}
	
	override int GetMeleeTargetType()
	{
		return EMeleeTargetType.NONALIGNABLE;
	}
	
	//Gate
	bool HasHinges()
	{
		return m_GateState > GATE_STATE_NONE;
	}
	
	bool HasFullyConstructedGate()
	{
		return m_GateState == GATE_STATE_FULL;
	}
	
	void SetGateState( int state )
	{
		m_GateState = state;
		SetSynchDirty();
	}
	
	int GetGateState()
	{
		return m_GateState;
	}
	
	int CheckGateState()
	{
		ConstructionPart gate_part = GetConstruction().GetGateConstructionPart();
		int state = GATE_STATE_NONE;
		if( gate_part.IsBuilt() )
		{
			ConstructionPart req_part;
			array<string> req_parts = gate_part.GetRequiredParts();
			for (int i = 0; i < req_parts.Count(); i++)
			{
				req_part = GetConstruction().GetConstructionPart(req_parts.Get(i));
				if(!req_part.IsBuilt())
					break;
			}
			
			if( i != req_parts.Count() )
			{
				state = GATE_STATE_PARTIAL;
			}
			else
			{
				state = GATE_STATE_FULL;
			}
			
		}
		return state;
	}
	
	void SetOpenedState( bool state )
	{
		m_IsOpened = state;
	}
	
	override bool IsOpened()
	{
		return m_IsOpened;
	}
	
	bool IsLocked()
	{
		CombinationLock combination_lock = GetCombinationLock();
		if ( combination_lock && combination_lock.IsLocked() )
		{
			return true;
		}
		
		return false;
	}
	
	override bool NameOverride(out string output)
	{
		if ( m_GateState != GATE_STATE_NONE )
		{
			output = "#str_cfgvehicles_construction_part_gate";  //changes object displayed name if in 'gate' state
			output.ToUpper();
			return true;
		}
		return false;
	}
//
	CombinationLock GetCombinationLock()
	{
		CombinationLock combination_lock = CombinationLock.Cast( FindAttachmentBySlotName( ATTACHMENT_SLOT_COMBINATION_LOCK ) );
		return combination_lock;
	}
	
	CamoNet GetCamoNet()
	{
		CamoNet camonet = CamoNet.Cast( FindAttachmentBySlotName( "Wall_Camonet" ) );
		return camonet;
	}
	
	BarbedWire GetBarbedWire1()
	{
		BarbedWire barbedwire = BarbedWire.Cast( FindAttachmentBySlotName( "Wall_Barbedwire_1" ) );
		return barbedwire;
	}
	
	BarbedWire GetBarbedWire2()
	{
		BarbedWire barbedwire = BarbedWire.Cast( FindAttachmentBySlotName( "Wall_Barbedwire_2" ) );
		return barbedwire;
	}
	
	//--- CONSTRUCTION KIT
	override vector GetKitSpawnPosition()
	{
		if ( MemoryPointExists( "kit_spawn_position" ) )
		{
			vector position;
			position = GetMemoryPointPos( "kit_spawn_position" );
			
			return ModelToWorld( position );
		}
		
		return GetPosition();
	}	
	
	// --- INVENTORY
	override bool CanDisplayAttachmentSlot( int slot_id )
	{
		if (!super.CanDisplayAttachmentSlot(slot_id))
			return false;
		
		string slot_name = InventorySlots.GetSlotName(slot_id);
		
		if ( slot_name == "Att_CombinationLock" )
		{
			if ( !HasFullyConstructedGate() )
			{
				return false;
			}
		}
		
		if ( !GateAttachmentConditions(InventorySlots.GetSlotIdFromString(slot_name)) )
			return false;
		
		return true;
	}

	override bool CanDisplayAttachmentCategory( string category_name )
	{
		if ( category_name == "Attachments" || category_name == "Material" )
		{
			if ( !HasBase() )
			{
				return false;
			}
		}
		
		return true;
	}	
	// ---
	
	// --- EVENTS
	override void OnStoreSave( ParamsWriteContext ctx )
	{   
		super.OnStoreSave( ctx );
		
		//write
		ctx.Write( m_GateState );
		ctx.Write( m_IsOpened );
		bsbDebugPrint("[bsb] OnStoreSave - build=" + m_GateState + " opened=" + m_IsOpened);
	}
	
	override bool OnStoreLoad( ParamsReadContext ctx, int version )
	{
		if ( !super.OnStoreLoad( ctx, version ) )
			return false;

		//--- Fence data ---
		//has gate
		if (version < 110)
		{
			if ( !ctx.Read( m_ToDiscard ) )
			{
				m_ToDiscard = false;
				return false;
			}
			m_GateState = GATE_STATE_NONE;
		}
		else if ( !ctx.Read( m_GateState ) )
		{
			m_GateState = GATE_STATE_NONE;
			return false;
		}
		
		//is opened
		if ( !ctx.Read( m_IsOpened ) )
		{
			m_IsOpened = false;
			return false;
		}
		
		bsbDebugPrint("[bsb] OnStoreLoad - build=" + m_GateState + " opened=" + m_IsOpened);
		//---
		
		return true;
	}
	
	override void AfterStoreLoad()
	{	
		super.AfterStoreLoad();
		
		//set gate state
		ConstructionPart gate_part = GetConstruction().GetGateConstructionPart();
		SetGateState( CheckGateState() );
		
		//update gate state visual
		if ( IsOpened() )
		{
			OpenFence();
		}
		
		UpdateVisuals();
		
		bsbDebugPrint("[bsb] AfterStoreLoad - build=" + gate_part.IsBuilt() + " opened=" + IsOpened());
	}	
	
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();

		if ( m_IsOpenedClient != m_IsOpened )
		{
			m_IsOpenedClient = m_IsOpened;
			
			if ( m_IsOpenedClient )
			{
				OpenFence();
			}
			else
			{
				CloseFence();
			}
		}
	}
	
	//--- BUILD EVENTS
	//CONSTRUCTION EVENTS
	override void OnPartBuiltServer( notnull Man player, string part_name, int action_id )
	{
		ConstructionPart constrution_part = GetConstruction().GetConstructionPart( part_name );
		
		super.OnPartBuiltServer( player, part_name, action_id );
		
		SetGateState( CheckGateState() );
		
		//drop attachments without carrier part
		if (constrution_part.IsGate())
			//GateAttachmentsSanityCheck();
		
		//update visuals (server)
		UpdateVisuals();
	}
	
	override void OnPartDismantledServer( notnull Man player, string part_name, int action_id )
	{
		ConstructionPart constrution_part = GetConstruction().GetConstructionPart( part_name );
		
		//check gate state
		if ( constrution_part.IsGate() )
		{
			if ( IsLocked() )
			{
				CombinationLock combination_lock = CombinationLock.Cast( FindAttachmentBySlotName( ATTACHMENT_SLOT_COMBINATION_LOCK ) );
				combination_lock.UnlockServer( player , this );
			}
		}
		
		super.OnPartDismantledServer( player, part_name, action_id );
		
		SetGateState( CheckGateState() );
		
		//update visuals (server)
		UpdateVisuals();
	}
	
	override void OnPartDestroyedServer( Man player, string part_name, int action_id, bool destroyed_by_connected_part = false )
	{
		super.OnPartDestroyedServer( player, part_name, action_id );
		
		//check gate state
		ConstructionPart constrution_part = GetConstruction().GetConstructionPart( part_name );
		if ( constrution_part.IsGate() && destroyed_by_connected_part ) //avoids attachment dropping on regular hinges destruction
		{
			//drop regular attachments
			HandleDropAttachment(GetBarbedWire1());
			HandleDropAttachment(GetBarbedWire2());
			HandleDropAttachment(GetCamoNet());
			HandleDropAttachment(GetCombinationLock());
			
			//rotate back to place
			if ( IsOpened() )
				CloseFence();
		}
		if ( part_name == "wall_base_down" )
		{
			HandleDropAttachment(GetBarbedWire1());
			HandleDropAttachment(GetCombinationLock());
		}
		if ( part_name == "wall_base_up" )
		{
			HandleDropAttachment(GetBarbedWire2());
			HandleDropAttachment(GetCamoNet());
			HandleDropAttachment(GetCombinationLock());
		}
		
		SetGateState( CheckGateState() );
		//update visuals (server)
		UpdateVisuals();
	}

	//--- ATTACHMENT & CONDITIONS
	override bool CanReceiveAttachment( EntityAI attachment, int slotId )
	{
		if ( !super.CanReceiveAttachment(attachment, slotId) )
			return false;
		
		//manage action initiator (AT_ATTACH_TO_CONSTRUCTION)
		if ( !GetGame().IsDedicatedServer() )
		{
			PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
			if ( player )
			{
				ConstructionActionData construction_action_data = player.GetConstructionActionData();
				
				//reset action initiator
				construction_action_data.SetActionInitiator( NULL );				
			}
		}
		
		//conditions
		if ( attachment.Type() != ATTACHMENT_WOODEN_LOG )
		{
			if ( !HasBase() )
			{
				return false;
			}
		}
		
		if ( attachment.IsInherited( ATTACHMENT_COMBINATION_LOCK ) )
		{
			return ( HasFullyConstructedGate() && !IsOpened() );
		}
		
		if ( !GateAttachmentConditions(slotId) )
			return false;
		
		return true;
	}
	
	//hands
	override bool CanPutIntoHands( EntityAI parent )
	{
		if( !super.CanPutIntoHands( parent ) )
		{
			return false;
		}
		
		if ( HasBase() )
		{
			return false;
		}
		
		return true;
	}
	
	override bool CanBeRepairedToPristine()
	{
		return true;
	}
	
	//--- OPEN/CLOSE ACTIONS
	bool CanOpenFence()
	{
		if ( HasHinges() && !IsOpened() && !IsLocked() )
		{
			return true;
		}
		
		return false;
	}
	
	bool CanCloseFence()
	{
		if ( HasHinges() && IsOpened() )
		{
			return true;
		}
		
		return false;
	}
	
	void OpenFence()
	{
		//server or single player
		if ( GetGame().IsServer() )
		{
			float value = GATE_ROTATION_ANGLE_DEG;
			SetAnimationPhase( "Wall_Interact_Rotate", 				value );
			SetAnimationPhase( "Wall_Barbedwire_1_Mounted_Rotate", 	value );
			SetAnimationPhase( "Wall_Barbedwire_2_Mounted_Rotate", 	value );
			SetAnimationPhase( "Wall_Camonet_Rotate", 				value );
			SetAnimationPhase( "Wall_Gate_Rotate", 					value );
			SetAnimationPhase( "Wall_Base_Down_Rotate", 			value );
			SetAnimationPhase( "Wall_Base_Up_Rotate", 				value );
			SetAnimationPhase( "Wall_Wood_Down_Rotate", 			value );
			SetAnimationPhase( "Wall_Wood_Up_Rotate", 				value );
			SetAnimationPhase( "Wall_Metal_Down_Rotate", 			value );
			SetAnimationPhase( "Wall_Metal_Up_Rotate", 				value );
			
			SetOpenedState( true );
			
			//regenerate navmesh
			GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( UpdateNavmesh, GATE_ROTATION_TIME_APPROX, false );
			
			//synchronize
			SynchronizeBaseState();
		}
		
		//client or single player
		if ( !GetGame().IsDedicatedServer() )
		{
			//play sound
			SoundGateOpenStart();
		}
		
		//remove BarbedWire AreaDamageTrigger
		UpdateBarbedWireAreaDamagePos(0,true);
		
		//add check
		GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).Remove(CheckFenceClosed);
		GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( CheckFenceOpened, 0, true );
	}
	
	void CloseFence()
	{
		//server or single player
		if ( GetGame().IsServer() )
		{
			float value = 0;
			SetAnimationPhase( "Wall_Interact_Rotate", 				value );
			SetAnimationPhase( "Wall_Barbedwire_1_Mounted_Rotate", 	value );
			SetAnimationPhase( "Wall_Barbedwire_2_Mounted_Rotate", 	value );
			SetAnimationPhase( "Wall_Camonet_Rotate", 				value );
			SetAnimationPhase( "Wall_Gate_Rotate", 					value );
			SetAnimationPhase( "Wall_Base_Down_Rotate", 			value );
			SetAnimationPhase( "Wall_Base_Up_Rotate", 				value );
			SetAnimationPhase( "Wall_Wood_Down_Rotate", 			value );
			SetAnimationPhase( "Wall_Wood_Up_Rotate", 				value );
			SetAnimationPhase( "Wall_Metal_Down_Rotate", 			value );
			SetAnimationPhase( "Wall_Metal_Up_Rotate", 				value );
			
			SetOpenedState( false );
			
			//regenerate navmesh
			GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( UpdateNavmesh, GATE_ROTATION_TIME_APPROX, false );
			
			//synchronize
			SynchronizeBaseState();
		}
		
		//client or single player
		if ( !GetGame().IsDedicatedServer() )
		{
			//play sound
			SoundGateCloseStart();
		}
		
		//remove BarbedWire AreaDamageTrigger
		UpdateBarbedWireAreaDamagePos(0,true);
		
		//add check
		GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).Remove(CheckFenceOpened);
		GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( CheckFenceClosed, 0, true );
	}
	
	protected void CheckFenceOpened()
	{
		if ( GetAnimationPhase( "Wall_Gate_Rotate" ) == GATE_ROTATION_ANGLE_DEG )			//animation finished - open
		{
			UpdateBarbedWireAreaDamagePos(GetAnimationPhase( "Wall_Gate_Rotate" ));
			//remove check
			GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).Remove( CheckFenceOpened );
		}
	}
	
	protected void CheckFenceClosed()
	{
		if ( GetAnimationPhase( "Wall_Gate_Rotate" ) == 0 )			//animation finished - closed
		{
			//client or single player
			if ( !GetGame().IsDedicatedServer() )
			{
				//play sound
				if ( this ) SoundGateCloseEnd();
			}
			UpdateBarbedWireAreaDamagePos(GetAnimationPhase( "Wall_Gate_Rotate" ));
			//remove check
			GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).Remove( CheckFenceClosed );
		}
	}
	
	//Damage triggers
	override void CreateAreaDamage( string slot_name, float rotation_angle = 0 )
	{
		if ( IsOpened() )
		{
			rotation_angle = 100;
		}
		
		super.CreateAreaDamage( slot_name, rotation_angle );
	}
	
	//BarbedWire update
	void UpdateBarbedWireAreaDamagePos(float rotation_angle = 0, bool to_delete = false )
	{
		int slot_id;
		string slot_name;
		string slot_name_mounted;
		if ( GetBarbedWire1() && GetBarbedWire1().IsMounted() )
		{
			GetBarbedWire1().GetInventory().GetCurrentAttachmentSlotInfo(slot_id,slot_name);
			slot_name_mounted = slot_name + "_Mounted";
			if (to_delete)
			{
				DestroyAreaDamage( slot_name_mounted );
			}
			else
			{
				super.CreateAreaDamage( slot_name_mounted, rotation_angle );
			}
		}
		if ( GetBarbedWire2() && GetBarbedWire2().IsMounted() )
		{
			GetBarbedWire2().GetInventory().GetCurrentAttachmentSlotInfo(slot_id,slot_name);
			slot_name_mounted = slot_name + "_Mounted";
			if (to_delete)
			{
				DestroyAreaDamage( slot_name_mounted );
			}
			else
			{
				super.CreateAreaDamage( slot_name_mounted, rotation_angle );
			}
		}
	}
	
	//Here deal damage to BarbedWire when entity taking damage from it
	override void PostAreaDamageActions()
	{
		/*if (GetBarbedWire1())
		{
			//DecreaseHealth("Wall_BarbedWire_1", "", 1000); //why no dmg to wire???
			//BarbedWire wire = GetBarbedWire1();
			//wire.PostAreaDamageActions();
			//Print(GetHealth("BarbedWire1", ""));
		}*/
	}
	
	//--- ACTION CONDITIONS
	override bool IsPlayerInside( PlayerBase player, string selection )
	{
		vector player_pos = player.GetPosition();
		vector fence_pos = GetPosition();
		vector ref_dir = GetDirection();
		ref_dir[1] = 0;
		ref_dir.Normalize();
		 
		vector x[2];
		vector b1,b2;
		GetCollisionBox(x);
		b1 = x[0];
		b2 = x[1];

		vector dir_to_fence = fence_pos - player_pos;
		dir_to_fence[1] = 0;
		float len = dir_to_fence.Length();

		dir_to_fence.Normalize();
		
		vector ref_dir_angle = ref_dir.VectorToAngles();
		vector dir_to_fence_angle = dir_to_fence.VectorToAngles();
		vector test_angles = dir_to_fence_angle - ref_dir_angle;
		
		vector test_position = test_angles.AnglesToVector() * len;
		
		if(test_position[0] < b1[0] || test_position[0] > b2[0] || test_position[2] < 0.2 || test_position[2] > 2.2 )
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	
	override bool IsFacingPlayer( PlayerBase player, string selection )
	{
		vector fence_pos = GetPosition();
		vector player_pos = player.GetPosition();
		vector ref_dir = GetDirection();
		
		//vector fence_player_dir = player_pos - fence_pos;
		vector fence_player_dir = player.GetDirection();
		fence_player_dir.Normalize();
		fence_player_dir[1] = 0; 	//ignore height
		
		ref_dir.Normalize();
		ref_dir[1] = 0;			//ignore height
		
		if ( ref_dir.Length() != 0 )
		{
			float angle = Math.Acos( fence_player_dir * ref_dir );
			
			if ( angle >= MAX_ACTION_DETECTION_ANGLE_RAD )
			{
				return true;
			}
		}
		
		return false;
	}
	
	override bool IsFacingCamera( string selection )
	{
		vector ref_dir = GetDirection();
		vector cam_dir = GetGame().GetCurrentCameraDirection();
		
		//ref_dir = GetGame().GetCurrentCameraPosition() - GetPosition();
		ref_dir.Normalize();
		ref_dir[1] = 0;		//ignore height
		
		cam_dir.Normalize();
		cam_dir[1] = 0;		//ignore height
		
		if ( ref_dir.Length() != 0 )
		{
			float angle = Math.Acos( cam_dir * ref_dir );
			
			if ( angle >= MAX_ACTION_DETECTION_ANGLE_RAD )
			{
				return true;
			}
		}

		return false;
	}
	
	override bool HasProperDistance( string selection, PlayerBase player )
	{
		if ( MemoryPointExists( selection ) )
		{
			vector selection_pos = ModelToWorld( GetMemoryPointPos( selection ) );
			float distance = vector.Distance( selection_pos, player.GetPosition() );
			if ( distance >= MAX_ACTION_DETECTION_DISTANCE )
			{
				return false;
			}
		}
		
		return true;
	}
	
	override bool CanUseConstructionBuild()
	{
		return !IsOpened();
	}
	
	//================================================================
	// SOUNDS
	//================================================================
	protected void SoundGateOpenStart()
	{
		//client or single player
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSet( m_SoundGate_Start, SOUND_GATE_OPEN_START, 0.1, 0.1 );
		}
	}

	protected void SoundGateCloseStart()
	{
		//client or single player
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSet( m_SoundGate_Start, SOUND_GATE_CLOSE_START, 0.1, 0.1 );
		}
	}

	protected void SoundGateCloseEnd()
	{
		//client or single player
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSet( m_SoundGate_End, SOUND_GATE_CLOSE_END, 0.1, 0.1 );
		}
	}
	
	void GateAttachmentsSanityCheck()
	{
		ConstructionPart wall_base_down = GetConstruction().GetConstructionPart("wall_base_down");
		ConstructionPart wall_base_up = GetConstruction().GetConstructionPart("wall_base_up");
		if(GetBarbedWire1() && !wall_base_down.IsBuilt())
		{
			HandleDropAttachment(GetBarbedWire1());
			HandleDropAttachment(GetCombinationLock());
		}
		if( ( GetCamoNet() || GetBarbedWire2() ) && !wall_base_up.IsBuilt() )
		{
			HandleDropAttachment(GetBarbedWire2());
			HandleDropAttachment(GetCamoNet());
			HandleDropAttachment(GetCombinationLock());
		}
	}
	
	void HandleDropAttachment(ItemBase item)
	{
		BarbedWire wire;
		CombinationLock lock;
		if (Class.CastTo(wire,item)) //special barbed wire beh.
		{
			wire.SetMountedState( false );
			GetInventory().DropEntity(InventoryMode.SERVER, this, wire);
		}
		else if (Class.CastTo(lock,item))
		{
			lock.UnlockServer(null,this);
		}
		else if (item)//generic behaviour
		{
			GetInventory().DropEntity(InventoryMode.SERVER, this, item);
		}
	}
	
	bool GateAttachmentConditions(int slotId)
	{
		if ( GetGateState() == GATE_STATE_PARTIAL )
		{
			ConstructionPart wall_base_down = GetConstruction().GetConstructionPart("wall_base_down");
			ConstructionPart wall_base_up = GetConstruction().GetConstructionPart("wall_base_up");
			string slot_name = InventorySlots.GetSlotName(slotId);
			if ( !wall_base_up.IsBuilt() )
			{
				if ( slot_name == ATTSLOT_CAMONET || slot_name == ATTSLOT_BARBEDWIRE_UP )
				{
					return false;
				}
			}
			if ( !wall_base_down.IsBuilt() )
			{
				if ( slot_name == ATTSLOT_BARBEDWIRE_DOWN )
				{
					return false;
				}
			}
		}
		return true;
	}
	
	//specific selection for camonet attaching (other ones might be animated via rotation!)
	override bool TranslateSlotFromSelection(string selection_name, out int slot_id)
	{
		if ( selection_name == "wall_camonet_attach" )
		{
			slot_id = InventorySlots.GetSlotIdFromString("Wall_Camonet");
			return true;
		}
		return false;
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionTogglePlaceObject);
		AddAction(ActionPlaceObject);
		AddAction(ActionFoldBaseBuildingObject);
		//AddAction(ActionDialCombinationLockOnTarget);
		//AddAction(ActionNextCombinationLockDialOnTarget);
		AddAction(ActionOpenFence);
		AddAction(ActionCloseFence);
	}
	
	//================================================================
	// DEBUG
	//================================================================	
	/*
	override void DebugCustomState()
	{
		//debug
		m_SyncParts01 = 881;		//full fence with gate
		m_HasHinges = true;
		m_HasBase = true;
		
		OnVariablesSynchronized();
	}
	*/
	
	//! Excludes certain parts from being built by OnDebugSpawn, uses Contains to compare
	override array<string> OnDebugSpawnBuildExcludes()
	{
		array<string> excludes = {};
		
		#ifdef DEVELOPER
		bool bWood = DiagMenu.GetBool(DiagMenuIDs.DM_BUILD_WOOD);
		#else
		bool bWood = false;
		#endif
		
		if (bWood)
		{
			excludes.Insert("_metal_");
		}
		else
		{
			excludes.Insert("_wood_");
		}
		
		#ifdef DEVELOPER
		bool bGate = DiagMenu.GetBool(DiagMenuIDs.DM_BUILD_GATE);
		#else
		bool bGate = false;
		#endif
		
		if (bGate)
		{
			excludes.Insert("platform");
		}
		else
		{
			excludes.Insert("gate");
		}
		
		return excludes;
	}
			
	//Debug menu Spawn Ground Special
	override void OnDebugSpawn()
	{
		super.OnDebugSpawn();
		
		GetInventory().CreateInInventory("CamoNet");
		
		for (int i = 0; i < 2; ++i)
		{
			BarbedWire wire = BarbedWire.Cast(GetInventory().CreateInInventory("BarbedWire"));		
			wire.SetMountedState(true);
		}
	}
}
