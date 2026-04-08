enum PDS_SYSTEMS
{
	STATS 				= 1,
	LEVELS 				= 2,
	MODS 				= 4,
	AGENTS 				= 8,
	STOMACH 			= 16,
	MODS_DETAILED 		= 32,
	TEMPERATURE 		= 64,
	HEALTH 				= 128,
}


class RegisterPlayerData
{
	int m_Bitmask;
	int m_DetailedModifierIndex;
	
	void RegisterPlayerData()
	{
		m_Bitmask = 0;
		m_DetailedModifierIndex = 0;
	}
}

class PluginDeveloperSync extends PluginBase
{
	int m_DetailedInfoRequested = 0;
	ref Timer m_UpdateTimer;
	ref map<PlayerBase, ref RegisterPlayerData> m_RegisteredPlayers;
	
	ref array<ref SyncedValue> m_PlayerStatsSynced;
	ref array<ref Param> m_PlayerStomachSynced;
	ref array<ref SyncedValueLevel> m_PlayerLevelsSynced;
	ref array<ref SyncedValueModifier> m_PlayerModsSynced;
	string 	m_PlayerModsDetailedSynced;
	string 	m_EnvironmentDebugMessage;
	ref array<ref SyncedValueAgent> m_PlayerAgentsSynced;
	protected ref array<ref SyncedValue> m_TargetHealthSynced;
	
	protected bool m_IsTargetChanged;										// client update
	protected ref map<PlayerBase, bool> 		m_HealthObserverMode;		// server tracking of player X health mode selection | true = target / false = self
	protected ref map<PlayerBase, EntityAI> 	m_HealthObserverEntity;		// server tracking of player X health target 
	
	bool m_StatsUpdateStatus;
	bool m_LevelsUpdateStatus;
	bool m_ModsUpdateStatus;
	bool m_AgentsUpdateStatus;
	bool m_StomachUpdateStatus;
	
	void PluginDeveloperSync()
	{
		m_RegisteredPlayers 	= new map<PlayerBase, ref RegisterPlayerData>;
		m_PlayerStatsSynced 	= new array<ref SyncedValue>;
		m_PlayerLevelsSynced 	= new array<ref SyncedValueLevel>;
		m_PlayerModsSynced 		= new array<ref SyncedValueModifier>;
		m_PlayerAgentsSynced 	= new array<ref SyncedValueAgent>;
		m_PlayerStomachSynced 	= new array<ref Param>;
		m_TargetHealthSynced	= new array<ref SyncedValue>;
		m_HealthObserverMode	= new map<PlayerBase,bool>;
		m_HealthObserverEntity	= new map<PlayerBase,EntityAI>;
		
		m_StatsUpdateStatus 	= false;
		m_LevelsUpdateStatus 	= false;
		m_ModsUpdateStatus 		= false;
		m_AgentsUpdateStatus 	= false;
		m_StomachUpdateStatus 	= false;
	}
	
	//================================================================
	// Init / Update
	//================================================================
	override void OnInit()
	{
		super.OnInit();
		
		if ( g_Game.IsServer() )
		{
			//create timer
			m_UpdateTimer = new Timer( CALL_CATEGORY_GAMEPLAY );
		}
	}

	
	void EnableUpdate( bool enable, int type, PlayerBase player )
	{
		//set update by type
		if ( !IsPlayerRegistered(player))
			RegisterPlayer( player );

		
		switch ( type )
		{
			case ERPCs.DEV_STATS_UPDATE: 
			{
				SetSystemInBitmask(player, PDS_SYSTEMS.STATS, enable);
				break;	
			}
			
			case ERPCs.DEV_LEVELS_UPDATE: 
			{
				SetSystemInBitmask(player, PDS_SYSTEMS.LEVELS, enable);
				break;	
			}
			
			case ERPCs.DEV_MODS_UPDATE: 
			{
				SetSystemInBitmask(player, PDS_SYSTEMS.MODS, enable);
				break;	
			}
			
			case ERPCs.DEV_AGENTS_UPDATE: 
			{
				SetSystemInBitmask(player, PDS_SYSTEMS.AGENTS, enable);
				break;	
			}	
			
			case ERPCs.DEV_STOMACH_UPDATE: 
			{
				SetSystemInBitmask(player, PDS_SYSTEMS.STOMACH, enable);
				break;	
			}
			case ERPCs.DEV_TEMP_UPDATE: 
			{
				SetSystemInBitmask(player, PDS_SYSTEMS.TEMPERATURE, enable);
				break;	
			}
			case ERPCs.DEV_HEALTH_UPDATE: 
			{
				SetSystemInBitmask(player, PDS_SYSTEMS.HEALTH, enable);
				break;	
			}
		}	
		
		//remove players with empty mask
		for (int i = 0; i < m_RegisteredPlayers.Count(); i++)
		{
			if (m_RegisteredPlayers.GetElement(i).m_Bitmask == 0)
			{
				m_RegisteredPlayers.RemoveElement(i);
				i = 0;
			}
		}
		
		//start/stop the tick based on existence/absence of players registered
		if ( m_UpdateTimer )
		{
			if ( m_RegisteredPlayers.Count() == 0)
			{
				m_UpdateTimer.Stop();
			}
			else if (!m_UpdateTimer.IsRunning())
			{
				m_UpdateTimer.Run( 1, this, "Update", NULL, true );
			}
		}
	}
	

