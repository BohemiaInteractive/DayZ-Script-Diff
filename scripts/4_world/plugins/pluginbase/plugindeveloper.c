typedef Param7<EntityAI, string, float, float, bool, string, FindInventoryLocationType> DevSpawnItemParams;//(target, item_name, health, quantity, special, presetName, locationType );
class PluginDeveloper extends PluginBase
{
	protected bool									m_IsWinHolding;	
	protected int									m_FeaturesMask;
	UIScriptedMenu									m_ScriptConsole;
	
	static PluginDeveloper GetInstance()
	{
		return PluginDeveloper.Cast( GetPlugin( PluginDeveloper ) );
	}
	
	//! Set Player position at his cursor position in the world
	void TeleportAtCursor()
	{
		DeveloperTeleport.TeleportAtCursorEx();
	}
	
	//! Teleport player at position
	void Teleport(PlayerBase player, vector position)
	{
		DeveloperTeleport.SetPlayerPosition(player, position);
	}
	
	//! Set player direction
	void SetDirection(PlayerBase player, vector direction)
	{
		DeveloperTeleport.SetPlayerDirection(player, direction);
	}
	
	//! Enable / Disable Free camera (Fly mod)
	void ToggleFreeCameraBackPos()
	{
		DeveloperFreeCamera.FreeCameraToggle( PlayerBase.Cast( g_Game.GetPlayer() ), false );
	}
	
	//! Enable / Disable Free camera (Fly mod) - disable of camera will teleport player at current free camera position.
	void ToggleFreeCamera()
	{
		DeveloperFreeCamera.FreeCameraToggle( PlayerBase.Cast( g_Game.GetPlayer() ), true );
	}
		
	bool IsEnabledFreeCamera()
	{
		return DeveloperFreeCamera.IsFreeCameraEnabled();
	}	
	
	// System Public Events
	void PluginDeveloper()
	{
		
	}

	override void OnInit()
	{
		super.OnInit();
		
		DeveloperFreeCamera.OnInit();
	}
	
	void OnRPC(PlayerBase player, int rpc_type, ParamsReadContext ctx)
	{
		#ifdef DIAG_DEVELOPER
		switch (rpc_type)
		{
			case ERPCs.DEV_RPC_SPAWN_ITEM_ON_GROUND:
				OnRPCSpawnEntityOnGround(player, ctx);
				break;

			case ERPCs.DEV_RPC_SPAWN_ITEM_ON_GROUND_PATTERN_GRID:
				OnRPCSpawnEntityOnGroundPatternGrid(player, ctx);
				break;

			case ERPCs.DEV_RPC_SPAWN_ITEM_ON_CURSOR:
				OnRPCSpawnEntityOnCursorDir(player, ctx);
				break;

			case ERPCs.DEV_RPC_SPAWN_ITEM_IN_INVENTORY:
				OnRPCSpawnEntity(player, ctx);
				break;

			case ERPCs.DEV_RPC_CLEAR_INV:
				OnRPCClearInventory(player);
				break;

			case ERPCs.DEV_RPC_SEND_SERVER_LOG:
				OnRPCServerLogRecieved(ctx);
				break;

			case ERPCs.RPC_SYNC_SCENE_OBJECT:
				OnRPCSyncSceneObject(ctx);
				break;

			case ERPCs.DEV_RPC_PLUGIN_DZCREATURE_DEBUG:
				OnRPCPluginDayzCreatureDebug(player, rpc_type, ctx);
				break;

			case ERPCs.DEV_RPC_SPAWN_PRESET:
				string presetName;
				EntityAI target;
				
				ctx.Read(presetName);
				ctx.Read(target);
				
				HandlePresetSpawn(player,presetName, target);
				break;

			case ERPCs.DEV_RPC_SET_TIME:
				HandleSetTime(player,ctx);
				break;
		}
		
		DeveloperTeleport.OnRPC(player, rpc_type, ctx);		
		#endif
	}
	
	// Public API
	// Send Print to scripted console
	void PrintLogClient(string msg_log)
	{	
		if ( g_Game.GetUIManager().IsMenuOpen(MENU_SCRIPTCONSOLE) )
		{
			Param1<string> msg_p = new Param1<string>(msg_log);
			CallMethod(CALL_ID_SCR_CNSL_ADD_PRINT, msg_p);
		}
	}	
	
