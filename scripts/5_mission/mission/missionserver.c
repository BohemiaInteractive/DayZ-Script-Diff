//! int  time of the logout end 
//: string  uid of the player
typedef Param2<int, string> LogoutInfo; 

class MissionServer extends MissionBase
{
	ref array<Man> m_Players;
	ref array<ref CorpseData> m_DeadPlayersArray;
	ref map<PlayerBase, ref LogoutInfo> m_LogoutPlayers;
	ref map<PlayerBase, ref LogoutInfo> m_NewLogoutPlayers;
	ref RainProcurementHandler m_RainProcHandler;
	const int SCHEDULER_PLAYERS_PER_TICK = 5;
	int m_currentPlayer;
	int m_RespawnMode;
	
	// -----------------------
	// ARTILLERY SOUNDS SETUP
	// -----------------------
	private float 				m_ArtyBarrageTimer = 0;			// This is not to be edited in Init.c this is just to increment time
	
	// Variables to be modified in Init.c
	protected bool 				m_PlayArty = false; 			// Toggle if Off map artillery sounds are played
	protected float 			m_ArtyDelay = 0; 				// Set how much time there is between two barrages (in seconds)
	protected int 				m_MinSimultaneousStrikes = 0;	// The MIN of simultaneous shots on the map (Will be clamped between 1 and max shots)
	protected int 				m_MaxSimultaneousStrikes = 0;	// The MAX of simultaneous shots on the map (Will be clamped between 1 and max amount of coords)
	protected ref array<vector> m_FiringPos; 					// Where we should fire from. On Init set the relevant data
	
	//All Chernarus firing coordinates 
	protected const ref array<vector> CHERNARUS_STRIKE_POS = 
	{
		"-500.00 165.00 5231.69",
		"-500.00 300.00 9934.41",
		"10406.86 192.00 15860.00",
		"4811.75 370.00 15860.00",
		"-500.00 453.00 15860.00"
	};
	
	//All livonia firing coordinates
	protected const ref  array<vector> LIVONIA_STRIKE_POS = 
	{ 
		"7440.00 417.00 -500.00",
		"-500.00 276.00 5473.00",
		"-500.00 265.00 9852.00",
		"4953.00 240.00 13300.00",
		"9620.00 188.00 13300.00",
		"13300.00 204.00 10322.00",
		"13300.00 288.00 6204.00",
		"13300.00 296.00 -500.00"
	};
	// -----------------------
	// END OF ARTILLERY SETUP
	// -----------------------
	
	PlayerBase m_player;
	MissionBase m_mission;
	
#ifndef NO_GUI
	protected int 					m_ControlDisabledMode;
	protected ref array<string> 	m_ActiveInputExcludeGroups; //exclude groups defined in 'specific.xml' file
	protected ref array<int> 		m_ActiveInputRestrictions; //additional scripted restrictions
	protected bool					m_ProcessInputExcludes;
#endif

	void MissionServer()
	{
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(this.UpdatePlayersStats, 30000, true);

		m_DeadPlayersArray = new array<ref CorpseData>;
		UpdatePlayersStats();
		m_Players = new array<Man>;
				
		m_LogoutPlayers = new map<PlayerBase, ref LogoutInfo>;
		m_NewLogoutPlayers = new map<PlayerBase, ref LogoutInfo>;
		m_RainProcHandler = new RainProcurementHandler(this);
		m_ActiveRefresherLocations 	= new array<vector>();
	}
	
	void ~MissionServer()
	{
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(this.UpdatePlayersStats);
	}
	
	override void OnInit()
	{
		super.OnInit();
		CfgGameplayHandler.LoadData();
		PlayerSpawnHandler.LoadData();
		CfgPlayerRestrictedAreaHandler.LoadData();
		UndergroundAreaLoader.SpawnAllTriggerCarriers();
		//Either pass consts in Init.c or insert all desired coords (or do both ;))
		m_FiringPos = new array<vector>();
	}
	
	override void OnMissionStart()
	{
		super.OnMissionStart();
		
		// We will load the Effect areas on Default mission start
		EffectAreaLoader.CreateZones();
	}
	