	void Update()
	{
		if ( !GetDayZGame().IsMultiplayer() || GetDayZGame().IsServer() )
		{
			for ( int i = 0; i < m_RegisteredPlayers.Count(); ++i )
			{
				PlayerBase player = m_RegisteredPlayers.GetKey( i );
				if ( !player || !player.IsAlive() )
				{
					m_RegisteredPlayers.RemoveElement(i);
					i--;
				}
				else
				{
					int bit_mask = m_RegisteredPlayers.Get( player ).m_Bitmask;
					
					if ((PDS_SYSTEMS.MODS & bit_mask) != 0 )
					{
						SendRPCMods( player);
						if ( PDS_SYSTEMS.MODS_DETAILED & bit_mask )
							SendRPCModsDetail(	player); //!!!!! Highly suspect
					}
					if ((PDS_SYSTEMS.LEVELS & bit_mask) != 0 )
					{
						SendRPCLevels( player );		
					}
					
					if ((PDS_SYSTEMS.STATS & bit_mask) != 0 )
					{
						SendRPCStats( player );
					}	
					if ((PDS_SYSTEMS.STOMACH & bit_mask) != 0 )
					{
						SendRPCStomach( player );
					}				
					if ((PDS_SYSTEMS.AGENTS & bit_mask) != 0 )
					{
						SendRPCAgents( player );
					}		
					if ((PDS_SYSTEMS.TEMPERATURE & bit_mask) )
					{
						SendRPCTemp( player );
					}
					if ((PDS_SYSTEMS.HEALTH & bit_mask) )
					{
						SendRPCHealth( player );
					}
				}
			}
		}	
	}
	
	//turns on/off a bit for a given system marking it active/inactive
	void SetSystemInBitmask( PlayerBase player, int system_bit, bool state)
	{
		if ( IsPlayerRegistered(player) )
		{
			int current_mask = m_RegisteredPlayers.Get(player).m_Bitmask;
			if (state)
				m_RegisteredPlayers.Get(player).m_Bitmask = current_mask | system_bit;
			else
				m_RegisteredPlayers.Get(player).m_Bitmask = current_mask & ~system_bit;
		}
			
	}
	
	//turns on/off a bit for a given system marking it active/inactive
	bool IsSetSystemInBitmask( PlayerBase player, int system_bit)
	{
		if ( IsPlayerRegistered(player) )
		{
			return (m_RegisteredPlayers.Get(player).m_Bitmask & system_bit);
		}
		return false;
	}
	
	
	
	
	//--- Register / Unregister player
	void RegisterPlayer( PlayerBase player )
	{
		if ( !IsPlayerRegistered( player ) ) 
		{
			m_RegisteredPlayers.Insert( player, new RegisterPlayerData );
		}
	}

	/*
	void UnregisterPlayer( PlayerBase player )
	{
		if ( IsPlayerRegistered( player ) ) 
		{
			m_RegisteredPlayers.Remove( player );
		}
	}
*/
	bool IsPlayerRegistered( PlayerBase player )
	{
		if ( m_RegisteredPlayers.Contains( player ))
		{
			return true;
		}
		
		return false;
	}
	
