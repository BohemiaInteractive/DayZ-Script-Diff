class PluginDeveloper extends PluginBase
{
	protected bool									m_IsWinHolding;	
	protected int									m_FeaturesMask;
	UIScriptedMenu									m_ScriptConsole;
	
	#ifdef DEVELOPER
	ref PresetSpawnBase 								m_PresetObj;
	#endif
	
	
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
	
	//! Enable / Disable Free camera (Fly mod)
	void ToggleFreeCameraBackPos()
	{
		DeveloperFreeCamera.FreeCameraToggle( PlayerBase.Cast( GetGame().GetPlayer() ), false );
	}
	
	//! Enable / Disable Free camera (Fly mod) - disable of camera will teleport player at current free camera position.
	void ToggleFreeCamera()
	{
		DeveloperFreeCamera.FreeCameraToggle( PlayerBase.Cast( GetGame().GetPlayer() ), true );
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
			{
				OnRPCSpawnEntityOnGround(player, ctx); break;
			}
			case ERPCs.DEV_RPC_SPAWN_ITEM_ON_GROUND_PATTERN_GRID:
			{
				OnRPCSpawnEntityOnGroundPatternGrid(player, ctx); break;
			}
			case ERPCs.DEV_RPC_SPAWN_ITEM_ON_CURSOR:
			{
				OnRPCSpawnEntityOnCursorDir(player, ctx); break;
			}
			case ERPCs.DEV_RPC_SPAWN_ITEM_IN_INVENTORY:
			{
				OnRPCSpawnEntity(player, ctx); break;
			}
			case ERPCs.DEV_RPC_CLEAR_INV:
			{
				OnRPCClearInventory(player); break;
			}
			case ERPCs.DEV_RPC_SEND_SERVER_LOG:
			{
				OnRPCServerLogRecieved( ctx ); break;
			}
			case ERPCs.RPC_SYNC_SCENE_OBJECT:
			{
				OnRPCSyncSceneObject( ctx ); break;
			}
			case ERPCs.DEV_RPC_PLUGIN_DZCREATURE_DEBUG:
			{
				OnRPCPluginDayzCreatureDebug(player, rpc_type, ctx); break;
			}
			case ERPCs.DEV_RPC_SPAWN_PRESET:
			{
				HandlePresetSpawn(player,ctx); break;
			}			
			case ERPCs.DEV_RPC_SET_TIME:
			{
				HandleSetTime(player,ctx); break;
			}
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
		if ( GetGame() )
		{
			array<Man> players = new array<Man>;
			GetGame().GetPlayers( players );
			
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
			
			GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallByName( scene_object, par.param1, par.param3 );
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

		if ( ctx.Read( p5 ) )
		{	
			//float time01 = p5.param4 / 100;
			//int time_minutes = Math.Lerp(0, 24*60, time01 );
			
			int year = p5.param1;
			int month = p5.param2;
			int day = p5.param3;
			int hour = p5.param4;
			int minute = p5.param5;
			
			
	
			GetGame().GetWorld().SetDate(year,month, day, hour, minute);
			/*
			Debug.Log("year:" + year);
			Debug.Log("month:" + month);
			Debug.Log("day:" + day);
			Debug.Log("hour:" + hour);
			Debug.Log("minute:" + minute);
			*/
		}
		
		#endif
	}
		
	void HandlePresetSpawn(PlayerBase player, ParamsReadContext ctx)
	{
		#ifdef DEVELOPER
		if ( ctx.Read(CachedObjectsParams.PARAM1_STRING ) )
		{	
			player.m_PresetSpawned = true;
			string preset_name = CachedObjectsParams.PARAM1_STRING.param1;
			string class_name = preset_name + "_Preset";
			typename type = class_name.ToType();
			
			if (type)
			{
				m_PresetObj = PresetSpawnBase.Cast(type.Spawn());
			}
			
			if (m_PresetObj)
			{
				m_PresetObj.Init(player);
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
		Param6<string, float, float, float, bool, string> p = new Param6<string, float, float, float, bool, string>("", 0, 0, 0, false, "");
		if ( ctx.Read(p) )
		{
			SpawnEntityOnCursorDir(player, p.param1, p.param2, p.param3, p.param4, p.param5, p.param6);
		}
	}
	
	void OnRPCSpawnEntityOnGround(PlayerBase player, ParamsReadContext ctx)
	{
		Param4<string, float, float, vector> p = new Param4<string, float, float, vector>("", 0, 0, "0 0 0");
		if ( ctx.Read(p) )
		{
			SpawnEntityOnGroundPos(player, p.param1, p.param2,  p.param3,  p.param4);
		}
	}
	
	void OnRPCSpawnEntityOnGroundPatternGrid(PlayerBase player, ParamsReadContext ctx)
	{
		auto p = new Param8<string,int, float, float, int, int, float, float>("",0,0,0,0,0,0,0);
		if ( ctx.Read(p) )
		{
			SpawnEntityOnGroundPatternGrid(player, p.param1, p.param2, p.param3, p.param4, p.param5, p.param6, p.param7, p.param8);
		}
	}
	void OnRPCSpawnEntity(PlayerBase player, ParamsReadContext ctx)
	{
		#ifdef DEVELOPER
		Param5<string, float, float,bool, string> p = new Param5<string, float, float, bool, string>("", 0, 0, false, "");
		if ( ctx.Read(p) )
		{
			EntityAI ent = SpawnEntityInInventory(player, p.param1, p.param2,  p.param3, p.param4);
			if (p.param5)
			{
				if (player.m_PresetSpawned)
				{
					player.m_PresetSpawned = false;
					player.m_PresetItems.Clear();
				}
				if (ent)
				{
					player.m_PresetItems.Insert(ent);
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
		if (presetName && player.m_PresetSpawned)//this is how we ascertain this is the first item being spawned from a new preset after a previous preset was already spawned in
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
		if (presetName)
		{
			player.m_PresetItems.Insert(entity);
		}
		if ( special )
			entity.OnDebugSpawn();
		#endif
	}
	
	
	void SpawnEntityOnGroundPatternGrid( PlayerBase player, string item_name, int count, float health, float quantity, int rows, int columns, float gapRow = 1, float gapColumn = 1)
	{
		if (!item_name)
		{
			return;
		}
		if ( GetGame().IsServer() )
		{	
			float rowDist = 0;	
			float columnDist = 0;
			
			vector playerPos = player.GetPosition();
			vector camDirForward = player.GetDirection();
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
					SpawnEntityOnGroundPos(player, item_name, hlth, quantity, placement );
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
			auto params = new Param8<string, int, float, float, int, int, float, float>(item_name, count, health, quantity, rows, columns, gapRow, gapColumn);
			player.RPCSingleParam(ERPCs.DEV_RPC_SPAWN_ITEM_ON_GROUND_PATTERN_GRID, params, true);
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
	EntityAI SpawnEntityOnGroundPos( PlayerBase player, string item_name, float health, float quantity, vector pos)
	{
		if ( GetGame().IsServer() )
		{		
			EntityAI entity = player.SpawnEntityOnGroundPos(item_name, pos);
			if (entity)
				SetupSpawnedEntity(player, entity, health, quantity);
			else
				OnSpawnErrorReport(item_name);
			return entity;
		}
		else
		{		
			Param4<string, float, float, vector> params = new Param4<string, float, float, vector>(item_name, health, quantity, pos);
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
	EntityAI SpawnEntityOnCursorDir( PlayerBase player, string item_name, float quantity, float distance, float health = -1, bool special = false, string presetName = "")
	{

		if ( GetGame().IsServer() )
		{		
			// Client -> Server Spawning: Server Side
			EntityAI entity = player.SpawnEntityOnGroundOnCursorDir(item_name, distance);
			
			if ( entity )
			{
				if ( health < 0 && entity.GetMaxHealth() > 0)//check for default (-1)
				{
					health = entity.GetMaxHealth();
				}
				SetupSpawnedEntity( player,entity, health, quantity, special, presetName );
			}
			else
				OnSpawnErrorReport( item_name );
			return entity;
		}
		else
		{		
			// Client -> Server Spawning: Client Side
			Param6<string, float, float, float, bool, string> params = new Param6<string, float, float, float, bool, string>(item_name, quantity, distance, health, special, presetName);
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
	EntityAI SpawnEntityInInventory( PlayerBase player, string item_name, float health, float quantity, bool special = false, string presetName = "")
	{
		if ( player )
		{
			if ( GetGame().IsServer() )
			{		
				InventoryLocation il = new InventoryLocation;
				if (player.GetInventory() && player.GetInventory().FindFirstFreeLocationForNewEntity(item_name, FindInventoryLocationType.ANY, il))
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
						EntityAI eai = GetGame().SpawnEntity(item_name, il, ECE_IN_INVENTORY, RF_DEFAULT);
						if ( eai && eai.IsInherited(ItemBase) )
						{
							if ( health < 0 )//check for default (-1)
							{
								health = eai.GetMaxHealth();
							}
							ItemBase i = ItemBase.Cast( eai );
							SetupSpawnedItem(i, health, quantity);
							if ( special )
								eai.OnDebugSpawn();
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
				Param5<string, float, float, bool, string> params = new Param5<string, float, float, bool, string>(item_name, health, quantity, special, presetName );
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
		if ( GetGame().IsServer() )
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
		UIScriptedMenu menu_curr = GetGame().GetUIManager().GetMenu();

		if ( menu_curr == NULL )
		{			
			PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
			if ( player )
			{
				if ( !GetGame().GetWorld().Is3rdPersonDisabled() )
				{
					player.SetIsInThirdPerson(!player.IsInThirdPerson());//this counters the effect of switching camera through pressing the 'V' key
				}
				
				vector pos_player = player.GetPosition();
				
				// Get item from clipboard
				string		clipboard;
				GetGame().CopyFromClipboard(clipboard);
							
				if (!clipboard.Contains(","))
				{
					//single item
					EntityAI object_spawned_from_clipboard = SpawnEntityOnCursorDir(player, clipboard, -1, 1);
					//Print(object_spawned_from_clipboard);	
				}
				else
				{
					TStringArray items = new TStringArray;
					clipboard.Split( ",", items );
					
					foreach(string item:items)
					{
						SpawnEntityOnCursorDir(player, item.Trim(), -1, 1);
					}
				}
			}
		}
		
		return NULL;
	}
	
	// Clear Player Inventory
	void ClearInventory(PlayerBase player)
	{
		if ( GetGame().IsServer() )
		{
			player.ClearInventory();
		}
		else
		{
			Param1<int> params = new Param1<int>(0);
			player.RPCSingleParam(ERPCs.DEV_RPC_CLEAR_INV, params, true);
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
		if ( GetGame() != null && GetGame().GetPlayer() != null )
		{
			if ( g_Game.GetUIManager().GetMenu() == NULL )
			{
				m_ScriptConsole = g_Game.GetUIManager().EnterScriptedMenu(MENU_SCRIPTCONSOLE, NULL);
				GetGame().GetMission().AddActiveInputExcludes({"menu"});
			}
			else if ( g_Game.GetUIManager().IsMenuOpen(MENU_SCRIPTCONSOLE) )
			{
				g_Game.GetUIManager().Back();
				GetGame().GetMission().RemoveActiveInputExcludes({"menu"},true);
			}
		}
	}
	
	// Mission Editor
	void ToggleMissionLoader()
	{
		g_Game.GetUIManager().OpenWindow( GUI_WINDOW_MISSION_LOADER );
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
		UIScriptedMenu menu_curr = GetGame().GetUIManager().GetMenu();

		if ( menu_curr == NULL )
		{			
			return true;
		}
		
		return false;
	}

	private bool IsInConsole()
	{
		UIScriptedMenu menu_curr = GetGame().GetUIManager().GetMenu();

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
		if ( GetGame() && GetGame().GetMission() )
		{
			GetGame().GetMission().ResetGUI();
		}
	}
	
	static void SetDeveloperItemClientEx(notnull Object entity, bool getFocus = false)
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		
		if (getFocus)
		{
			GetGame().GetInput().ChangeGameFocus(1);
			GetGame().GetUIManager().ShowUICursor(true);
		}

		if (GetGame().IsMultiplayer())
		{
			GetGame().RPCSingleParam(GetGame().GetPlayer(), ERPCs.DEV_SET_DEV_ITEM, new Param1<Object>(entity), true, GetGame().GetPlayer().GetIdentity());
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
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());

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