	override void OnUpdate(float timeslice)
	{
#ifndef NO_GUI
#ifdef FEATURE_CURSOR
		if (GetTimeStamp() == 0)
		{
			//! Only once the game is loaded should we take control of the cursor
			g_Game.GetUIManager().ShowUICursor(false);
		}
#endif
#endif

		UpdateDummyScheduler();
		TickScheduler(timeslice);
		UpdateLogoutPlayers();		
		m_WorldData.UpdateBaseEnvTemperature(timeslice);	// re-calculate base enviro temperature
		m_RainProcHandler.Update(timeslice);
		
		RandomArtillery(timeslice);
		
		super.OnUpdate(timeslice);

#ifndef NO_GUI
		UIManager uiManager = g_Game.GetUIManager();
		UIScriptedMenu menu = uiManager.GetMenu();
		Input input = g_Game.GetInput();
		
		if (menu && !menu.UseKeyboard() && menu.UseMouse())
		{
			int i;
			for (i = 0; i < 5; i++)
			{
				input.DisableKey(i | INPUT_DEVICE_MOUSE);
				input.DisableKey(i | INPUT_ACTION_TYPE_DOWN_EVENT | INPUT_DEVICE_MOUSE);
				input.DisableKey(i | INPUT_ACTION_TYPE_DOUBLETAP | INPUT_DEVICE_MOUSE);
			}
			
			for (i = 0; i < 6; i++)
			{
				input.DisableKey(i | INPUT_DEVICE_MOUSE_AXIS);
			}
		}

		if (m_ProcessInputExcludes)
		{
			PerformRefreshExcludes();
			m_ProcessInputExcludes = false;
		}
#endif

	}
	
	override void OnGameplayDataHandlerLoad()
	{
		m_RespawnMode = CfgGameplayHandler.GetDisableRespawnDialog();
		g_Game.SetDebugMonitorEnabled(g_Game.ServerConfigGetInt("enableDebugMonitor"));
		
		InitialiseWorldData();
	}
		
	
	void RandomArtillery(float deltaTime)
	{
		// ARTY barrage 
		if (m_PlayArty)
		{
			// We only perform timer checks and increments if we enabled the artillery barrage
			if (m_ArtyBarrageTimer > m_ArtyDelay)
			{
				//We clamp to guarantee 1 and never have multiple shots on same pos, even in case of entry error
				m_MaxSimultaneousStrikes = Math.Clamp(m_MaxSimultaneousStrikes, 1, m_FiringPos.Count());
				m_MinSimultaneousStrikes = Math.Clamp(m_MinSimultaneousStrikes, 1, m_MaxSimultaneousStrikes);
				
				// Variables to be used in this scope
				int randPos; // Select random position
				Param1<vector> pos; // The value to be sent through RPC
				array<ref Param> params; // The RPC params
				
				if (m_MaxSimultaneousStrikes == 1)
				{
					// We only have one set of coordinates to send
					randPos = Math.RandomIntInclusive(0, m_FiringPos.Count() - 1);
					pos = new Param1<vector>(m_FiringPos[randPos]);
					params = new array<ref Param>;
					params.Insert(pos);
					g_Game.RPC(null, ERPCs.RPC_SOUND_ARTILLERY, params, true);
				}
				else
				{
					//We will do some extra steps to
					/*
					1. Send multiple coords (Send one RPC per coord set)
					2. Ensure we don't have duplicates
					*/
					array<int> usedIndices = new array<int>; // Will store all previusly fired upon indices
					
					// We determine how many positions fire between MIN and MAX
					int randFireNb = Math.RandomIntInclusive(m_MinSimultaneousStrikes, m_MaxSimultaneousStrikes);
					for (int i = 0; i < randFireNb; i++)
					{
						randPos = Math.RandomIntInclusive(0, m_FiringPos.Count() - 1);
						
						if (usedIndices.Count() <= 0 || usedIndices.Find(randPos) < 0) //We do not find the index or array is empty
						{
							// We prepare to send the message
							pos = new Param1<vector>(m_FiringPos[randPos]);
							params = new array<ref Param>;
							
							// We send the message with this set of coords
							params.Insert(pos);
							g_Game.RPC(null, ERPCs.RPC_SOUND_ARTILLERY, params, true);
							
							// We store the last used value
							usedIndices.Insert(randPos);
						}
					}
				}
				
				// Reset timer for new loop
				m_ArtyBarrageTimer = 0.0;
			}
			
			m_ArtyBarrageTimer += deltaTime;
		}
	}