	//================================================================
	// RPC
	//================================================================ 
	void OnRPC( PlayerBase player, int rpc_type, ParamsReadContext ctx )
	{
		if ( !g_Game.IsDebug() )
		{
			return;
		}
		
		switch ( rpc_type )
		{
			case ERPCs.DEV_STATS_UPDATE:
			{
				EnableUpdate( GetRPCUpdateState( ctx ), ERPCs.DEV_STATS_UPDATE, player ); break;
			}
			
			case ERPCs.DEV_LEVELS_UPDATE:
			{
				EnableUpdate( GetRPCUpdateState( ctx ), ERPCs.DEV_LEVELS_UPDATE, player ); break;
			}
			
			case ERPCs.DEV_MODS_UPDATE:
			{
				EnableUpdate( GetRPCUpdateState( ctx ), ERPCs.DEV_MODS_UPDATE, player ); break;
			}
			
			case ERPCs.DEV_AGENTS_UPDATE:
			{
				EnableUpdate( GetRPCUpdateState( ctx ), ERPCs.DEV_AGENTS_UPDATE, player ); break;
			}
			
			case ERPCs.DEV_STOMACH_UPDATE:
			{
				EnableUpdate( GetRPCUpdateState( ctx ), ERPCs.DEV_STOMACH_UPDATE, player ); break;
			}
			
			case ERPCs.DEV_HEALTH_UPDATE:
			{
				EnableUpdate( GetRPCUpdateState( ctx ), ERPCs.DEV_HEALTH_UPDATE, player ); break;
			}
			
			case ERPCs.DEV_RPC_STATS_DATA:
			{
				OnRPCStats( ctx ); break;
			}
			
			case ERPCs.DEV_RPC_LEVELS_DATA:
			{
				OnRPCLevels( ctx ); break;
			}

			case ERPCs.DEV_RPC_MODS_DATA:
			{
				OnRPCMods( ctx ); break;
			}
			
			case ERPCs.DEV_RPC_MODS_DATA_DETAILED:
			{
				OnRPCModsDetailed( ctx ); break;
			}
			
			case ERPCs.DEV_RPC_AGENTS_DATA:
			{
				OnRPCAgents( ctx ); break;
			}		
			case ERPCs.DEV_RPC_STOMACH_DATA:
			{
				OnRPCStomach( ctx ); break;
			}
			
			case ERPCs.DEV_RPC_MODS_ACTIVATE:
			{
				ActivateModifier( GetRPCModifierID( ctx ), player ); break;
			}
			case ERPCs.DEV_RPC_MODS_DETAILED:
			{
				RequestDetailedInfo( GetRPCModifierID( ctx ), player ); break;
			}
			
			case ERPCs.DEV_RPC_MODS_DEACTIVATE:
			{
				DeactivateModifier( GetRPCModifierID( ctx ), player ); break;
			}
			
			case ERPCs.DEV_RPC_MODS_LOCK:
			{
				int id;
				bool lock;
				GetRPCModifierLock( ctx, id, lock );
				LockModifier( id, lock, player ); break;
			}
			
			case ERPCs.DEV_RPC_MODS_RESET:
			{
				player.GetModifiersManager().DeactivateAllModifiers();
				break;
			}	
			
			case ERPCs.DEV_RPC_STATS_RESET:
			{
				player.GetPlayerStats().ResetAllStats();
				break;
			}
			
			case ERPCs.DEV_RPC_STAT_SET:
			{
				OnRPCStatSet(ctx, player);
				break;
			}
			case ERPCs.DEV_TEMP_UPDATE:
			{
				OnRPCTemp(ctx, player);
				break;
			}
			case ERPCs.DEV_RPC_HEALTH_DATA:
			{
				OnRPCHealth(ctx, player); break;
			}
			
			case ERPCs.DEV_RPC_HEALTH_SET:
			{
				OnRPCHealthSet(ctx, player);
				break;
			}
			
			case ERPCs.DEV_RPC_HORTICULTURE_UPDATE:
			{
				OnRPCHorticultUpdate(ctx, player);
				break;
			}
			
			case ERPCs.DEV_RPC_HORTICULTURE_SPEED:
			{
				OnRPCHorticultureSpeed(ctx, player);
				break;
			}
		}
	}
	
	//get update status from ctx data
	bool GetRPCUpdateState( ParamsReadContext ctx )
	{
		Param1<bool> p = new Param1<bool>( 0 );
		if ( ctx.Read( p ) )
		{
			return p.param1;
		}
		
		return false;
	}
	
	//get modifier id from ctx data
	int GetRPCModifierID( ParamsReadContext ctx )
	{
		Param1<int> p = new Param1<int>( 0 );
		if ( ctx.Read( p ) )
		{
			return p.param1;
		}
		
		return 0;
	}
	
	//get modifier lock state from lock ctx data
	void GetRPCModifierLock( ParamsReadContext ctx, out int id, out bool lock )
	{
		Param2<int, bool> p = new Param2<int, bool>( 0, false );
		if ( ctx.Read( p ) )
		{
			id = p.param1;
			lock = p.param2;
		}
	}