	// Server Log Synch: Server Side
	void SendServerLogToClient(string msg)
	{
		if ( g_Game )
		{
			array<Man> players = new array<Man>;
			g_Game.GetPlayers( players );
			
			for ( int i = 0; i < players.Count(); ++i )
			{
				Param1<string> param = new Param1<string>( msg );
				Man player = players.Get(i);
				
				if ( player  &&  player.HasNetworkID() )
				{
					player.RPCSingleParam(ERPCs.DEV_RPC_SEND_SERVER_LOG, param, true, player.GetIdentity());
				}
			}
		}
	}
	
	void OnRPCSyncSceneObject( ParamsReadContext ctx )
	{
		Param3<string, EntityAI, Param> par = new Param3<string, EntityAI, Param>( "", NULL, new Param );

		if ( ctx.Read( par ) )
		{		
			PluginSceneManager module_scene_editor = PluginSceneManager.Cast( GetPlugin( PluginSceneManager ) );
			SceneData	scene_data = module_scene_editor.GetLoadedScene();
			SceneObject scene_object = scene_data.GetSceneObjectByEntityAI( par.param2 );
			
			g_Game.GetCallQueue( CALL_CATEGORY_SYSTEM ).CallByName( scene_object, par.param1, par.param3 );
		}
	}
	
	void OnRPCPluginDayzCreatureDebug(PlayerBase player, int rpc_type, ParamsReadContext ctx)
	{
		PluginDayZCreatureAIDebug pluginDZCreatureDebug = PluginDayZCreatureAIDebug.Cast( GetPlugin( PluginDayZCreatureAIDebug ) );
		if (pluginDZCreatureDebug)
		{
			pluginDZCreatureDebug.OnRpc(player, rpc_type, ctx);
		}
	}

	void HandleSetTime(PlayerBase player, ParamsReadContext ctx)
	{
		#ifdef DEVELOPER
		Param5<int,int,int,int,int> p5 = new Param5<int,int,int,int,int>(0,0,0,0,0);
		if (ctx.Read(p5))
		{	
			int year = p5.param1;
			int month = p5.param2;
			int day = p5.param3;
			int hour = p5.param4;
			int minute = p5.param5;
	
			g_Game.GetWorld().SetDate(year,month, day, hour, minute);
		}		
		#endif
	}
		
	void HandlePresetSpawn(PlayerBase player, string presetName, EntityAI target)
	{
		#ifdef DEVELOPER
		
		PlayerBase targetPlayer = player;
		
		if (target)
		{	
			targetPlayer = PlayerBase.Cast(target);
		}
		if (!targetPlayer)
			return;
		
		targetPlayer.m_PresetSpawned = true;
		string class_name = presetName + "_Preset";
		typename type = class_name.ToType();
		
		if (type)
		{
			PresetSpawnBase presetObj = PresetSpawnBase.Cast(type.Spawn());
			
			if (presetObj)
			{
				presetObj.Init(targetPlayer);
			}
		}

		#endif
	}
	
	// RPC Events
	// Server Log Synch: Client Side
	void OnRPCServerLogRecieved(ParamsReadContext ctx)
	{
		Param1<string> param = new Param1<string>( "" );
		
		if ( ctx.Read(param) && param.param1 != "" )
		{
			Debug.ReceivedLogMessageFromServer(param.param1);
		}
	}
	
	void OnRPCClearInventory(PlayerBase player)
	{
		ClearInventory(player);
	}
	
	// Client -> Server Spawning: Server Side
	void OnRPCSpawnEntityOnCursorDir(PlayerBase player, ParamsReadContext ctx)
	{
		Param7<string, float, float, float, bool, string, bool> p = new Param7<string, float, float, float, bool, string, bool>("", 0, 0, 0, false, "", false);
		if ( ctx.Read(p) )
		{
			SpawnEntityOnCursorDir(player, p.param1, p.param2, p.param3, p.param4, p.param5, p.param6, p.param7);
		}
	}
	
	void OnRPCSpawnEntityOnGround(PlayerBase player, ParamsReadContext ctx)
	{
		Param6<string, float, float, vector, bool, bool> p = new Param6<string, float, float, vector, bool, bool>("", 0, 0, "0 0 0", false, false);
		if ( ctx.Read(p) )
		{
			SpawnEntityOnGroundPos(player, p.param1, p.param2,  p.param3,  p.param4, p.param5, p.param6);
		}
	}
	