	override bool IsServer()
	{
		return true;
	}	
	
	override bool IsPlayerDisconnecting(Man player)
	{
		return (m_LogoutPlayers && m_LogoutPlayers.Contains(PlayerBase.Cast(player))) || (m_NewLogoutPlayers && m_NewLogoutPlayers.Contains(PlayerBase.Cast(player)));
	}
	
	void UpdatePlayersStats()
	{
		PluginLifespan moduleLifespan;
		Class.CastTo(moduleLifespan, GetPlugin(PluginLifespan));
		array<Man> players = new array<Man>();
		g_Game.GetPlayers(players);
			
		foreach (Man man : players)
		{
			PlayerBase player;
			if (Class.CastTo(player, man))
			{
				player.StatUpdateByTime(AnalyticsManagerServer.STAT_PLAYTIME);
				player.StatUpdateByPosition(AnalyticsManagerServer.STAT_DISTANCE);

				moduleLifespan.UpdateLifespan(player);
			}
		}
		
		UpdateCorpseStatesServer();
	}
	
	protected void AddNewPlayerLogout(PlayerBase player, notnull LogoutInfo info)
	{
		m_LogoutPlayers.Insert(player, info);
		m_NewLogoutPlayers.Remove(player);
	}
	
	// check if logout finished for some players
	void UpdateLogoutPlayers()
	{
		for (int i = 0; i < m_LogoutPlayers.Count();)
		{
			LogoutInfo info = m_LogoutPlayers.GetElement(i);
			
			if (g_Game.GetTime() >= info.param1)
			{
				PlayerIdentity identity;
				PlayerBase player = m_LogoutPlayers.GetKey(i);
				if (player)
				{
					identity = player.GetIdentity();
					m_LogoutPlayers.Remove(player);
				}
				else
				{
					m_LogoutPlayers.RemoveElement(i);
				}
				
				// disable reconnecting to old char
				// g_Game.RemoveFromReconnectCache(info.param2);
	
				PlayerDisconnected(player, identity, info.param2);						
			}
			else
			{
				++i;
			}
		}
	}
	
	override void OnEvent(EventType eventTypeId, Param params) 
	{
		PlayerIdentity identity;
		PlayerBase player;
		int counter = 0;
		
		switch (eventTypeId)
		{
		case ClientPrepareEventTypeID:
			ClientPrepareEventParams clientPrepareParams;
			Class.CastTo(clientPrepareParams, params);
			CfgGameplayHandler.SyncDataSendEx(clientPrepareParams.param1);
			UndergroundAreaLoader.SyncDataSend(clientPrepareParams.param1);
			CfgPlayerRestrictedAreaHandler.SyncDataSend(clientPrepareParams.param1);
			OnClientPrepareEvent(clientPrepareParams.param1, clientPrepareParams.param2, clientPrepareParams.param3, clientPrepareParams.param4, clientPrepareParams.param5);
			break;

		case ClientNewEventTypeID:
			ClientNewEventParams newParams;
			Class.CastTo(newParams, params);
			player = OnClientNewEvent(newParams.param1, newParams.param2, newParams.param3);
			if (!player)
			{
				Debug.Log("ClientNewEvent: Player is empty");
				return;
			}
			identity = newParams.param1;
			InvokeOnConnect(player,identity);
			SyncEvents.SendPlayerList();
			
			ControlPersonalLight(player);
			SyncGlobalLighting(player);
			
			break;
			
		case ClientReadyEventTypeID:
			ClientReadyEventParams readyParams;
			Class.CastTo(readyParams, params);
			identity = readyParams.param1;
			Class.CastTo(player, readyParams.param2);
			if (!player)
			{
				Debug.Log("ClientReadyEvent: Player is empty");
				return;
			}
			
			OnClientReadyEvent(identity, player);
			InvokeOnConnect(player, identity);
			// Send list of players at all clients
			SyncEvents.SendPlayerList();
			ControlPersonalLight(player);
			SyncGlobalLighting(player);
			break;
					
		case ClientRespawnEventTypeID:
			ClientRespawnEventParams respawnParams;
			Class.CastTo(respawnParams, params);
			identity = respawnParams.param1;
			Class.CastTo(player, respawnParams.param2);
			if (!player)
			{
				Debug.Log("ClientRespawnEvent: Player is empty");
				return;
			}
			
			OnClientRespawnEvent(identity, player);
			break;
			
		case ClientReconnectEventTypeID:
			ClientReconnectEventParams reconnectParams;
			Class.CastTo(reconnectParams, params);
			
			identity = reconnectParams.param1;
			Class.CastTo(player, reconnectParams.param2);
			if (!player)
			{
				Debug.Log("ClientReconnectEvent: Player is empty");
				return;
			}
			
			OnClientReconnectEvent(identity, player);
			break;
		
		case ClientDisconnectedEventTypeID:
			ClientDisconnectedEventParams discoParams;
			Class.CastTo(discoParams, params);		
			
			identity = discoParams.param1;
			Class.CastTo(player, discoParams.param2);			
			int logoutTime = discoParams.param3;
			bool authFailed = discoParams.param4;

			if (!player)
			{
				Debug.Log("ClientDisconnectenEvent: Player is empty");
				return;
			}
			
			OnClientDisconnectedEvent(identity, player, logoutTime, authFailed);
			break;
			
		case LogoutCancelEventTypeID:
			LogoutCancelEventParams logoutCancelParams;
			
			Class.CastTo(logoutCancelParams, params);				
			Class.CastTo(player, logoutCancelParams.param1);
			identity = player.GetIdentity();
			if (identity)
			{
				// disable reconnecting to old char
				// g_Game.RemoveFromReconnectCache(identity.GetId());
				Print("[Logout]: Player " + identity.GetId() + " cancelled"); 
			}
			else
			{
				Print("[Logout]: Player cancelled"); 
			}
			m_LogoutPlayers.Remove(player);
			m_NewLogoutPlayers.Remove(player);
			break;
		}
	}
	