	//============================================
	// STATS
	//============================================	
	//send player stats through RPC
	void SendRPCStats( PlayerBase player )
	{
		//write and send values
		if ( player )
		{
			array<ref Param> rpc_params = new array<ref Param>;
			
			//param count
			for ( int i = 0; i < player.m_PlayerStats.GetPCO().Get().Count(); i++ ) 
			{
				string label = player.m_PlayerStats.GetPCO().Get().Get( i ).GetLabel();
				float value = (float) player.m_PlayerStats.GetPCO().Get().Get( i ).Get();
				float value_norm = (float) player.m_PlayerStats.GetPCO().Get().Get( i ).GetNormalized();
				rpc_params.Insert( new Param3<string, float, float>( label, value, value_norm ) );
			}
			rpc_params.Insert(new Param3<string, float, float>( "(NaS)Immunity", player.GetImmunity(),0 ));
			rpc_params.Insert(new Param3<string, float, float>( "(NaS)BrokenLegs", player.m_BrokenLegState,0 ));
		
			rpc_params.InsertAt( new Param1<int>(rpc_params.Count() ), 0);// param count
			//send params
			GetDayZGame().RPC( player, ERPCs.DEV_RPC_STATS_DATA, rpc_params, true, player.GetIdentity() );
		}
	}

	//Display player stats
	void OnRPCStats( ParamsReadContext ctx )
	{
		//clear values
		m_PlayerStatsSynced.Clear();
		
		
		Param1<int> p_count = new Param1<int>(0);
		
		Param3<string, float, float> p = new Param3<string, float, float>( "", 0, 0 );
		
		//get param count
		int param_count = 0;
		if ( ctx.Read(p_count) )
		{
			param_count = p_count.param1;
		}
		
		//read values and set 
		for ( int i = 0; i < param_count; i++ )
		{
			ctx.Read(p);
			m_PlayerStatsSynced.Insert( new SyncedValue( p.param1, p.param2, false, p.param3 ) );
		}
	}

	//Update data on local
	void UpdateStatsLocal()
	{
		PlayerBase player = PlayerBase.Cast( GetDayZGame().GetPlayer() );
		
		//clear values
		m_PlayerStatsSynced.Clear();
		
		//set values
		for ( int i = 0; i < player.m_PlayerStats.GetPCO().Get().Count(); i++ ) 
		{
			string label = player.m_PlayerStats.GetPCO().Get().Get( i ).GetLabel();
			float value = player.m_PlayerStats.GetPCO().Get().Get( i ).Get();
			float value_norm = player.m_PlayerStats.GetPCO().Get().Get( i ).GetNormalized();
			m_PlayerStatsSynced.Insert( new SyncedValue( label, value, false, value_norm ) );
		}
		
		m_PlayerStatsSynced.Insert(new SyncedValue(  "(NaS)Immunity", player.GetImmunity() , false, 0));
		m_PlayerStatsSynced.Insert(new SyncedValue(  "(NaS) BrokenLegs", player.m_BrokenLegState, false, 0));
	}
	
	void OnRPCStatSet( ParamsReadContext ctx , PlayerBase player)
	{
		Param2<string, float> p = new Param2<string, float>( "", 0 );
		if ( ctx.Read(p) )
		{
			for ( int i = 0; i < player.m_PlayerStats.GetPCO().Get().Count(); i++ ) 
			{
				string label = player.m_PlayerStats.GetPCO().Get().Get( i ).GetLabel();
				if ( label == p.param1 )
				{
					player.m_PlayerStats.GetPCO().Get().Get( i).SetByFloatEx(p.param2);
				}
				
			}	
		}
	
	}
	
	//============================================
	// HEALTH
	//============================================	
	ref array<ref SyncedValue> GetHealthZonesSynched()
	{
		return m_TargetHealthSynced;
	}
	
	bool GetIsTargetChanged()
	{
		return m_IsTargetChanged;
	}
	
	void SetTargetChanged(bool state)
	{
		m_IsTargetChanged = state;
	}
	
