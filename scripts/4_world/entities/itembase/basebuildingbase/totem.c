class TerritoryFlag extends BaseBuildingBase
{
	const float MAX_ACTION_DETECTION_ANGLE_RAD 		= 1.3;		//1.3 RAD = ~75 DEG
	const float MAX_ACTION_DETECTION_DISTANCE 		= 2.0;		//meters
	
	bool m_RefresherActive;
	bool m_RefresherActiveLocal;
	bool m_RefresherInitialized;
	int m_RefresherTimeRemaining;
	int m_RefreshTimeCounter;
	
	protected int m_FlagRefresherFrequency = GameConstants.REFRESHER_FREQUENCY_DEFAULT; 		//how often does the flag increase lifetimes
	protected int m_FlagRefresherMaxDuration = GameConstants.REFRESHER_MAX_DURATION_DEFAULT; 	//how long will the refresher run; multiple of m_FlagRefresherFrequency by default
	
	void TerritoryFlag()
	{
		m_RefresherActive = false;
		m_RefresherActiveLocal = false;
		m_RefresherInitialized = false;
		m_RefresherTimeRemaining = 0;
		
		if ( GetCEApi() )
		{
			InitRefresherData();
		}
		RegisterNetSyncVariableBool("m_RefresherActive");
	}
	
	void ~TerritoryFlag()
	{
		RemoveRefresherPosition();
	}
	
	void InitRefresherData()
	{
		int frequency = GetCEApi().GetCEGlobalInt("FlagRefreshFrequency");
		int max_duration = GetCEApi().GetCEGlobalInt("FlagRefreshMaxDuration");
		
		if ( frequency > 0)
		{
			m_FlagRefresherFrequency = frequency;
		}
		if ( max_duration > 0 )
		{
			m_FlagRefresherMaxDuration = max_duration;
		}
		m_RefresherInitialized = true;
	}
	
	override string GetConstructionKitType()
	{
		return "TerritoryFlagKit";
	}
	
	override int GetMeleeTargetType()
	{
		return EMeleeTargetType.NONALIGNABLE;
	}
	
	/*override bool NameOverride(out string output)
	{
		if ( m_GateState != GATE_STATE_NONE )
		{
			output = "#str_cfgvehicles_construction_part_gate";  //changes object displayed name if in 'gate' state
			output.ToUpper();
			return true;
		}
		return false;
	}*/
	
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
	
	override bool CanDisplayAttachmentCategory( string category_name )
	{
		if ( category_name == "Base" && !HasBase() )
			return true;
		else if ( category_name == "Support" && HasBase() && !GetConstruction().IsPartConstructed("support") )
			return true;
		else if ( category_name == "Pole" && GetConstruction().IsPartConstructed("support") && !GetConstruction().IsPartConstructed("pole") )
			return true;
		else if ( category_name == "Flag" && GetConstruction().IsPartConstructed("pole") )
			return true;
		else
			return false;
	}
	// ---
	
	// --- EVENTS
	override void OnStoreSave( ParamsWriteContext ctx )
	{   
		super.OnStoreSave( ctx );
		
		ctx.Write( m_RefresherTimeRemaining );
		ctx.Write( m_RefreshTimeCounter );
		ctx.Write( m_FlagRefresherMaxDuration );
		ctx.Write( m_RefresherActive );
	}
	
	override bool OnStoreLoad( ParamsReadContext ctx, int version )
	{
		if ( !super.OnStoreLoad( ctx, version ) )
			return false;
		
		//int loaded_frequency;
		int loaded_max_duration;
		
		if ( !ctx.Read( m_RefresherTimeRemaining ) )
		{
			return false;
		}
		
		if ( !ctx.Read( m_RefreshTimeCounter ) )
		{
			return false;
		}
		
		if ( !ctx.Read( loaded_max_duration ) )
		{
			return false;
		}
		
		if ( version >= 118 && !ctx.Read( m_RefresherActive ) )
		{
			return false;
		}
		
		CheckLoadedVariables(loaded_max_duration);
		AnimateFlag(1 - GetRefresherTime01());
		
		return true;
	}
	
	override void AfterStoreLoad()
	{
		super.AfterStoreLoad();
		
		if (!m_RefresherInitialized && GetCEApi())
		{
			InitRefresherData();
		}
		SetSynchDirty();
		UpdateVisuals();
	}
	
	override void OnCEUpdate()
	{
		super.OnCEUpdate();
		
		int time_elapsed_rounded = Math.Round(m_ElapsedSinceLastUpdate);
		
		if ( m_RefresherTimeRemaining > 0 )
		{
			m_RefreshTimeCounter += time_elapsed_rounded;
			if ( m_RefreshTimeCounter >= m_FlagRefresherFrequency )
			{
				GetCEApi().RadiusLifetimeReset(GetPosition(),GameConstants.REFRESHER_RADIUS);
				m_RefresherTimeRemaining = Math.Clamp(m_RefresherTimeRemaining - m_RefreshTimeCounter,0,m_FlagRefresherMaxDuration);
				m_RefreshTimeCounter = 0; //possibly carry over the rest?
				AnimateFlag( 1 - GetRefresherTime01() );
			}
		}
		
		SetRefresherActive(m_RefresherTimeRemaining > 0);
	}
	
	//! Client-side, saves positions of active lifetime refreshers to MissionGameplay
	void HandleRefreshers()
	{
		Mission mission = GetGame().GetMission();
		if (!mission.m_ActiveRefresherLocations)
			return;
		
		int idx = mission.m_ActiveRefresherLocations.Find(GetPosition());
		if ( m_RefresherActive && idx == -1 )
		{
			InsertRefresherPosition();
		}
		else if ( !m_RefresherActive && idx > -1 )
		{
			RemoveRefresherPosition(idx);
		}
	}
	
	void SetRefresherActive(bool state)
	{
		if ( m_RefresherActive != state )
		{
			m_RefresherActive = state;
			SetSynchDirty();
			
			//update on refresher activation / last update on refresher deactivation
			GetCEApi().RadiusLifetimeReset(GetPosition(),GameConstants.REFRESHER_RADIUS); //TODO spammable!!!
		}
	}
	
	void InsertRefresherPosition()
	{
		Mission mission = GetGame().GetMission();
		mission.m_ActiveRefresherLocations.Insert(GetPosition());
	}
	
	void RemoveRefresherPosition(int idx = -2)
	{
		if (!GetGame() || (GetGame().IsMultiplayer() && GetGame().IsServer()) )
			return;
		
		Mission mission = GetGame().GetMission();
		if (!mission || !mission.m_ActiveRefresherLocations)
			return;
		
		if (idx == -2)
		{
			idx = mission.m_ActiveRefresherLocations.Find(GetPosition());
		}
		
		if (idx > -1)
		{
			mission.m_ActiveRefresherLocations.Remove(idx);
		}
	}
	
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		
		if ( m_RefresherActive != m_RefresherActiveLocal )
		{
			HandleRefreshers();
			m_RefresherActiveLocal = m_RefresherActive;
		}
	}
	
	//--- BUILD EVENTS
	//CONSTRUCTION EVENTS
	override void OnPartBuiltServer( notnull Man player, string part_name, int action_id )
	{
		//ConstructionPart constrution_part = GetConstruction().GetConstructionPart( part_name );
		
		super.OnPartBuiltServer( player, part_name, action_id );
		
		//update visuals (server)
		UpdateVisuals();
	}
	
	override void OnPartDismantledServer( notnull Man player, string part_name, int action_id )
	{
		ConstructionPart constrution_part = GetConstruction().GetConstructionPart( part_name );
		
		super.OnPartDismantledServer( player, part_name, action_id );
		
		//update visuals (server)
		UpdateVisuals();
	}
	
	override void OnPartDestroyedServer( Man player, string part_name, int action_id, bool destroyed_by_connected_part = false )
	{
		super.OnPartDestroyedServer( player, part_name, action_id );
		
		//update visuals (server)
		UpdateVisuals();
	}

	//--- ATTACHMENT & CONDITIONS
	override bool CanReceiveAttachment( EntityAI attachment, int slotId ) //TODO
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
		string slot_name = InventorySlots.GetSlotName(slotId);
		//flag item attachment
		if ( slot_name == "Material_FPole_Flag" )
		{
			if ( GetConstruction().IsPartConstructed("pole") )
				return true;
			else
				return false;
		}
		//materials
		ConstructionPart part;
		ConstructionPart part_lowest;
		int part_id = -1;
		for (int i = 0; i < GetConstruction().GetConstructionParts().Count(); i++)
		{
			part = GetConstruction().GetConstructionParts().GetElement(i);
			if (!part.IsBuilt())
			{
				if (part_id == -1 || part_id > part.GetId())
				{
					part_lowest = part;
					part_id = part.GetId();
				}
			}
		}
		
		if (part_lowest /*&& !part_lowest.IsBuilt()*/)
		{
			string cfg_path = "cfgVehicles " + GetType() + " Construction " + part_lowest.GetMainPartName() + " " + part_lowest.GetPartName() + " Materials";
			string material_path;
			if ( GetGame().ConfigIsExisting(cfg_path) )
			{
				string child_name;
				string child_slot_name;
				for ( i = 0; i < GetGame().ConfigGetChildrenCount( cfg_path ); i++ )
				{
					GetGame().ConfigGetChildName( cfg_path, i, child_name );
					material_path = "" + cfg_path + " " + child_name + " slot_name";
					if ( GetGame().ConfigGetText(material_path,child_slot_name) && child_slot_name == slot_name )
					{
						return true;
					}
				}
			}
		}
		
		return false;
	}
	
	//hands
	override bool CanPutIntoHands( EntityAI parent )
	{
		return false;
	}
	
	override bool CanBeRepairedToPristine()
	{
		return true;
	}
	
	//--- ACTION CONDITIONS
	override bool IsPlayerInside( PlayerBase player, string selection )
	{
		return true; //TODO
		
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
		return true; //TODO
		
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
		return false; //TODO
		
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
		//TODO
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
	
	//================================================================
	// SOUNDS
	//================================================================
	//TODO
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionRaiseFlag);
		AddAction(ActionLowerFlag);
		AddAction(ActionFoldBaseBuildingObject);
	}
	
	/*override void UpdateVisuals()
	{
		super.UpdateVisuals();
	}*/
	
	//================================================================
	// FLAG MANIPULATION + REFRESHER
	//================================================================
	void AnimateFlag(float delta)
	{
		float temp = Math.Clamp( delta,0,1 );
		float phase_new;
		if (temp < 0.01 || temp > 0.99)
		{
			phase_new = Math.Round(temp);
		}
		else
			phase_new = temp;
		SetAnimationPhase("flag_mast",phase_new);
		
		GetInventory().SetSlotLock(InventorySlots.GetSlotIdFromString("Material_FPole_Flag"),phase_new != 1);
	}
	
	void SetRefreshTimer01(float fraction)
	{
		float temp = Math.Clamp(m_FlagRefresherMaxDuration * fraction, 0, m_FlagRefresherMaxDuration);
		m_RefresherTimeRemaining = Math.Round(temp);
	}
	
	/*void AddRefresherTimeFlat(float seconds) //seconds?
	{
		float temp = Math.Clamp(m_RefresherTimeRemaining + seconds, 0, m_FlagRefresherMaxDuration);
		m_RefresherTimeRemaining = Math.Round(temp);
		SetRefresherActive(m_RefresherTimeRemaining > 0);
	}*/
	
	void AddRefresherTime01(float fraction)
	{
		float temp = Math.Clamp(m_RefresherTimeRemaining + (fraction * m_FlagRefresherMaxDuration), 0, m_FlagRefresherMaxDuration);
		m_RefresherTimeRemaining = Math.Round(temp);
		SetRefresherActive(m_RefresherTimeRemaining > 0);
		//Print(m_RefresherTimeRemaining);
	}
	
	float GetRefresherTime01()
	{
		return m_RefresherTimeRemaining / m_FlagRefresherMaxDuration;
	}
	
	void CheckLoadedVariables(int max_duration)
	{
		if (max_duration != m_FlagRefresherMaxDuration)
		{
			m_RefresherTimeRemaining = m_FlagRefresherMaxDuration * m_RefresherTimeRemaining / max_duration;
		}
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
	
	//Debug menu Spawn Ground Special
	override void OnDebugSpawn()
	{
		super.OnDebugSpawn();

		GetInventory().CreateInInventory("Flag_DayZ");
		AnimateFlag(0);
		AddRefresherTime01(1);
	}
}