	void InvokeOnConnect(PlayerBase player, PlayerIdentity identity)
	{
		Debug.Log("InvokeOnConnect:"+this.ToString(),"Connect");
		if (player)
			player.OnConnect();
	}

	void InvokeOnDisconnect(PlayerBase player)
	{
		Debug.Log("InvokeOnDisconnect:"+this.ToString(),"Connect");
		if (player)
			player.OnDisconnect();
	}

	void OnClientPrepareEvent(PlayerIdentity identity, out bool useDB, out vector pos, out float yaw, out int preloadTimeout)
	{
		if (GetHive())
		{
			// use character from database
			useDB = true;
		}
		else
		{
			// use following data without database
			useDB = false;
			pos = "1189.3 0.0 5392.48";
			yaw = 0;
		}
	}
	
	// Enables/Disables personal light on the given player.
	void ControlPersonalLight(PlayerBase player)
	{
		if (player)
		{
			bool is_personal_light = ! g_Game.ServerConfigGetInt("disablePersonalLight");
			Param1<bool> personal_light_toggle = new Param1<bool>(is_personal_light);
			g_Game.RPCSingleParam(player, ERPCs.RPC_TOGGLE_PERSONAL_LIGHT, personal_light_toggle, true, player.GetIdentity());
		}
		else
		{
			Error("Error! Player was not initialized at the right time. Thus cannot send RPC command to enable or disable personal light!");
		}
	}
	
	// syncs global lighting setup from the server (lightingConfig server config parameter) 
	void SyncGlobalLighting(PlayerBase player)
	{
		if (player)
		{
			int lightingID = g_Game.ServerConfigGetInt("lightingConfig");
			Param1<int> lightID = new Param1<int>(lightingID);
			g_Game.RPCSingleParam(player, ERPCs.RPC_SEND_LIGHTING_SETUP, lightID, true, player.GetIdentity());
		}
	}
	
	//! returns whether received data is valid, ctx can be filled on client in StoreLoginData()
	bool ProcessLoginData(ParamsReadContext ctx)
	{
		//creates temporary server-side structure for handling default character spawn
		return g_Game.GetMenuDefaultCharacterData(false).DeserializeCharacterData(ctx);
	}
	
	//
	PlayerBase CreateCharacter(PlayerIdentity identity, vector pos, ParamsReadContext ctx, string characterName)
	{
		Entity playerEnt;
		playerEnt = g_Game.CreatePlayer(identity, characterName, pos, 0, "NONE");//Creates random player
		Class.CastTo(m_player, playerEnt);
		
		g_Game.SelectPlayer(identity, m_player);
	
		return m_player;
	}
	