	void SendRPCHealth(PlayerBase player)
	{
		//write and send values
		if ( player )
		{
			array<ref Param> rpc_params = new array<ref Param>;
			
			EntityAI target;
			if (m_HealthObserverMode.Get(player) == false)
				target = player;
			else 
			{
				PluginItemDiagnostic itemDiagPlugin = PluginItemDiagnostic.Cast(GetPlugin(PluginItemDiagnostic));	
				target =  EntityAI.Cast(itemDiagPlugin.GetWatchedItem(player));
			}
			
			if (target)
			{
				DamageZoneMap dmgZones = target.GetEntityDamageZoneMap();
							
				SetupZoneValues(rpc_params, dmgZones, target, "Health", 0);
				SetupZoneValues(rpc_params, dmgZones, target, "Shock", 1);
				SetupZoneValues(rpc_params, dmgZones, target, "Blood", 2);
			}
			
			bool targetChanged = false;
			if (target != m_HealthObserverEntity.Get(player))
			{
				targetChanged = true;
				m_HealthObserverEntity.Set(player, target);
			}
				
			rpc_params.InsertAt( new Param2<int, bool>(rpc_params.Count(), targetChanged), 0);

			GetDayZGame().RPC( player, ERPCs.DEV_RPC_HEALTH_DATA, rpc_params, true, player.GetIdentity() );
		}
	}
	
	void OnRPCHealth(ParamsReadContext ctx , PlayerBase player)
	{
		m_TargetHealthSynced.Clear();
		
		Param2<int, bool> paramsHeader = new Param2<int, bool>(0, false);
		Param4<string, float, float, bool> p = new Param4<string, float, float, bool>("", 0, 0, false);
		
		int paramCount = 0;
		if ( ctx.Read(paramsHeader) )
		{
			paramCount = paramsHeader.param1;
			SetTargetChanged(paramsHeader.param2);
		}
		
		for ( int i = 0; i < paramCount; i++ )
		{
			ctx.Read(p);
			m_TargetHealthSynced.Insert( new SyncedValue( p.param1, p.param2, p.param4, p.param3 ) );
		}
	}
	
	void SetupZoneValues(inout array<ref Param> rpc_params, DamageZoneMap dmgZones, EntityAI target, string healthType, int typeID)
	{
		array<string> outHealthTypes = new array<string>;
		
		//title entry
		bool isTitleEntry = true;
		rpc_params.Insert( new Param4<string, float, float, bool>( "", 0, typeID, isTitleEntry ) );
		
		// global entry
		isTitleEntry = false;
		float value;
		target.GetAdditionalHealthTypes("", outHealthTypes);
		if (healthType == "Health" || outHealthTypes.Find(healthType) != -1) // zone might not have additional health type
		{
			value = target.GetHealth("", healthType);
			rpc_params.Insert( new Param4<string, float, float, bool>( "Global", value, typeID, isTitleEntry ) );
		}
				
		//entries
		int count = dmgZones.Count();
		for (int i = 0; i < count; i++)
		{
			string zoneName = dmgZones.GetKey(i);
			outHealthTypes.Clear();
			target.GetAdditionalHealthTypes(zoneName, outHealthTypes);
			
			if (healthType != "Health")
			{
				if (outHealthTypes.Find(healthType) == -1)
					continue;
			}
				
			if (target.GetMaxHealth(zoneName, healthType) == 0)	
				continue;
			
			isTitleEntry = false;
			value = target.GetHealth(zoneName, healthType);
			rpc_params.Insert( new Param4<string, float, float, bool>( zoneName, value, typeID, isTitleEntry ) );
		}
	}
	
	// Used for client requesting health change or targeting mode (self/target) change
	void OnRPCHealthSet( ParamsReadContext ctx , PlayerBase player)
	{
		Param3<int, string, string> p = new Param3<int, string, string>( 0, "", "" );
		if ( !ctx.Read(p) )
			return;
					
		int buttonID = p.param1;
		string zoneName = p.param2;
		string healthType = p.param3;
		
		if (buttonID > 3) // target change only
		{
			if (buttonID == 4)		// ButtonTarget
				m_HealthObserverMode.Set(player, true);
			else if (buttonID == 5)	// ButtonSelf
				m_HealthObserverMode.Set(player, false);
			
			return;
		}
		
		EntityAI target;
		if (m_HealthObserverMode.Get(player) == false)
			target = player;
		else 
		{
			PluginItemDiagnostic itemDiagPlugin = PluginItemDiagnostic.Cast(GetPlugin(PluginItemDiagnostic));	
			target =  EntityAI.Cast(itemDiagPlugin.GetWatchedItem(player));
		}
		
		if (zoneName == "Global")
			zoneName = "";
		
		float zoneMax = target.GetMaxHealth(zoneName, healthType);
		float zoneCurrentHealth = target.GetHealth(zoneName, healthType);
		float targetHealth;
		
		if (buttonID == 0)			// ButtonMin
			targetHealth = 0;
		else if  (buttonID == 1)	// ButtonMax
			targetHealth = zoneMax;		
		else if  (buttonID == 2)	// ButtonAdd
			targetHealth = zoneCurrentHealth + zoneMax * 0.1;
		else if  (buttonID == 3)	// ButtonSubtract
			targetHealth = zoneCurrentHealth - zoneMax * 0.1;
		
		target.SetHealth(zoneName, healthType, targetHealth);
	}
	