	void OnRPCSpawnEntityOnGroundPatternGrid(PlayerBase player, ParamsReadContext ctx)
	{
		auto p = new Param10<string,int, float, float, int, int, float, float, bool, bool>("",0,0,0,0,0,0,0, false, false);
		if ( ctx.Read(p) )
		{
			SpawnEntityOnGroundPatternGrid(player, p.param1, p.param2, p.param3, p.param4, p.param5, p.param6, p.param7, p.param8, p.param9, p.param10);
		}
	}
	void OnRPCSpawnEntity(PlayerBase player, ParamsReadContext ctx)
	{
		#ifdef DEVELOPER
		DevSpawnItemParams p = new DevSpawnItemParams(null,"", 0, 0, false, "", FindInventoryLocationType.ANY);
		if ( ctx.Read(p) )
		{
			EntityAI target = EntityAI.Cast(p.param1);
			PlayerBase playerTarget = PlayerBase.Cast(target);

			EntityAI ent = SpawnEntityInInventory( target, p.param2,  p.param3, p.param4, p.param5, "", p.param7);
			if (playerTarget && p.param5)
			{
				if (playerTarget.m_PresetSpawned)
				{
					playerTarget.m_PresetSpawned = false;
					playerTarget.m_PresetItems.Clear();
				}
				if (ent)
				{
					playerTarget.m_PresetItems.Insert(ent);
				}
			}
		}
		#endif
	}
	
	void OnSetFreeCameraEvent( PlayerBase player, FreeDebugCamera camera )
	{
		DeveloperFreeCamera.OnSetFreeCameraEvent( player, camera );
	}
	
	void OnSpawnErrorReport (string name)
	{
		PrintString("PluginDeveloper.SpawnEntity() Warning- Cant spawn object: " + name);
		//DumpStack();
		PrintString("PluginDeveloper.SpawnEntity() Warning END");
	}

	void SetupSpawnedEntity(PlayerBase player, EntityAI entity, float health, float quantity = -1, bool special = false, string presetName = "")
	{
		#ifdef DEVELOPER
		if (presetName && player && player.m_PresetSpawned)//this is how we ascertain this is the first item being spawned from a new preset after a previous preset was already spawned in
		{
			player.m_PresetItems.Clear();
			player.m_PresetSpawned = false;//is set to 'true' elsewhere after all the items have been spawned in
		}
		if ( entity.IsInherited( PlayerBase ) ) 
		{
			PlayerBase plr = PlayerBase.Cast( entity );
			plr.OnSpawnedFromConsole();
		}
		else if ( entity.IsInherited(ItemBase) )
		{
			ItemBase item = ItemBase.Cast( entity );
			SetupSpawnedItem(item, health, quantity);
		}
		else if (entity.IsInherited(House))
		{
			entity.PlaceOnSurface();
			vector pos = entity.GetPosition();
			vector ori = g_Game.GetSurfaceOrientation(pos[0], pos[2]);
			entity.SetOrientation(ori);
		}
		if (player && presetName)
		{
			player.m_PresetItems.Insert(entity);
		}

		if (special)
		{
			if (player)
			{
				auto debugParamsPlayer = DebugSpawnParams.WithPlayer(player);
				entity.OnDebugSpawnEx(debugParamsPlayer);
			}
			else
			{
				auto debugParamsNone = DebugSpawnParams.None();
				entity.OnDebugSpawnEx(debugParamsNone);
			}
		}
		#endif
	}
	
	
	void SpawnEntityOnGroundPatternGrid( PlayerBase player, string item_name, int count, float health, float quantity, int rows, int columns, float gapRow = 1, float gapColumn = 1, bool special= false, bool withPhysics = false)
	{
		if (!item_name)
		{
			return;
		}
		if ( g_Game.IsServer() )
		{	
			float rowDist = 0;	
			float columnDist = 0;
			
			vector playerPos;
			vector camDirForward;

			GetCameraDirections(player, false, playerPos, camDirForward);

			vector camDirRight = camDirForward.Perpend() * -1;
			int countLoop = 0;
			for (int i = 0; i < rows; i++)
			{
				vector posRow = playerPos + camDirForward * rowDist;
				vector offsetSide;
				columnDist = 0;
				for (int j = 0; j < columns; j++)
				{
					offsetSide = camDirRight * columnDist;
					vector placement = posRow + offsetSide;
					float hlth = health * MiscGameplayFunctions.GetTypeMaxGlobalHealth( item_name );
					EntityAI ent = SpawnEntityOnGroundPos(player, item_name, hlth, quantity, placement, special );
					ent.PlaceOnSurface();
			
					InventoryItem item;
					if (Class.CastTo(item, ent) && withPhysics)
						item.ThrowPhysically(null, "0 0 0");
					
					countLoop++;
					if (countLoop == count)
					{
						return;
					}
					columnDist += gapColumn;
				}
				rowDist += gapRow;
			}
		}
		else
		{
			auto params = new Param10<string, int, float, float, int, int, float, float, bool, bool>(item_name, count, health, quantity, rows, columns, gapRow, gapColumn, special, withPhysics);
			player.RPCSingleParam(ERPCs.DEV_RPC_SPAWN_ITEM_ON_GROUND_PATTERN_GRID, params, true);
		}
	}
	