	//! Spawns character equip from received data. Checks validity against config, randomizes if invalid value and config array not empty.
	void EquipCharacter(MenuDefaultCharacterData char_data)
	{
		int slot_ID;
		string attachment_type;
		for (int i = 0; i < DefaultCharacterCreationMethods.GetAttachmentSlotsArray().Count(); i++)
		{
			slot_ID = DefaultCharacterCreationMethods.GetAttachmentSlotsArray().Get(i);
			attachment_type = "";
			if (m_RespawnMode != GameConstants.RESPAWN_MODE_CUSTOM || !char_data.GetAttachmentMap().Find(slot_ID,attachment_type) || !VerifyAttachmentType(slot_ID,attachment_type)) //todo insert verification fn here
			{
				//randomize
				if (DefaultCharacterCreationMethods.GetConfigArrayCountFromSlotID(slot_ID) > 0)
				{
					attachment_type = DefaultCharacterCreationMethods.GetConfigAttachmentTypes(slot_ID).GetRandomElement();
				}
				else //undefined, moving on
					continue;
			}
			
			if (attachment_type != "")
			{
				m_player.GetInventory().CreateAttachmentEx(attachment_type,slot_ID);
			}
		}
		
		StartingEquipSetup(m_player, true);
	}
	
	//! can be overriden to manually set up starting equip. 'clothesChosen' is legacy parameter, does nothing.
	void StartingEquipSetup(PlayerBase player, bool clothesChosen)
	{
	}
	
	bool VerifyAttachmentType(int slot_ID, string attachment_type)
	{
		return DefaultCharacterCreationMethods.GetConfigAttachmentTypes(slot_ID).Find(attachment_type) > -1;
	}
	
	PlayerBase OnClientNewEvent(PlayerIdentity identity, vector pos, ParamsReadContext ctx)
	{
		string characterType = g_Game.CreateRandomPlayer();
		bool generateRandomEquip = false;
		
		// get login data for new character
		if (ProcessLoginData(ctx) && (m_RespawnMode == GameConstants.RESPAWN_MODE_CUSTOM) && !g_Game.GetMenuDefaultCharacterData(false).IsRandomCharacterForced())
		{
			if (g_Game.ListAvailableCharacters().Find(g_Game.GetMenuDefaultCharacterData().GetCharacterType()) > -1)
				characterType = g_Game.GetMenuDefaultCharacterData().GetCharacterType();
		}
		else
		{
			generateRandomEquip = true;
		}
		
		if (PlayerSpawnHandler.IsInitialized())
		{
			PlayerSpawnPreset presetData = PlayerSpawnHandler.GetRandomCharacterPreset();
			if (presetData && presetData.IsValid())
			{
				string presetCharType = presetData.GetRandomCharacterType();
				if (presetCharType == string.Empty)
					presetCharType = characterType;
				if (CreateCharacter(identity, pos, ctx, presetCharType) != null)
				{
					PlayerSpawnHandler.ProcessEquipmentData(m_player,presetData);
					return m_player;
				}
				else
				{
					ErrorEx("Failed to create character from type: " + presetCharType + ", using default spawning method");
				}
			}
			else
			{
				ErrorEx("Failed to load PlayerSpawnPreset data properly, using default spawning method");
			}
		}
		
		if (CreateCharacter(identity, pos, ctx, characterType))
		{
			if (generateRandomEquip)
				g_Game.GetMenuDefaultCharacterData().GenerateRandomEquip();
			EquipCharacter(g_Game.GetMenuDefaultCharacterData());
		}
		
		return m_player;
	}
	
	void OnClientReadyEvent(PlayerIdentity identity, PlayerBase player)
	{
		g_Game.SelectPlayer(identity, player);
		
		#ifdef DIAG_DEVELOPER
		if (FeatureTimeAccel.m_CurrentTimeAccel)
		{
			g_Game.RPCSingleParam(player, ERPCs.DIAG_TIMEACCEL_CLIENT_SYNC, FeatureTimeAccel.m_CurrentTimeAccel, true, identity);
		}
		#endif
	}	
	