	void OnRPCHorticultUpdate( ParamsReadContext ctx , PlayerBase player)
	{
		Param6<int, GardenBase, int, int, int, int> p = new Param6<int, GardenBase, int, int, int, int>(-1, null, 0, 0, 0, 0);
		if ( !ctx.Read(p) )
			return;
		
		GardenBase garden = p.param2;
		
		if (p.param1 != -1 && garden)
			{
				Slot gSlot = garden.GetSlotByIndex(p.param1);
				if (gSlot && gSlot.GetPlant())
					gSlot.GetPlant().DebugSetTimes(p.param3, p.param4, p.param5, p.param6);
			}
		else
			PlantBase.DebugSetGlobalTimes(p.param3, p.param4, p.param5, p.param6);
	}
	
	void OnRPCHorticultureSpeed(ParamsReadContext ctx , PlayerBase player)
	{
		Param1<float> p = new Param1<float>(1);
		if (!ctx.Read(p))
			return;
		
		PlantBase.DebugSetTickSpeedMultiplier(p.param1);
	}

	//============================================
	// LEVELS
	//============================================	
	//send player stats through RPC
	void SendRPCLevels( PlayerBase player )
	{
		//write and send values
		if ( player )
		{
			array<ref Param> rpc_params = new array<ref Param>;

			rpc_params.Insert(new Param3<string, float, float>( "Immunity: "+ typename.EnumToString(EStatLevels, player.GetImmunityLevel() ),player.GetImmunityLevel(), player.GetBordersImmunity()));
			rpc_params.Insert(new Param3<string, float, float>( "Blood: "+ typename.EnumToString(EStatLevels, player.GetStatLevelBlood()),player.GetStatLevelBlood(), player.GetStatBordersBlood()));
			rpc_params.Insert(new Param3<string, float, float>( "Health: "+ typename.EnumToString(EStatLevels, player.GetStatLevelHealth() ),player.GetStatLevelHealth(), player.GetStatBordersHealth()));
			rpc_params.Insert(new Param3<string, float, float>( "Energy: "+ typename.EnumToString(EStatLevels, player.GetStatLevelEnergy() ),player.GetStatLevelEnergy(), player.GetStatBordersEnergy()));
			rpc_params.Insert(new Param3<string, float, float>( "Water: "+ typename.EnumToString(EStatLevels, player.GetStatLevelWater() ),player.GetStatLevelWater(), player.GetStatBordersWater()));
			
			int param_count = rpc_params.Count();
			rpc_params.InsertAt( new Param3<string, float, float>( "PARAM_COUNT", param_count,0 ), 0);
			//send params
			GetDayZGame().RPC( player, ERPCs.DEV_RPC_LEVELS_DATA, rpc_params, true, player.GetIdentity() );
		}
	}

	//Display player stats
	void OnRPCLevels( ParamsReadContext ctx )
	{
		//clear values
		m_PlayerLevelsSynced.Clear();
		
		Param3<string, float, float> p3 = new Param3<string, float, float>( "", 0,0 );
		
		//get param count
		int param_count = 0;
		if ( ctx.Read(p3) )
		{
			param_count = p3.param2;
		}
		
		//read values and set 
		for ( int i = 0; i < param_count; i++ )
		{
			if ( ctx.Read(p3) )
				m_PlayerLevelsSynced.Insert( new SyncedValueLevel( p3.param1, p3.param2, p3.param3 ) );
		}
	}
	
	
	void SendRPCMods( PlayerBase player )
	{
		//write and send values
		if ( player )
		{
			array<ref Param> rpc_params = new array<ref Param>;
			
			//get modifiers
			ModifiersManager mods_manager = player.GetModifiersManager();
			
			//param count
			array<ref ModifierDebugObj> modifiers = new array<ref ModifierDebugObj>;
			mods_manager.DbgGetModifiers( modifiers );
			
			float param_count = ( float ) modifiers.Count();
			rpc_params.Insert( new Param1<int>( param_count ));
			
			//set modifiers
			for ( int i = 0; i < modifiers.Count(); ++i ) 
			{
				int id = modifiers.Get(i).GetID();
				string name = modifiers.Get(i).GetName();
				bool active =  modifiers.Get(i).IsActive();
				bool locked =  modifiers.Get(i).IsLocked();
				
				rpc_params.Insert( new Param4<int, string, bool, bool>( id, name, active, locked ) );
			}
			
			//send params
			GetDayZGame().RPC( player, ERPCs.DEV_RPC_MODS_DATA, rpc_params, true, player.GetIdentity() );
		}
	}
	