	EntityAI SpawnAI(string object_name, vector pos)
	{
		bool is_ai = g_Game.IsKindOf(object_name, "DZ_LightAI");
		if (is_ai)
		{
			return EntityAI.Cast(g_Game.CreateObjectEx(object_name, pos, ECE_PLACE_ON_SURFACE|ECE_INITAI|ECE_EQUIP_ATTACHMENTS));
		}
		return NULL;
	}

	void GetCameraDirections(Man player, bool allowFreeflight, out vector position, out vector direction)
	{
		position = g_Game.GetCurrentCameraPosition();
		direction = g_Game.GetCurrentCameraDirection();

		if ((g_Game.IsDedicatedServer() || allowFreeflight) && FreeDebugCamera.GetInstance().IsActive())
		{
			position = FreeDebugCamera.GetInstance().GetPosition();
			direction = FreeDebugCamera.GetInstance().GetDirection();
			return;
		}

		if (player && !allowFreeflight)
		{			
			position = player.GetPosition();
			direction = player.GetDirection();
		}
	}

	EntityAI SpawnEntityOnGroundPos(PlayerBase player, string object_name, vector pos)
	{
		if (player)
		{
			return player.SpawnEntityOnGroundPos(object_name, pos);
		}

		bool is_AI = g_Game.IsKindOf(object_name, "DZ_LightAI");
		if (is_AI)
		{
			return SpawnAI(object_name, pos);
		}
		else
		{
			InventoryLocation inv_loc = new InventoryLocation;
			vector mtx[4];
			Math3D.MatrixIdentity4(mtx);
			mtx[3] = pos;
			inv_loc.SetGround(null, mtx);
			
			int flags = ECE_PLACE_ON_SURFACE;
			#ifdef DEVELOPER
			if (g_Game.IsKindOf(object_name, "Boat"))
				flags = ECE_KEEPHEIGHT;
			#endif

			return EntityAI.Cast(g_Game.CreateObjectEx(object_name, inv_loc.GetPos(), flags));
		}

		return null;
	}

	void SpawnItemOnCrosshair(PlayerBase player, string itemName, float health, float quantity, float maxDist = 100, bool allowFreeflight = false, bool special = false, bool withPhysics = false)
	{
		vector from, to, dir;
		GetCameraDirections(player, true, from, dir);
		to = from + (dir * maxDist);
		
		float hitFraction;
		vector start, end;
		vector direction;

		vector hitPos, hitNormal;
		Object obj;
		
		PhxInteractionLayers hitMask =  PhxInteractionLayers.BUILDING | PhxInteractionLayers.DOOR | PhxInteractionLayers.VEHICLE | PhxInteractionLayers.ROADWAY | PhxInteractionLayers.TERRAIN | PhxInteractionLayers.CHARACTER | PhxInteractionLayers.AI | PhxInteractionLayers.RAGDOLL | PhxInteractionLayers.RAGDOLL_NO_CHARACTER;
		DayZPhysics.RayCastBullet(from, to, hitMask, player, obj, hitPos, hitNormal, hitFraction);
		
		// something is hit
		if (hitPos != vector.Zero)
		{
			SpawnEntityOnGroundPos(player, itemName, health, quantity, hitPos, special, withPhysics);
		}
	}
	