	void OnClientRespawnEvent(PlayerIdentity identity, PlayerBase player)
	{
		if (player)
		{
			if (player.IsUnconscious() || player.IsRestrained())
			{
				PluginAdminLog adm = PluginAdminLog.Cast(GetPlugin(PluginAdminLog));
				adm.PlayerKilledByRespawn(player);
				
				// kill character
				player.SetHealth("", "", 0.0);
			}
		}
		
		#ifdef DIAG_DEVELOPER
		if (FeatureTimeAccel.m_CurrentTimeAccel)
		{
			g_Game.RPCSingleParam(player, ERPCs.DIAG_TIMEACCEL_CLIENT_SYNC, FeatureTimeAccel.m_CurrentTimeAccel, true, identity);
		}
		#endif
	}
	
	void OnClientReconnectEvent(PlayerIdentity identity, PlayerBase player)
	{
		if (player)
		{
			player.OnReconnect();
		}
	}	
	
	void OnClientDisconnectedEvent(PlayerIdentity identity, PlayerBase player, int logoutTime, bool authFailed)
	{
		bool disconnectNow = true;
		
		// TODO: get out of vehicle
		// using database and no saving if authorization failed
		if (GetHive() && !authFailed)
		{			
			if (player.IsAlive())
			{	
				if (!m_LogoutPlayers.Contains(player) && !m_NewLogoutPlayers.Contains(player))
				{
					Print("[Logout]: New player " + identity.GetId() + " with logout time " + logoutTime.ToString());
					
					// send statistics to client
					player.StatSyncToClient();
					
					// inform client about logout time
					g_Game.SendLogoutTime(player, logoutTime);
			
					// wait for some time before logout and save
					LogoutInfo params = new LogoutInfo(g_Game.GetTime() + logoutTime * 1000, identity.GetId());
					
					m_NewLogoutPlayers.Insert(player, params);
					g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AddNewPlayerLogout, 0, false, player, params);
					
					// allow reconnecting to old char only if not in cars, od ladders etc. as they cannot be properly synchronized for reconnect
					//if (!player.GetCommand_Vehicle() && !player.GetCommand_Ladder())
					//{
					//	g_Game.AddToReconnectCache(identity);
					//}
					// wait until logout timer runs out
					disconnectNow = false;
				}
				return;
			}		
		}
		