	//============================================
	// MODS
	//============================================	
	//send player mods through RPC
	void SendRPCModsDetail( PlayerBase player )
	{
		//write and send values
		if ( player )
		{
			Param1<string> p1 = new Param1<string>("");
			ModifiersManager mods_manager = player.GetModifiersManager();
			int mod_id = m_RegisteredPlayers.Get(player).m_DetailedModifierIndex;
			p1.param1 = mods_manager.GetModifier(mod_id).GetDebugText();
			//send params
			if (p1.param1 != "")
				GetDayZGame().RPCSingleParam( player, ERPCs.DEV_RPC_MODS_DATA_DETAILED, p1, true, player.GetIdentity() );
		}
	}

	//Display player modifiers
	void OnRPCMods( ParamsReadContext ctx )
	{
		//clear values
		m_PlayerModsSynced.Clear();
		
		//ref Param3<string, float, bool> p = new Param3<string, float, bool>( "", 0, false );
		Param1<int> p1 = new Param1<int>(0);
		Param4<int, string, bool, bool> p4 = new Param4<int, string, bool, bool>( 0,"", false, false );
		//get param count
		int param_count = 0;
		if ( ctx.Read(p1) )
		{
			param_count = (int) p1.param1;
		}
		
		//read values and set 
		for ( int i = 0; i < param_count; i++ )
		{
			if (ctx.Read(p4))
			{
				m_PlayerModsSynced.Insert( new SyncedValueModifier( p4.param1, p4.param2, p4.param3, p4.param4 ) );
			}
		}
	}
	
	void OnRPCModsDetailed( ParamsReadContext ctx )
	{
		Param1<string> p1 = new Param1<string>("");
		
		//get param count
		if ( ctx.Read(p1) )
		{
			m_PlayerModsDetailedSynced = p1.param1;
		}
	}

	
	/*
	//Activates modifier with given ID
	void RequestDetailedInfo( int id, PlayerBase player = NULL )
	{
		int modifier_id = Math.AbsInt( id );
		if(id == m_DetailedInfoRequested)
		{
			m_DetailedInfoRequested = 0;//repeated request --> disable
		}
		else
		{
			m_DetailedInfoRequested = modifier_id;
		}
	}	
	*/
	//Activates modifier with given ID
	void RequestDetailedInfo( int id, notnull PlayerBase player)
	{
		int modifier_id = Math.AbsInt( id );
		m_RegisteredPlayers.Get(player).m_DetailedModifierIndex = modifier_id;
		
		if (IsSetSystemInBitmask(player, PDS_SYSTEMS.MODS_DETAILED ))
		{
			SetSystemInBitmask(player, PDS_SYSTEMS.MODS_DETAILED, false);
		}
		else
		{
			SetSystemInBitmask(player, PDS_SYSTEMS.MODS_DETAILED, true);
		}
	}
	
	
	//Activates modifier with given ID
	void ActivateModifier( int id, PlayerBase player = NULL )
	{
		if ( !player )
		{
			player = PlayerBase.Cast( GetDayZGame().GetPlayer() );
		}
		ModifiersManager mods_manager = player.GetModifiersManager();
		mods_manager.ActivateModifier( Math.AbsInt( id ) );
	}

	//Activates modifier with given ID
	void DeactivateModifier( int id, PlayerBase player = NULL )
	{
		if ( !player )
		{
			player = PlayerBase.Cast( GetDayZGame().GetPlayer() );
		}
		ModifiersManager mods_manager = player.GetModifiersManager();
		mods_manager.DeactivateModifier( Math.AbsInt( id ) );
	}

	//Activates modifier with given ID
	void LockModifier( int id, bool lock, PlayerBase player = NULL )
	{
		if ( !player )
		{
			player = PlayerBase.Cast( GetDayZGame().GetPlayer() );
		}
		ModifiersManager mods_manager = player.GetModifiersManager();
		mods_manager.SetModifierLock( Math.AbsInt( id ), lock );
	}
	
	bool IsModifierLocked( int id, PlayerBase player = NULL )
	{
		if ( !player )
		{
			player = PlayerBase.Cast( GetDayZGame().GetPlayer() );
		}
		ModifiersManager mods_manager = player.GetModifiersManager();	
		
		return mods_manager.GetModifierLock( Math.AbsInt( id ) );
	}
	
	//============================================
	// TEMPERATURE
	//============================================	