	/**
	 * @fn		SpawnEntityOnGroundPos
	 * @brief	spawns entity on exact position
	 * @param[in]	health	\p	health of the item
	 * @param[in]	quantity	\p	quantity to set if item.HasQuantity() (-1 == set to max)
	 * @param[in]	pos	\p	position where to spawn
	 * @return	entity if ok, null otherwise
	 **/
	EntityAI SpawnEntityOnGroundPos( PlayerBase player, string item_name, float health, float quantity, vector pos, bool special = false, bool withPhysics = false)
	{
		if ( g_Game.IsServer() )
		{		
			EntityAI entity = SpawnEntityOnGroundPos(player, item_name, pos);
			if (entity)
				SetupSpawnedEntity(player, entity, health, quantity, special);
			else
				OnSpawnErrorReport(item_name);
			
			InventoryItem item;
			if (Class.CastTo(item, entity) && withPhysics)
				item.ThrowPhysically(null, "0 0 0");
			
			return entity;
		}
		else
		{		
			Param6<string, float, float, vector, bool, bool> params = new Param6<string, float, float, vector, bool, bool>(item_name, health, quantity, pos, special, withPhysics);
			player.RPCSingleParam(ERPCs.DEV_RPC_SPAWN_ITEM_ON_GROUND, params, true);
		}
		return NULL;
	}
	/**
	 * @fn		SpawnEntityOnCursorDir
	 * @brief	spawns entity in direction of cursor at specified distance
	 * @param[in]	health	\p	health of the item
	 * @param[in]	quantity	\p	quantity to set if item.HasQuantity() (-1 == set to max)
	 * @param[in]	distance	\p	distance of the item from player
	 * @return	entity if ok, null otherwise
	 **/
	EntityAI SpawnEntityOnCursorDir( PlayerBase player, string item_name, float quantity, float distance, float health = -1, bool special = false, string presetName = "", bool withPhysics = false)
	{

		if ( g_Game.IsServer() )
		{		
			// Client -> Server Spawning: Server Side
			EntityAI entity = player.SpawnEntityOnGroundOnCursorDir(item_name, distance);
			
			if ( entity )
			{
				if ( !entity.IsBuilding() && health < 0 && entity.GetMaxHealth() > 0)//check for default (-1)
				{
					health = entity.GetMaxHealth();
				}
				SetupSpawnedEntity( player,entity, health, quantity, special, presetName );
			}
			else
				OnSpawnErrorReport( item_name );
			
			InventoryItem item;
			if (Class.CastTo(item, entity) && withPhysics)
				item.ThrowPhysically(null, "0 0 0");
			
			return entity;
		}
		else
		{		
			// Client -> Server Spawning: Client Side
			Param7<string, float, float, float, bool, string, bool> params = new Param7<string, float, float, float, bool, string, bool>(item_name, quantity, distance, health, special, presetName, withPhysics);
			player.RPCSingleParam(ERPCs.DEV_RPC_SPAWN_ITEM_ON_CURSOR, params, true);
		}
		return NULL;
	}
	/**
	 * @fn		SpawnEntityInInventory
	 * @brief	tries to spawn item somewhere in inventory
	 *
	 * SpawnEntityInInventory queries future parent about free inventory location of the future item
	 *
	 * @param[in]	health	\p	health of the item
	 * @param[in]	quantity	\p	quantity to set if item.HasQuantity() (-1 == set to max)
	 * @return	entity if ok, null otherwise
	 **/
	EntityAI SpawnEntityInInventory( notnull EntityAI target, string className, float health, float quantity, bool special = false, string presetName = "", FindInventoryLocationType locationType = FindInventoryLocationType.ANY)
	{
		if (target.IsPlayer())
		{
			return SpawnEntityInPlayerInventory(PlayerBase.Cast(target), className, health, quantity, special, presetName, locationType);
		}
		if ( g_Game.IsServer() )
		{
			InventoryLocation il = new InventoryLocation;
			if (target.GetInventory() && target.GetInventory().FindFirstFreeLocationForNewEntity(className, FindInventoryLocationType.ANY, il))
			{
				EntityAI eai = SpawnEntity(className, il, ECE_IN_INVENTORY, RF_DEFAULT);
				if ( eai && eai.IsInherited(ItemBase) )
				{
					if ( health < 0 )//check for default (-1)
					{
						health = eai.GetMaxHealth();
					}
					ItemBase i = ItemBase.Cast( eai );
					SetupSpawnedItem(i, health, quantity);
					if ( special )
					{
						auto debugParams = DebugSpawnParams.WithPlayer(null);
						eai.OnDebugSpawnEx(debugParams);
					}
				}
				return eai;
			}
		}
		else
		{
			DevSpawnItemParams params = new DevSpawnItemParams(target, className, health, quantity, special, presetName, 0 );
				g_Game.GetPlayer().RPCSingleParam(ERPCs.DEV_RPC_SPAWN_ITEM_IN_INVENTORY, params, true, g_Game.GetPlayer().GetIdentity());
		}
		return null;
	}
	