		if (disconnectNow)
		{
			Print("[Logout]: New player " + identity.GetId() + " with instant logout");
			
			// inform client about instant logout
			g_Game.SendLogoutTime(player, 0);
			
			PlayerDisconnected(player, identity, identity.GetId());
		}
	}

	void PlayerDisconnected(PlayerBase player, PlayerIdentity identity, string uid)
	{
		// Note: At this point, identity can be already deleted
		if (!player)
		{
			Print("[Logout]: Skipping player " + uid + ", already removed");
			return;
		}
		
		// disable reconnecting to old char
		//g_Game.RemoveFromReconnectCache(uid);

		// now player can't cancel logout anymore, so call everything needed upon disconnect
		InvokeOnDisconnect(player);

		Print("[Logout]: Player " + uid + " finished");

		if (GetHive())
		{
			// save player
			player.Save();
			
			// unlock player in DB	
			GetHive().CharacterExit(player);		
		}
		
		// handle player's existing char in the world
		player.ReleaseNetworkControls();		
		HandleBody(player);
		
		// remove player from server
		g_Game.DisconnectPlayer(identity, uid);
		// Send list of players at all clients
		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SyncEvents.SendPlayerList, 1000);
	}
	
	bool ShouldPlayerBeKilled(PlayerBase player)
	{
		if (player.IsUnconscious() || player.IsRestrained())
		{
			switch (player.GetKickOffReason())
			{
				case EClientKicked.SERVER_EXIT:
					return false;
				case EClientKicked.KICK_ALL_ADMIN:
					return false;
				case EClientKicked.KICK_ALL_SERVER:
					return false;
				case EClientKicked.SERVER_SHUTDOWN:
					return false;
				default:
					return true;
			}
		}
		
		return false;
	}
	
	void HandleBody(PlayerBase player)
	{
		if (player.IsAlive())
		{
			if (ShouldPlayerBeKilled(player))
			{
				PluginAdminLog adm = PluginAdminLog.Cast(GetPlugin(PluginAdminLog));
				adm.PlayerKilledByDisconnect(player);
				
				player.SetHealth("", "", 0.0);//kill
			}
			else
			{
				player.Delete();// remove the body
			}
		}
	}
	
	void TickScheduler(float timeslice)
	{
		g_Game.GetWorld().GetPlayerList(m_Players);
		int players_count = m_Players.Count();
		int tick_count_max = Math.Min(players_count, SCHEDULER_PLAYERS_PER_TICK);
		
		for (int i = 0; i < tick_count_max; ++i)
		{
			if (m_currentPlayer >= players_count)
			{
				m_currentPlayer = 0;
			}
			
			PlayerBase currentPlayer = PlayerBase.Cast(m_Players.Get(m_currentPlayer));
			if (currentPlayer)
				currentPlayer.OnTick();
			++m_currentPlayer;
		}
	}
	
	//--------------------------------------------------
	override bool InsertCorpse(Man player)
	{
		CorpseData corpse_data = new CorpseData(PlayerBase.Cast(player),g_Game.GetTime());
		return m_DeadPlayersArray.Insert(corpse_data) >= 0;
	}
	
	void UpdateCorpseStatesServer()
	{
		int nDeadPlayers = m_DeadPlayersArray.Count();
		if (nDeadPlayers == 0)//nothing to process, abort
			return;
		int current_time = g_Game.GetTime();
		array<int> invalid_corpses = new array<int>;
		CorpseData corpse_data;
		
		for (int i = 0; i < nDeadPlayers; ++i)
		{
			corpse_data = m_DeadPlayersArray.Get(i);
			if (!corpse_data || (corpse_data && (!corpse_data.m_Player || !corpse_data.m_bUpdate)))
			{
				invalid_corpses.Insert(i);
			}
			else if (corpse_data.m_bUpdate && current_time - corpse_data.m_iLastUpdateTime >= 30000)
			{
				corpse_data.UpdateCorpseState();
				corpse_data.m_iLastUpdateTime = current_time;
			}
		}
		
		//cleanup
		int nInvalidCorpses = invalid_corpses.Count();
		if (nInvalidCorpses > 0)
		{
			for (i = nInvalidCorpses - 1; i >= 0; --i)
			{
				m_DeadPlayersArray.Remove(invalid_corpses.Get(i));
			}
		}
	}
	//--------------------------------------------------
	
	override void SyncRespawnModeInfo(PlayerIdentity identity)
	{
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(m_RespawnMode);
		rpc.Send(null, ERPCs.RPC_SERVER_RESPAWN_MODE, true, identity);
	}
	
	override RainProcurementHandler GetRainProcurementHandler()
	{
		return m_RainProcHandler;
	}
	
	override array<vector> GetActiveRefresherLocations()
	{
		return m_ActiveRefresherLocations;
	}