	void SendRPCTemp( PlayerBase player )
	{
		if ( player )
		{
			CachedObjectsParams.PARAM1_STRING.param1 = player.m_Environment.GetDebugMessage();
			GetDayZGame().RPCSingleParam( player, ERPCs.DEV_TEMP_UPDATE, CachedObjectsParams.PARAM1_STRING, true, player.GetIdentity() );
		}
	}

	void OnRPCTemp( ParamsReadContext ctx , PlayerBase player)
	{
		if (g_Game.IsServer() && g_Game.IsMultiplayer())
		{
			EnableUpdate( GetRPCUpdateState( ctx ), ERPCs.DEV_TEMP_UPDATE, player );
		}
		else if (!g_Game.IsMultiplayer() || g_Game.IsClient())
		{
			if ( ctx.Read(CachedObjectsParams.PARAM1_STRING) )
			{
				m_EnvironmentDebugMessage = CachedObjectsParams.PARAM1_STRING.param1;
			}
		}

	}
	
	//============================================
	// AGENTS
	//============================================	
	//send player mods through RPC
	void SendRPCAgents( PlayerBase player )
	{
		//write and send values
		if ( ( player && player.GetIdentity() && player.m_AgentPool != null ) || ( player && !g_Game.IsMultiplayer() && player.m_AgentPool != null ) )
		{
			
			//get agent pool data
			array<ref Param> agent_pool = new array<ref Param>;
			player.m_AgentPool.GetDebugObject( agent_pool );
		
			GetDayZGame().RPC( player, ERPCs.DEV_RPC_AGENTS_DATA, agent_pool, true, player.GetIdentity() );
		}
	}

	//Display player modifiers
	void OnRPCAgents(ParamsReadContext ctx)
	{
		//clear values
		m_PlayerAgentsSynced.Clear();
		
		Param4<string, string, int, float> p4 = new Param4<string, string, int, float>("", "" , 0, 0.0);
		Param1<int> p1 = new Param1<int>(0);
		
		//get param count
		int paramCount = 0;
		if (ctx.Read(p1))
			paramCount = p1.param1;
		
		//read values and set 
		for (int i = 0; i < paramCount; i++)
		{
			ctx.Read(p4);
			m_PlayerAgentsSynced.Insert(new SyncedValueAgent(p4.param1, p4.param2, p4.param3, p4.param4));
		}
	}
	
	//============================================
	// STOMACH
	//============================================	

	void SendRPCStomach( PlayerBase player )
	{
		//write and send values
		if ( player )
		{			
			array<ref Param> stomach = new array<ref Param>;
			int count = player.m_PlayerStomach.GetDebugObject( stomach );
			stomach.InsertAt(new Param1<int>(count), 0);
		
			GetDayZGame().RPC( player, ERPCs.DEV_RPC_STOMACH_DATA, stomach, true, player.GetIdentity() );
		}
	}


	void OnRPCStomach( ParamsReadContext ctx )
	{
		//clear values
		m_PlayerStomachSynced.Clear();
		
		Param1<int> p1 = new Param1<int>(0);
		int param_count = 0;
		if ( ctx.Read(p1) )
		{
			param_count = p1.param1;
		}
		
		if (param_count)
		{
			//invidividual stomach items
			for ( int i = 0; i < param_count; i++ )
			{
				Param5<int,int,int,float,float> p5 = new Param5<int,int,int,float,float>(0,0,0,0,0);
				ctx.Read(p5);
				m_PlayerStomachSynced.Insert(p5);
			}
			//volume
			Param1<float> p1b = new Param1<float>(0);
			ctx.Read(p1b);
			m_PlayerStomachSynced.Insert(p1b);
			//temperature
			Param1<float> paramTemp = new Param1<float>(0);
			ctx.Read(paramTemp);
			m_PlayerStomachSynced.Insert(paramTemp);
		}
	}

	//================================================================
	// FOCUS
	//================================================================ 
	void CaptureFocus()
	{
		g_Game.GetInput().ChangeGameFocus( 1 );
		g_Game.GetUIManager().ShowUICursor( true );
	}
	
	void ReleaseFocus()
	{
		g_Game.GetInput().ChangeGameFocus( -1 );
		g_Game.GetUIManager().ShowUICursor( false );
	}
	
	void ToggleFocus()
	{
		if ( g_Game.GetInput().HasGameFocus() )
		{
			g_Game.GetInput().ChangeGameFocus( 1 );
			g_Game.GetUIManager().ShowUICursor( true );
		}
		else
		{
			g_Game.GetInput().ResetGameFocus();
			g_Game.GetUIManager().ShowUICursor( false );
		}
	}
}
