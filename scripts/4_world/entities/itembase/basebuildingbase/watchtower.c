class Watchtower extends BaseBuildingBase
{	
	typename ATTACHMENT_BARBED_WIRE			= BarbedWire;
	typename ATTACHMENT_CAMONET 			= CamoNet;
	
	const float MAX_FLOOR_VERTICAL_DISTANCE 		= 0.5;
	
	const float MIN_ACTION_DETECTION_ANGLE_RAD 		= 0.35;		//0.35 RAD = 20 DEG
	const float MAX_ACTION_DETECTION_DISTANCE 		= 2.0;		//meters
	
	static const string BASE_VIEW_NAME				= "level_";
	static const string BASE_WALL_NAME				= "_wall_";
	static const string BASE_ROOF_NAME				= "_roof";
	static const int	MAX_WATCHTOWER_FLOORS		= 3;
	static const int	MAX_WATCHTOWER_WALLS		= 3;
	
	void Watchtower()
	{
	}
	
	override string GetConstructionKitType()
	{
		return "WatchtowerKit";
	}		
	
	override int GetMeleeTargetType()
	{
		return EMeleeTargetType.NONALIGNABLE;
	}
	
	/*override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
		
		if (component == -1)
		{
			Print("EEHitBy: " + this + "; damageType: "+ damageType +"; source: "+ source +"; component: "+ component +"; dmgZone: "+ dmgZone +"; ammo: "+ ammo +"; modelPos: "+ modelPos);
			Print("GetDamage " + damageResult.GetDamage("","Health"));
			Print("GetHighestDamage " + damageResult.GetHighestDamage("Health"));
		}
	}*/
	
	//overriden for the express purpose of handling view geometry (interaction) animations
	override void UpdateVisuals()
	{
		super.UpdateVisuals();
		
		SetAnimationPhase( "level_1", 0); //always visible
		SetAnimationPhase( "level_1_wall_1", 0); //always visible
		SetAnimationPhase( "level_1_wall_2", 0); //always visible
		SetAnimationPhase( "level_1_wall_3", 0); //always visible
		
		string part_name = "";
		bool built = false;
		
		for ( int i = 1; i < MAX_WATCHTOWER_FLOORS; ++i )
		{
			//roof checks
			part_name = "" + BASE_VIEW_NAME + i + BASE_ROOF_NAME;
			built = GetConstruction().IsPartConstructed(part_name);
			//Print(part_name);
			//Print(built);
			
			//string tmp = "";
			
			if ( built )
			{
				SetAnimationPhase( BASE_VIEW_NAME + (i + 1), 0); //show
				for ( int j = 1; j < MAX_WATCHTOWER_WALLS + 1; ++j )
				{
					//tmp = BASE_VIEW_NAME + (i + 1) + BASE_WALL_NAME + j;
					//Print(tmp);
					SetAnimationPhase( BASE_VIEW_NAME + (i + 1) + BASE_WALL_NAME + j, 0); //show
				}
			}
			else
			{
				SetAnimationPhase( BASE_VIEW_NAME + (i + 1), 1 ); //hide
				for ( j = 1; j < MAX_WATCHTOWER_WALLS + 1; ++j )
				{
					//tmp = BASE_VIEW_NAME + (i + 1) + BASE_WALL_NAME + j;
					//Print(tmp);
					SetAnimationPhase( BASE_VIEW_NAME + (i + 1) + BASE_WALL_NAME + j, 1); //hide
				}
			}
		}
	}
	
	//--- ATTACHMENT & CONDITIONS
	override bool CanReceiveAttachment( EntityAI attachment, int slotId )
	{
		if ( !super.CanReceiveAttachment( attachment, slotId ) )
			return false;

		//because CanReceiveAttachment() method can be called on all clients in the vicinity, vertical distance check needs to be skipped on clients that don't
		//interact with the object through attach action (AT_ATTACH_TO_CONSTRUCTION)
		PlayerBase player;
		if ( !GetGame().IsDedicatedServer() )
		{
			//check action initiator (AT_ATTACH_TO_CONSTRUCTION)
			player = PlayerBase.Cast( GetGame().GetPlayer() );
			if ( player )
			{
				ConstructionActionData construction_action_data = player.GetConstructionActionData();
				PlayerBase action_initiator = construction_action_data.GetActionInitiator();
				
				if ( action_initiator == player )			
				{
					construction_action_data.SetActionInitiator( NULL );				//reset action initiator
				}
				else
				{
					player = null;					//do not do vertical check (next)
				}
			}
		}
		//
			
		return CheckSlotVerticalDistance( slotId, player );
	}
	
	//can put into hands
	override bool CanPutIntoHands( EntityAI parent )
	{
		return false;
	}
	
	override bool CanBeRepairedToPristine()
	{
		return true;
	}
	
	override bool PerformRoofCheckForBase( string partName, PlayerBase player, out bool result )
	{
		if (CfgGameplayHandler.GetDisablePerformRoofCheck())
			return false;
		
		if (partName != "level_1_base" && partName != "level_2_base" && partName != "level_3_base" && partName != "level_3_roof")
		{
			return false;
		}
		
		vector center;
		vector orientation = GetOrientation();
		vector edge_length;
		vector min_max[2];
		ref array<Object> excluded_objects = new array<Object>;
		ref array<Object> collided_objects = new array<Object>;
		
		excluded_objects.Insert( this );
		excluded_objects.Insert( player );
		
		if ( partName == "level_2_base" )
		{
			min_max[0] = GetMemoryPointPos( "level_2_wall_1_down_min" );
			min_max[1] = GetMemoryPointPos( "level_2_roof_max" );
		}
		else if ( partName == "level_3_base" )
		{
			min_max[0] = GetMemoryPointPos( "level_3_wall_1_down_min" );
			min_max[1] = GetMemoryPointPos( "level_3_wall_2_up_max" );
		}
		else if ( partName == "level_3_roof" )
		{
			min_max[0] = GetMemoryPointPos( "level_3_roof_min" );
			min_max[1] = GetMemoryPointPos( "level_3_roof_max" );
		}
		else
		{
			//min_max[0] = GetMemoryPointPos( "level_1_wall_1_up_min" );
			//min_max[0] = GetMemoryPointPos( "level_1_wall_1_down_min" );
			min_max[0] = GetMemoryPointPos( "level_1_collisioncheck_min" );
			min_max[1] = GetMemoryPointPos( "level_1_roof_max" );
		}
		center = GetPosition();
		center[1] = center[1] + ( min_max[1][1] + min_max[0][1] ) / 2;
		
		edge_length[0] = min_max[1][0] - min_max[0][0];
		edge_length[2] = min_max[1][2] - min_max[0][2];
		edge_length[1] = min_max[1][1] - min_max[0][1];
		
		result = false;
		/*result = */GetGame().IsBoxCollidingGeometry( center, orientation, edge_length, ObjIntersectView, ObjIntersectGeom, excluded_objects, collided_objects );
		if ( collided_objects.Count() > 0 )
		{
			foreach ( Object o : collided_objects )
			{
				if (Building.Cast(o))
				{
					result = true;
				}
			}
		}
		return true;
	}
	
	// --- INVENTORY
	override bool CanDisplayAttachmentSlot( int slot_id )
	{
		//super
		if ( !super.CanDisplayAttachmentSlot( slot_id ) )
			return false;

		string slot_name = InventorySlots.GetSlotName(slot_id);
		slot_name.ToLower();
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		//base attachments
		if ( slot_name.Contains( "material_l1" ) || slot_name.Contains( "level_1_" ) )
		{
			if ( slot_name.Contains( "woodenlogs" ) )
			{
				return CheckMemoryPointVerticalDistance( MAX_FLOOR_VERTICAL_DISTANCE, "level_1", player );
			}
			else
			{
				return GetConstruction().IsPartConstructed( "level_1_base" ) && CheckMemoryPointVerticalDistance( MAX_FLOOR_VERTICAL_DISTANCE, "level_1", player );
			}
		}
		else if ( slot_name.Contains( "material_l2" ) || slot_name.Contains( "level_2_" ) )
		{
			if ( slot_name.Contains( "woodenlogs" ) )
			{
				return CheckMemoryPointVerticalDistance( MAX_FLOOR_VERTICAL_DISTANCE, "level_2", player );
			}
			else
			{
				return GetConstruction().IsPartConstructed( "level_2_base" ) && CheckMemoryPointVerticalDistance( MAX_FLOOR_VERTICAL_DISTANCE, "level_2", player );
			}
		}
		else if ( slot_name.Contains( "material_l3" ) || slot_name.Contains( "level_3_" ) )
		{
			if ( slot_name.Contains( "woodenlogs" ) )
			{
				return CheckMemoryPointVerticalDistance( MAX_FLOOR_VERTICAL_DISTANCE, "level_3", player );
			}
			else
			{
				return GetConstruction().IsPartConstructed( "level_3_base" ) && CheckMemoryPointVerticalDistance( MAX_FLOOR_VERTICAL_DISTANCE, "level_3", player );
			}
		}
				
		return true;
	}

	override bool CanDisplayAttachmentCategory( string category_name )
	{
		//super
		if ( !super.CanDisplayAttachmentCategory( category_name ) )
			return false;
		//
	
		category_name.ToLower();
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		//level 1
		if ( category_name.Contains( "level_1" ) )
		{
			if ( category_name.Contains( "level_1_" ) )
			{
				return GetConstruction().IsPartConstructed( "level_1_base" ) && CheckMemoryPointVerticalDistance( MAX_FLOOR_VERTICAL_DISTANCE, "level_1", player );
			}
			else
			{
				return CheckMemoryPointVerticalDistance( MAX_FLOOR_VERTICAL_DISTANCE, "level_1", player );
			}
		}
		//level 2
		if ( category_name.Contains( "level_2" ) )
		{
			if ( category_name.Contains( "level_2_" ) )
			{
				return GetConstruction().IsPartConstructed( "level_2_base" ) && CheckMemoryPointVerticalDistance( MAX_FLOOR_VERTICAL_DISTANCE, "level_2", player );
			}
			else
			{
				return GetConstruction().IsPartConstructed( "level_1_roof" ) && CheckMemoryPointVerticalDistance( MAX_FLOOR_VERTICAL_DISTANCE, "level_2", player );
			}
		}
		//level 3
		if ( category_name.Contains( "level_3" ) )
		{
			if ( category_name.Contains( "level_3_" ) )
			{
				return GetConstruction().IsPartConstructed( "level_3_base" ) && CheckMemoryPointVerticalDistance( MAX_FLOOR_VERTICAL_DISTANCE, "level_3", player );
			}
			else
			{
				return GetConstruction().IsPartConstructed( "level_2_roof" ) && CheckMemoryPointVerticalDistance( MAX_FLOOR_VERTICAL_DISTANCE, "level_3", player );
			}
		}
		
		return true;
	}	
	
	//returns true if attachment slot position is within given range
	override bool CheckSlotVerticalDistance( int slot_id, PlayerBase player )
	{
		string slot_name;
		InventorySlots.GetSelectionForSlotId( slot_id , slot_name );		
		slot_name.ToLower();
		
		//wall attachments
		//level 1
		if ( slot_name.Contains( "material_l1" ) || slot_name.Contains( "level_1_" ) )
		{
			if ( slot_name.Contains( "woodenlogs" ) )
			{
				return CheckMemoryPointVerticalDistance( MAX_FLOOR_VERTICAL_DISTANCE, "level_1", player );
			}
			else
			{
				return GetConstruction().IsPartConstructed( "level_1_base" ) && CheckMemoryPointVerticalDistance( MAX_FLOOR_VERTICAL_DISTANCE, "level_1", player );
			}
		}
		//level 2
		if ( slot_name.Contains( "material_l2" ) || slot_name.Contains( "level_2_" ) )
		{
			if ( slot_name.Contains( "material_l2w" ) || slot_name.Contains( "level_2_wall" ) )
			{
				return GetConstruction().IsPartConstructed( "level_2_base" ) && CheckMemoryPointVerticalDistance( MAX_FLOOR_VERTICAL_DISTANCE, "level_2", player );
			}
			else
			{
				if ( slot_name.Contains( "woodenlogs" ) )
				{
					return CheckMemoryPointVerticalDistance( MAX_FLOOR_VERTICAL_DISTANCE, "level_2", player );
				}
				else
				{
					return GetConstruction().IsPartConstructed( "level_1_roof" ) && CheckMemoryPointVerticalDistance( MAX_FLOOR_VERTICAL_DISTANCE, "level_2", player );
				}
			}
		}
		//level 3
		if ( slot_name.Contains( "material_l3" ) || slot_name.Contains( "level_3_" ) )
		{
			if ( slot_name.Contains( "material_l3w" ) || slot_name.Contains( "level_3_wall" ) )
			{
				return GetConstruction().IsPartConstructed( "level_3_base" ) && CheckMemoryPointVerticalDistance( MAX_FLOOR_VERTICAL_DISTANCE, "level_3", player );
			}
			else
			{
				if ( slot_name.Contains( "woodenlogs" ) )
				{
					return CheckMemoryPointVerticalDistance( MAX_FLOOR_VERTICAL_DISTANCE, "level_3", player );
				}
				else
				{
					return GetConstruction().IsPartConstructed( "level_2_roof" ) && CheckMemoryPointVerticalDistance( MAX_FLOOR_VERTICAL_DISTANCE, "level_3", player );
				}
			}
		}		
	
		return true;		
	}
	
	//returns true if player->mem_point position is within given range
	override bool CheckMemoryPointVerticalDistance( float max_dist, string selection, PlayerBase player )
	{
		if ( player )
		{
			//check vertical distance
			vector player_pos = player.GetPosition();
			vector pos;
			
			if ( MemoryPointExists( selection ) )
			{
				pos = ModelToWorld( GetMemoryPointPos( selection ) );
			}
			
			if ( Math.AbsFloat( player_pos[1] - pos[1] ) <= max_dist )
			{
				return true;
			}
			else
			{
				return false;
			}
		}			

		return true;
	}	
	
	override bool CheckLevelVerticalDistance( float max_dist, string selection, PlayerBase player )
	{
		if ( player )
		{
			if ( selection.Contains( "level_1_" ) )
				return CheckMemoryPointVerticalDistance( max_dist, "level_1", player );
			
			if ( selection.Contains( "level_2_" ) )
				return CheckMemoryPointVerticalDistance( max_dist, "level_2", player );
			
			if ( selection.Contains( "level_3_" ) )
				return CheckMemoryPointVerticalDistance( max_dist, "level_3", player );
		}
		return false;
	}
	// ---	
	override void AfterStoreLoad()
	{
		super.AfterStoreLoad();
		
		UpdateVisuals();
	}
	
	override void OnPartBuiltServer( notnull Man player, string part_name, int action_id )
	{
		super.OnPartBuiltServer( player, part_name, action_id );
		//update visuals (server)
		UpdateVisuals();
	}
	
	override void OnPartDismantledServer( notnull Man player, string part_name, int action_id )
	{
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
	
	//--- ACTION CONDITIONS
	//returns dot product of player->construction direction based on existing/non-existing reference point
	override bool IsFacingPlayer( PlayerBase player, string selection )
	{
		vector ref_pos;
		vector ref_dir;
		vector player_dir;
		float dot;
		bool has_memory_point = MemoryPointExists( selection );
		
		if ( has_memory_point )
		{
			ref_pos = ModelToWorld( GetMemoryPointPos( selection ) );
			ref_dir = ref_pos - GetPosition();
		}
		else
		{
			ref_pos = GetPosition();
			ref_dir = ref_pos - player.GetPosition();
		}
		
		ref_dir.Normalize();
		ref_dir[1] = 0;				//ignore height
		
		player_dir = player.GetDirection();
		player_dir.Normalize();
		player_dir[1] = 0;			//ignore height
		
		if ( ref_dir.Length() != 0 )
		{
			dot = vector.Dot( player_dir, ref_dir );
		}
		
		if ( has_memory_point )
		{
			if ( dot < 0 && Math.AbsFloat( dot ) > MIN_ACTION_DETECTION_ANGLE_RAD )
			{
				return true;
			}
		}
		else
		{
			if ( dot > 0 && Math.AbsFloat( dot ) > MIN_ACTION_DETECTION_ANGLE_RAD )
			{
				return true;
			}
		}
		
		return false;
	}
		
	override bool IsFacingCamera( string selection )
	{
		vector ref_pos;
		vector ref_dir;
		vector cam_dir = GetGame().GetCurrentCameraDirection();
		
		if ( MemoryPointExists( selection ) )
		{
			ref_pos = ModelToWorld( GetMemoryPointPos( selection ) );
			ref_dir = ref_pos - GetPosition();
			
			ref_dir.Normalize();
			ref_dir[1] = 0;		//ignore height
			
			cam_dir[1] = 0;		//ignore height
			
			if ( ref_dir.Length() > 0.5 )		//if the distance (m) is too low, ignore this check
			{
				float dot = vector.Dot( cam_dir, ref_dir );
			
				if ( dot < 0 )	
				{
					return true;
				}
			}
		}

		return false;
	}
	
	override bool IsPlayerInside( PlayerBase player, string selection )
	{
		if ( selection != "")
		{
			CheckLevelVerticalDistance( MAX_FLOOR_VERTICAL_DISTANCE, selection, player );
		}
		vector player_pos = player.GetPosition();
		vector tower_pos = GetPosition();
		vector ref_dir = GetDirection();
		ref_dir[1] = 0;
		ref_dir.Normalize();
		
		vector min,max;
		
		min = -GetMemoryPointPos( "interact_min" );
		max = -GetMemoryPointPos( "interact_max" );
		
		vector dir_to_tower = tower_pos - player_pos;
		dir_to_tower[1] = 0;
		float len = dir_to_tower.Length();
		

		dir_to_tower.Normalize();
		
		vector ref_dir_angle = ref_dir.VectorToAngles();
		vector dir_to_tower_angle = dir_to_tower.VectorToAngles();
		vector test_angles = dir_to_tower_angle - ref_dir_angle;
		
		vector test_position = test_angles.AnglesToVector() * len;
		
		if (test_position[0] > max[0] || test_position[0] < min[0] || test_position[2] > max[2] || test_position[2] < min[2] )
		{
			return false;
		}

		return true;
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
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionTogglePlaceObject);
		AddAction(ActionPlaceObject);
		AddAction(ActionFoldBaseBuildingObject);
	}	
	
		
	//================================================================
	// DEBUG
	//================================================================
	
	//! Excludes certain parts from being built by OnDebugSpawn, uses Contains to compare
	override array<string> OnDebugSpawnBuildExcludes()
	{
		array<string> excludes;
		
		#ifdef DEVELOPER
		bool bWood = DiagMenu.GetBool(DiagMenuIDs.DM_BUILD_WOOD);
		#else
		bool bWood = false;
		#endif
		
		if (bWood)
		{
			excludes = {"_metal_"};
		}
		else
		{
			excludes = {"_wood_"};
		}
		
		return excludes;
	}
	
	//Debug menu Spawn Ground Special
	override void OnDebugSpawn()
	{
		super.OnDebugSpawn();
		
		int i;
				
		for (i = 0; i < MAX_WATCHTOWER_FLOORS * MAX_WATCHTOWER_WALLS; ++i)
		{
			GetInventory().CreateInInventory("CamoNet");		
		}	
		
		for (i = 0; i < 2 * MAX_WATCHTOWER_WALLS; ++i)
		{
			BarbedWire wire = BarbedWire.Cast(GetInventory().CreateInInventory("BarbedWire"));		
			wire.SetMountedState(true);
		}
	}

}