#ifndef NO_GUI
	//! Removes one or more exclude groups and refreshes excludes
	override void RemoveActiveInputExcludes(array<string> excludes, bool bForceSupress = false)
	{
		super.RemoveActiveInputExcludes(excludes,bForceSupress);
		
		if (excludes.Count() != 0)
		{
			bool changed = false;
			
			if (m_ActiveInputExcludeGroups)
			{
				foreach (string excl : excludes)
				{
					if (m_ActiveInputExcludeGroups.Find(excl) != -1)
					{
						m_ActiveInputExcludeGroups.RemoveItem(excl);
						changed = true;
					}
				}
				
				if (changed)
				{
					RefreshExcludes();
				}
			}
			
			// supress control for next frame
			GetUApi().SupressNextFrame(bForceSupress);
		}
	}
	
	//! Removes one restriction (specific behaviour oudside regular excludes, defined below)
	override void RemoveActiveInputRestriction(int restrictor)
	{
		//unique behaviour outside regular excludes
		if (restrictor > -1)
		{
			switch (restrictor)
			{
				case EInputRestrictors.INVENTORY:
				{
					GetUApi().GetInputByID(UAWalkRunForced).ForceEnable(false); // force walk off!
					break;
				}
				case EInputRestrictors.MAP:
				{
					GetUApi().GetInputByID(UAWalkRunForced).ForceEnable(false); // force walk off!
					break;
				}
			}
			
			if (m_ActiveInputRestrictions && m_ActiveInputRestrictions.Find(restrictor) != -1)
			{
				m_ActiveInputRestrictions.RemoveItem(restrictor);
			}
		}
	}
	
	//! Adds one or more exclude groups to disable and refreshes excludes
	override void AddActiveInputExcludes(array<string> excludes)
	{
		super.AddActiveInputExcludes(excludes);
		
		if (excludes.Count() != 0)
		{
			bool changed = false;
			if (!m_ActiveInputExcludeGroups)
			{
				m_ActiveInputExcludeGroups = new array<string>;
			}
			
			foreach (string excl : excludes)
			{
				if (m_ActiveInputExcludeGroups.Find(excl) == -1)
				{
					m_ActiveInputExcludeGroups.Insert(excl);
					changed = true;
				}
			}
			
			if (changed)
			{
				RefreshExcludes();
				#ifdef BULDOZER
					GetUApi().SupressNextFrame(true);
				#endif
			}
		}
	}
	
	//! Adds one input restriction (specific behaviour oudside regular excludes, defined below)
	override void AddActiveInputRestriction(int restrictor)
	{
		//unique behaviour outside regular excludes
		if (restrictor > -1)
		{
			switch (restrictor)
			{
				case EInputRestrictors.INVENTORY:
				{
					GetUApi().GetInputByID(UAWalkRunForced).ForceEnable(true); // force walk on!
					PlayerBase player = PlayerBase.Cast( g_Game.GetPlayer() );
					if ( player )
					{
						ItemBase item = player.GetItemInHands();
						if (item && item.IsWeapon())
							player.RequestResetADSSync();
					}
					break;
				}
				case EInputRestrictors.MAP:
				{
					GetUApi().GetInputByID(UAWalkRunForced).ForceEnable(true); // force walk on!
					break;
				}
			}
			
			if (!m_ActiveInputRestrictions)
			{
				m_ActiveInputRestrictions = new array<int>;
			}
			if (m_ActiveInputRestrictions.Find(restrictor) == -1)
			{
				m_ActiveInputRestrictions.Insert(restrictor);
			}
		}
	}
	
	//! queues refresh of input excludes
	override void RefreshExcludes()
	{
		m_ProcessInputExcludes = true;
	}
	
	//! applies queued excludes (0 == clear excludes)
	protected void PerformRefreshExcludes()
	{
		if (m_ActiveInputExcludeGroups)
		{
			foreach (string excl : m_ActiveInputExcludeGroups)
			{
				GetUApi().ActivateExclude(excl);
			}
		}
		
		GetUApi().UpdateControls();
	}
	
	//! Removes all active input excludes and restrictions
	override void EnableAllInputs(bool bForceSupress = false)
	{
		m_ControlDisabledMode = -1;
		
		if (m_ActiveInputRestrictions)
		{
			int count = m_ActiveInputRestrictions.Count();
			for (int i = 0; i < count; i++)
			{
				RemoveActiveInputRestriction(m_ActiveInputRestrictions[0]);
			}
			m_ActiveInputRestrictions.Clear(); //redundant?
		}
		if (m_ActiveInputExcludeGroups)
		{
			m_ActiveInputExcludeGroups.Clear();
		}
		
		GetUApi().UpdateControls(); //it is meant to happen instantly, does not wait for update to process
		GetUApi().SupressNextFrame(bForceSupress); // supress control for next frame
	}
	
	//! returns if ANY exclude groups, restriction (or deprecated disable, if applicable) is active
	override bool IsControlDisabled()
	{
		bool active = false;
		if (m_ActiveInputExcludeGroups)
		{
			active |= m_ActiveInputExcludeGroups.Count() > 0;
		}
		if (m_ActiveInputRestrictions)
		{
			active |= m_ActiveInputRestrictions.Count() > 0;
		}
		active |= m_ControlDisabledMode >= INPUT_EXCLUDE_ALL; //legacy stuff, Justin case
		return active;
	}
	
	//! Returns true if the particular input exclude group had been activated via script and is active
	override bool IsInputExcludeActive(string exclude)
	{
		return m_ActiveInputExcludeGroups && m_ActiveInputExcludeGroups.Find(exclude) != -1;
	}
	
	//! Returns true if the particular 'restriction' (those govern special behaviour outside regular input excludes, *EInputRestrictors*) is active
	override bool IsInputRestrictionActive(int restriction)
	{
		return m_ActiveInputRestrictions && m_ActiveInputRestrictions.Find(restriction) != -1;
	}
#endif
	
	//! DEPRECATED
	PluginAdditionalInfo m_moduleDefaultCharacter;
}