	EntityAI SpawnEntityInPlayerInventory(PlayerBase player, string item_name, float health, float quantity, bool special = false, string presetName = "", FindInventoryLocationType locationType = FindInventoryLocationType.ANY)
	{
		if (player)
		{
			if (g_Game.IsServer())
			{
				if (locationType == FindInventoryLocationType.HANDS && player.GetItemInHands())
				{
					if (!g_Game.IsMultiplayer())
						player.DropItem(player.GetItemInHands());
					else
						player.ServerDropEntity(player.GetItemInHands());

					g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SpawnEntityInPlayerInventory, 500, false, player, item_name, health, quantity, special, presetName, locationType);

					return null;
				}
			
				
				#ifdef DEVELOPER
				if (g_Game.IsKindOf(item_name, "Transport"))
				{
					EntityAI vehicle = SpawnEntityOnGroundPos(player, item_name, 1, quantity, player.GetPosition());
					auto debugParams = DebugSpawnParams.WithPlayer(player);
					vehicle.OnDebugSpawnEx(debugParams);
					
					if (g_Game.IsMultiplayer())
					{
						DayZPlayerSyncJunctures.SendGetInVehicle(player, vehicle);
					}
					else
					{
						player.SetGetInVehicleDebug(vehicle);
					}

					return vehicle;
				}
				#endif

				InventoryLocation il = new InventoryLocation;
				if (player.GetInventory() && player.GetInventory().FindFirstFreeLocationForNewEntity(item_name, locationType, il))
				{
					Weapon_Base wpn = Weapon_Base.Cast(il.GetParent());
					bool is_mag = il.GetSlot() == InventorySlots.MAGAZINE || il.GetSlot() == InventorySlots.MAGAZINE2 || il.GetSlot() == InventorySlots.MAGAZINE3;
					if (wpn && is_mag)
					{
						vector pos = player.GetPosition();
						EntityAI eai_gnd = SpawnEntityOnGroundPos(player, item_name, health, quantity, pos);
						Magazine mag_gnd = Magazine.Cast(eai_gnd);
						if (mag_gnd && player.GetWeaponManager().CanAttachMagazine(wpn, mag_gnd))
						{
							player.GetWeaponManager().AttachMagazine(mag_gnd);
						}
						return eai_gnd;
					}
					else
					{
						EntityAI eai = SpawnEntity(item_name, il, ECE_IN_INVENTORY, RF_DEFAULT);
						if ( eai && eai.IsInherited(ItemBase) )
						{
							if ( health < 0 )//check for default (-1)
							{
								health = eai.GetMaxHealth();
							}
							ItemBase i = ItemBase.Cast( eai );
							SetupSpawnedItem(i, health, quantity);
							if ( special )
							{
								auto debugParams2 = DebugSpawnParams.WithPlayer(player);
								eai.OnDebugSpawnEx(debugParams2);
							}
						}
						return eai;
					}
				}
				else
					OnSpawnErrorReport(item_name);
				return NULL;
			}
			else
			{		
				// Client -> Server Spawning: Client Side
				DevSpawnItemParams params = new DevSpawnItemParams(player, item_name, health, quantity, special, presetName, locationType );
				player.RPCSingleParam(ERPCs.DEV_RPC_SPAWN_ITEM_IN_INVENTORY, params, true);
			}
		}
		return NULL;
	}
	/**
	 * @fn		SpawnEntityAsAttachment
	 * @brief	tries to spawn item as attachment
	 * @param[in]	health	\p	health of the item
	 * @param[in]	quantity	\p	quantity to set if item.HasQuantity() (-1 == set to max)
	 * @return	entity if ok, null otherwise
	 **/
	EntityAI SpawnEntityAsAttachment (PlayerBase player, EntityAI parent, string att_name, float health, float quantity)
	{
		if ( g_Game.IsServer() )
		{
			Weapon_Base wpn = Weapon_Base.Cast(parent);
			if (wpn)
			{
				GameInventory inventory = parent.GetInventory();
				return inventory.CreateInInventory(att_name);
				/*
				vector pos = player.GetPosition();
				EntityAI eai_gnd = SpawnEntityOnGroundPos(player, att_name, health, quantity, pos);
				Magazine mag_gnd = Magazine.Cast(eai_gnd);
				if (mag_gnd && player.GetWeaponManager().CanAttachMagazine(wpn, mag_gnd))
				{
					player.GetWeaponManager().AttachMagazine(mag_gnd);
				}
				return eai_gnd;
				*/
			}
			else
			{
				EntityAI eai = parent.GetInventory().CreateAttachment(att_name);
				if (eai)
				{
					if ( eai.IsInherited(ItemBase) )
					{
						ItemBase i = ItemBase.Cast( eai );
						SetupSpawnedItem(ItemBase.Cast( eai ), health, quantity);
					}
					return eai;
				}
				else
					OnSpawnErrorReport(att_name);
			}
			return NULL;
		}
		else
		{		
			// Client -> Server Spawning: Client Side
			Param3<string, float, float> params = new Param3<string, float, float>(att_name, health, quantity);
			player.RPCSingleParam(ERPCs.DEV_RPC_SPAWN_ITEM_AS_ATTACHMENT, params, true);
		}
		return NULL;
	}
	
	EntityAI SpawnFromClipboard()
	{
		UIScriptedMenu menu_curr = g_Game.GetUIManager().GetMenu();

		if ( menu_curr == NULL )
		{			
			PlayerBase player = PlayerBase.Cast( g_Game.GetPlayer() );

				if ( player && !g_Game.GetWorld().Is3rdPersonDisabled() )
				{
					player.SetIsInThirdPerson(!player.IsInThirdPerson());//this counters the effect of switching camera through pressing the 'V' key
				}

				vector pos_player, pos_direction;
				GetCameraDirections(player, false, pos_player, pos_direction);
					
					// Get item from clipboard
					string		clipboard;
					g_Game.CopyFromClipboard(clipboard);
								
					if (!clipboard.Contains(","))
					{
						//single item
						if (DeveloperFreeCamera.IsFreeCameraEnabled())
							SpawnItemOnCrosshair(player, clipboard.Trim(), -1, 1, 40, true );
						else
							SpawnEntityOnCursorDir(player, clipboard.Trim(), 1, 1);
					}
					else
					{
						TStringArray items = new TStringArray;
						clipboard.Split( ",", items );
						
						foreach (string item:items)
						{
							if (DeveloperFreeCamera.IsFreeCameraEnabled())
								SpawnItemOnCrosshair(player, item.Trim(), -1, 1, 40, true );
							else
								SpawnEntityOnCursorDir(player, item.Trim(), 1, 1);
						}
					}
				
		}
		
		return NULL;
	}
	
	// Clear Entity Inventory
	void ClearInventory(EntityAI entity)
	{
		if ( g_Game.IsServer() )
		{
			entity.ClearInventory();
		}
		else
		{
			Param1<int> params = new Param1<int>(0);
			entity.RPCSingleParam(ERPCs.DEV_RPC_CLEAR_INV, params, true);
		}
	}	
	
	void ToggleHelpScreen()
	{
		if ( g_Game.GetUIManager().GetMenu() == NULL )
		{
			g_Game.GetUIManager().EnterScriptedMenu(MENU_HELP_SCREEN, NULL);
		}
		else if ( g_Game.GetUIManager().IsMenuOpen(MENU_HELP_SCREEN) )
		{
			g_Game.GetUIManager().Back();
		}
	}

	void ToggleScriptConsole()
	{
		if (g_Game != null && !g_Game.IsLoading() && g_Game.GetMission())
		{
			if ( g_Game.GetUIManager().GetMenu() == NULL )
			{
				m_ScriptConsole = g_Game.GetUIManager().EnterScriptedMenu(MENU_SCRIPTCONSOLE, NULL);
			}
			else if ( g_Game.GetUIManager().IsMenuOpen(MENU_SCRIPTCONSOLE) )
			{
				g_Game.GetUIManager().Back();
				g_Game.GetMission().RemoveActiveInputExcludes({"menu"},true);
			}
		}
	}
	
	// Mission Editor
	void ToggleMissionLoader()
	{
		if ( g_Game.GetUIManager().IsMenuOpen(MENU_MISSION_LOADER) )
		{
			g_Game.GetUIManager().Back();
			g_Game.GetMission().RemoveActiveInputExcludes({"menu"},true);
			return;
		}

		if ( g_Game.GetUIManager().GetMenu() )
			g_Game.GetUIManager().GetMenu().Close();
		
		g_Game.GetUIManager().EnterScriptedMenu(MENU_MISSION_LOADER, NULL);
		g_Game.GetMission().AddActiveInputExcludes({"menu"});
		
		
	}
	
	// Script Editor History
	private void ScriptHistoryNext()
	{
		// Console key press
		if ( g_Game.GetUIManager().IsMenuOpen(MENU_SCRIPTCONSOLE) )
		{
			CallMethod(CALL_ID_SCR_CNSL_HISTORY_NEXT, NULL);
		}
	}

	private	void ScriptHistoryBack()
	{
		// Console key press
		if ( g_Game.GetUIManager().IsMenuOpen(MENU_SCRIPTCONSOLE) )
		{
			CallMethod(CALL_ID_SCR_CNSL_HISTORY_BACK, NULL);
		}
	}
	
	private bool IsIngame()
	{
		UIScriptedMenu menu_curr = g_Game.GetUIManager().GetMenu();

		if ( menu_curr == NULL )
		{			
			return true;
		}
		
		return false;
	}

	private bool IsInConsole()
	{
		UIScriptedMenu menu_curr = g_Game.GetUIManager().GetMenu();

		if ( menu_curr != NULL && menu_curr.GetID() == MENU_SCRIPTCONSOLE )
		{			
			return true;
		}
		
		return false;
	}
	
	// Tools
	int QuickSortPartition( TStringArray arr, int left, int right )
	{
		string pivot = arr.Get( left );
		int i = left;
		int j = right + 1;
		string temp;

		while ( true )
		{
			while ( true )
			{ 
				i++; 
				if ( i > right || arr.Get(i) > pivot )
				{
					break;
				}
			}
			
			while ( true )
			{
				j--; 
				if ( arr.Get(j) <= pivot )
				{
					break;
				}
			}
			
			if ( i >= j ) 
			{
				break;
			}
			
			temp = arr.Get( i ); 
			arr.Set( i, arr.Get(j) ); 
			arr.Set( j, temp );
		}
		
		temp = arr.Get( left ); 
		arr.Set( left, arr.Get(j) );
		arr.Set( j, temp );
	   
		return j;
	}

	void QuickSort( TStringArray arr, int left, int right )
	{
		int j;

		if ( left < right ) 
		{
			j = QuickSortPartition( arr, left, right );
			QuickSort( arr, left, j - 1 );
			QuickSort( arr, j + 1, right );
		}
	}

	void SortStringArray( TStringArray arr )
	{
		QuickSort( arr, 0, arr.Count() - 1 );
	}
	
	void ResetGUI()
	{
		if ( g_Game && g_Game.GetMission() )
		{
			g_Game.GetMission().ResetGUI();
		}
	}
	
	static void SetDeveloperItemClientEx(notnull Object entity, bool getFocus = false)
	{
		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
		
		if (getFocus)
		{
			g_Game.GetInput().ChangeGameFocus(1);
			g_Game.GetUIManager().ShowUICursor(true);
		}

		if (g_Game.IsMultiplayer())
		{
			g_Game.RPCSingleParam(g_Game.GetPlayer(), ERPCs.DEV_SET_DEV_ITEM, new Param1<Object>(entity), true, g_Game.GetPlayer().GetIdentity());
		}
		else
		{
			PluginItemDiagnostic mid2 = PluginItemDiagnostic.Cast(GetPlugin(PluginItemDiagnostic));
			mid2.RegisterDebugItem(entity, player);
		}
		#ifdef DEVELOPER
		SetDebugDeveloper_item(entity);
		#endif
	}
	
	void SetDeveloperItemClient()
	{
		#ifdef DEVELOPER
		Object entity;
		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());

		FreeDebugCamera debugCam = FreeDebugCamera.GetInstance();
		if (debugCam && debugCam.GetCurrentCamera())
		{
			entity = debugCam.GetCrosshairObject();
		}
		else
		{
			
			if (player && player.GetActionManager())
			{
				ActionTarget at = player.GetActionManager().FindActionTarget();
				entity = at.GetObject();
			}
		}

		if (entity)
		{
			SetDeveloperItemClientEx(entity, true);
		}
		#endif
	}
}