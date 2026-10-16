class ShelterSite extends BaseBuildingBase
{
	const float MAX_ACTION_DETECTION_ANGLE_RAD 		= 1.3;		//1.3 RAD = ~75 DEG
	const float MAX_ACTION_DETECTION_DISTANCE 		= 2.0;		//meters

	override string GetConstructionKitType()
	{
		return "ShelterKit";
	}

	override int GetMeleeTargetType()
	{
		return EMeleeTargetType.NONALIGNABLE;
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

	//--- BUILD EVENTS
	//CONSTRUCTION EVENTS
	override void OnPartBuiltServer( notnull Man player, string part_name, int action_id )
	{
		ConstructionPart constrution_part = GetConstruction().GetConstructionPart( part_name );
		
		string shelter_type = "";
		switch (part_name)
		{
			case "leather": 
				shelter_type = "ShelterLeather";
			break;
			
			case "fabric": 
				shelter_type = "ShelterFabric";
			break;
			
			case "stick": 
				shelter_type = "ShelterStick";
			break;
		}
		
		if (shelter_type != "")
		{
			PlayerBase playerBase = PlayerBase.Cast(player);
			GetConstruction().DropNonUsableMaterialsServer( player, part_name );

			InventoryLocation targetIL = new InventoryLocation();
			auto lambda = BuildShelterFromShelterSiteLambda(this, shelter_type, playerBase);
			player.ServerReplaceItemWithNew(lambda);
			
			PluginAdminLog admin_log = PluginAdminLog.Cast( GetPlugin(PluginAdminLog) );
			if (admin_log)
			{
				string playerPrefix = admin_log.GetPlayerPrefix(playerBase, player.GetIdentity());
				admin_log.DirectAdminLogPrint(playerPrefix +" built " + shelter_type + " with Hands ");
			}
		}
	}
	
	override bool CanPutIntoHands( EntityAI parent )
	{
		return false;
	}

	override bool CanBeRepairedToPristine()
	{
		return true;
	}
	
	override bool MustBeBuiltFromOutside()
	{
		return true;
	}
	
	override bool IsFacingCamera( string selection )
	{
		vector ref_dir = GetDirection();
		vector cam_dir = g_Game.GetCurrentCameraDirection();
		
		//ref_dir = g_Game.GetCurrentCameraPosition() - GetPosition();
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
	
	override bool IsPlayerInside( PlayerBase player, string selection )
	{
		vector player_pos = player.GetPosition();
		vector object_pos = GetPosition();
		vector ref_dir = GetDirection();
		ref_dir[1] = 0;
		ref_dir.Normalize();
		
		vector min,max;
		min = -GetMemoryPointPos( "BoundingBox_min" );
		max = -GetMemoryPointPos( "BoundingBox_max" );
		
		vector dir_to_object = object_pos - player_pos;
		dir_to_object[1] = 0;
		float len = dir_to_object.Length();
		
		dir_to_object.Normalize();
		
		vector ref_dir_angle = ref_dir.VectorToAngles();
		vector dir_to_object_angle = dir_to_object.VectorToAngles();
		vector test_angles = dir_to_object_angle - ref_dir_angle;
		
		vector test_position = test_angles.AnglesToVector() * len;
		
		if(test_position[0] > max[0] || test_position[0] < min[0] || test_position[2] > max[2] || test_position[2] < min[2] )
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
		AddAction(ActionBuildPart);
	}
	
	override bool DestroyOnFall()
	{
		return true;
	}
	
	override void OnItemFallRebuilding()
	{
		if (!g_Game.IsServer())
			 return;
		
		DropAttachmentAndCargoItems();
		
		if (DestroyOnFall())
		{
			ItemBase constructionKit = CreateConstructionKit();
			constructionKit.ThrowPhysically(null, vector.Zero);
			g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(DeleteSafe, 100, false);
		}
	}
	
	override void OnDebugSpawn()
	{
		ItemBase item;
		
		GameInventory inventory = GetInventory();
		if (Class.CastTo(item,inventory.CreateInInventory("LongWoodenStick")))
			item.SetQuantityMax();
		if (Class.CastTo(item,inventory.CreateInInventory("Fabric")))
			item.SetQuantityMax();
	}
}
