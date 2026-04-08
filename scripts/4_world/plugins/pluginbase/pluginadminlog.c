class PluginAdminLog extends PluginBase			// Class for admin log messages handled by script
{
	string 							m_PlayerName;
	string							m_Pid;
	vector 							m_Position; 
	string							m_PlayerPrefix;
	string 							m_PlayerPrefix2;
	string							m_Message;
	string							m_DisplayName;
	string 							m_HitMessage;
	float							m_Distance;
	PlayerBase						m_Player;
	PlayerBase						m_Source;
	string							m_ItemInHands;
	string 							m_PosArray[3];
	int 							m_DotIndex;
	PlayerStat<float>				m_StatWater;
	PlayerStat<float>				m_StatEnergy;
	BleedingSourcesManagerServer	m_BleedMgr;
	// filters
	protected int 					m_HitFilter;
	protected int 					m_PlacementFilter;
	protected int 					m_ActionsFilter;
	protected int 					m_PlayerListFilter;
	
	ref Timer						m_Timer;
	autoptr array<Man>				m_PlayerArray; 	
	const int 						TIMER_PLAYERLIST = GetPlayerListTimer();
	
	static int GetPlayerListTimer()
	{
		return 300; // seconds
	} 
	
	/* 
		EXE side ADM log messages (not removable):
		Connect / Disconnect
		Chat
		Player->Admin report (ingame chat: #toadmin <text>)	
	*/
	
	void PluginAdminLog()
	{
		m_HitFilter = g_Game.ServerConfigGetInt("adminLogPlayerHitsOnly");  //  1 - log player hits only / 0 - log all hits ( animals/infected )
		m_PlacementFilter = g_Game.ServerConfigGetInt("adminLogPlacement"); //  1 - log placement ( traps, tents )
		m_ActionsFilter = g_Game.ServerConfigGetInt("adminLogBuildActions"); //  1 - log basebuilding actions ( build, dismantle, destroy )
		m_PlayerListFilter = g_Game.ServerConfigGetInt("adminLogPlayerList"); // 1 - log periodic player list with position every 5 minutes
		
		m_PlayerArray = new array<Man>;
		
		if ( m_PlayerListFilter == 1 )
		{
			m_Timer = new Timer();
			m_Timer.Run( TIMER_PLAYERLIST , this, "PlayerList", NULL, true );
		}
	}
	
	void ~PluginAdminLog()
	{
	}
			
	void LogPrint( string message )
	{
		g_Game.AdminLog( message );
	}
			
	string GetPlayerPrefix( PlayerBase player, PlayerIdentity identity )  // player name + id + position prefix for log prints
	{	
		
		m_Position = player.GetPosition();
		m_PosArray[3] = { m_Position[0].ToString(), m_Position[2].ToString(), m_Position[1].ToString() };
		
		for ( int i = 0; i < 3; i++ )	// trim position precision
		{
			m_DotIndex = m_PosArray[i].IndexOf(".");
			if ( m_DotIndex != -1 )
			{
			m_PosArray[i] = m_PosArray[i].Substring( 0, m_DotIndex + 2 );
			}
		}
		
		if ( identity ) 	// return partial message even if it fails to fetch identity 
		{
			//return "Player \"" + "Unknown/Dead Entity" + "\" (id=" + "Unknown" + " pos=<" +  m_PosArray[0] + ", " + m_PosArray[1] + ", " + m_PosArray[2] + ">)";
			m_PlayerName = "\"" + identity.GetName() + "\"";
			m_Pid = identity.GetId();
		}
		else
		{
			m_PlayerName = "\"" + player.GetCachedName() + "\"";
			m_Pid = player.GetCachedID();
		}
		
		
		if ( !player.IsAlive() )
		{
		 	m_PlayerName = m_PlayerName + " (DEAD)"; 
		}
		
		return "Player " + m_PlayerName + " (id=" + m_Pid + " pos=<" +  m_PosArray[0] + ", " + m_PosArray[1] + ", " + m_PosArray[2] + ">)";
	}
	
	string GetHitMessage( TotalDamageResult damageResult, int component, string zone, string ammo) 
	{	
		if ( damageResult )	
		{
			float dmg = damageResult.GetHighestDamage("Health");
			return " into " + zone + "(" + component.ToString() + ") for " + dmg.ToString() + " damage (" + ammo + ")";
		}
		else 				// block
		{
			return " into Block" + "(" + component.ToString() + ") for 0 damage ";
		}
	}
	
	void PlayerKilled( PlayerBase player, Object source )  // PlayerBase.c   
	{
		if (!player || !source) 
		{
	        LogPrint("DEBUG: PlayerKilled() player/source does not exist");
	        return;
    	}
		
		PlayerBase playerSource = PlayerBase.Cast( EntityAI.Cast( source ).GetHierarchyParent() );	
		if (!playerSource)
		{
			playerSource = PlayerBase.Cast( source );	
		}
		
		string playerPrefix, playerPrefix2;
		if (playerSource)
		{
			playerPrefix2 = GetPlayerPrefix( playerSource ,  playerSource.GetIdentity() );
		}
		
		playerPrefix = GetPlayerPrefix( player , player.GetIdentity() );
		if (player == source)	// deaths not caused by another object (starvation, dehydration)
		{
			m_StatWater = player.GetStatWater();
			m_StatEnergy = player.GetStatEnergy();
			m_BleedMgr = player.GetBleedingManagerServer();
			
			string reason = " died.";
			if (player.GetDrowningWaterLevelCheck())
				reason = " drowned.";

			string additionalStats;
			if (m_StatWater && m_StatEnergy)
				additionalStats = " Stats> Water: " + m_StatWater.Get().ToString() + " Energy: " + m_StatEnergy.Get().ToString();
			
			if (m_BleedMgr)
				additionalStats = additionalStats + " Bleed sources: " + m_BleedMgr.GetBleedingSourcesCount().ToString();
			
			LogPrint(playerPrefix + reason + additionalStats);
			
		}
		else if (source.IsWeapon() || source.IsMeleeWeapon())  // player
		{			
			if (ExplosivesBase.Cast(source))
			{
				LogPrint( playerPrefix + " killed by " + source.GetDisplayName() );
			}
			else if (source.IsMeleeWeapon())
			{			
				LogPrint( playerPrefix + " killed by " + playerPrefix2 + " with " + source.GetDisplayName() );	
			}
			else
			{
				m_Distance = vector.Distance( player.GetPosition(), playerSource.GetPosition() );
				LogPrint( playerPrefix + " killed by " + playerPrefix2 + " with " + source.GetDisplayName() + " from " + m_Distance + " meters " );
			}
		}
		else
		{
			if (playerSource)
			{
				//fists
				LogPrint( playerPrefix + " killed by " + playerPrefix2 + " with (MeleeFist)" );	
			}
			else
			{
				//rest, Animals, Zombies
				LogPrint( playerPrefix + " killed by " + source.GetType());
			}
			
		}
	}
	
	void PlayerKilledByDrowningUncon(PlayerBase player)
	{
		string playerPrefix = GetPlayerPrefix( player , player.GetIdentity() );
		LogPrint( playerPrefix + " has drowned while unconscious" );
	}
	
	void PlayerKilledByRespawn(PlayerBase player)
	{
		string playerPrefix = GetPlayerPrefix( player , player.GetIdentity() );
		LogPrint( playerPrefix + " is choosing to respawn" );
	}
	
	void PlayerKilledByDisconnect(PlayerBase player)
	{
		string playerPrefix = GetPlayerPrefix( player , player.GetIdentity() );
		if (player.IsUnconscious())
			LogPrint( playerPrefix + " is disconnecting while being unconscious" );
		else if (player.IsRestrained())
			LogPrint( playerPrefix + " is disconnecting while being restrained" );
	}
		
	void PlayerHitBy( TotalDamageResult damageResult, int damageType, PlayerBase player, EntityAI source, int component, string dmgZone, string ammo ) // PlayerBase.c 
	{
		if ( player && source )		
		{
			string playerPrefix = GetPlayerPrefix( player ,  player.GetIdentity() ) + "[HP: " + player.GetHealth().ToString() + "]";
			string playerPrefix2;
			m_HitMessage = GetHitMessage( damageResult, component, dmgZone, ammo );
			PlayerBase playerSource;
			
			if ( source.IsPlayer() )// Fists
				playerSource = PlayerBase.Cast( source );
			else
				playerSource = PlayerBase.Cast( source.GetHierarchyParent() );
			
			if (playerSource)
				playerPrefix2 = GetPlayerPrefix( playerSource ,  playerSource.GetIdentity() );
			
			switch ( damageType )
			{
				case DamageType.CLOSE_COMBAT:	// Player melee, animals, infected 
				
					if (source.IsZombie() || source.IsAnimal())  // Infected & Animals
					{
						if (m_HitFilter == 1)
							break;
					
						m_DisplayName = source.GetDisplayName();
												
						LogPrint( playerPrefix + " hit by " + m_DisplayName + m_HitMessage );	
					}			
					else if (source.IsPlayer())// Fists
					{
						LogPrint( playerPrefix + " hit by " + playerPrefix2 + m_HitMessage );
					}
					else if ( playerSource && (source.IsMeleeWeapon() || source.IsWeapon()))			// Melee weapons
					{				
						m_ItemInHands = source.GetDisplayName();
							
						LogPrint( playerPrefix + " hit by " + playerPrefix2 + m_HitMessage + " with " + m_ItemInHands );				
					}
					else
					{
						m_DisplayName = source.GetType();
					
						LogPrint( playerPrefix + " hit by " + m_DisplayName + m_HitMessage );					
					} 
					break;
				
				case DamageType.FIRE_ARM:	// Player ranged
				
					if ( source.IsWeapon() && playerSource )
					{
						m_ItemInHands = source.GetDisplayName();				
						m_Distance = vector.Distance( player.GetPosition(), playerSource.GetPosition() );
					
						LogPrint( playerPrefix + " hit by " + playerPrefix2 + m_HitMessage + " with " + m_ItemInHands + " from " + m_Distance + " meters ");
					}
					else 
					{
						m_DisplayName = source.GetType();
					
						LogPrint( playerPrefix + " hit by " + m_DisplayName + m_HitMessage );			
					}
					break;
				
				case DamageType.EXPLOSION:	// Explosion
				
					LogPrint( playerPrefix + " hit by explosion (" + ammo + ")" );
					break;
						
				case DamageType.STUN: 		// unused atm
				
					LogPrint( playerPrefix + " stunned by " + ammo );
					break;
						
				case DamageType.CUSTOM:		// Others (Vehicle hit, fall, fireplace, barbed wire ...)
					float globalHealthDamage = damageResult.GetDamage("", "Health");
					if (ammo == DayZPlayerImplementFallDamage.FALL_DAMAGE_AMMO_HEALTH || ammo == DayZPlayerImplementFallDamage.FALL_DAMAGE_AMMO_SHOCK || ammo == DayZPlayerImplementFallDamage.FALL_DAMAGE_AMMO_HEALTH_OTHER_ATTACHMENTS)
					{
						if (globalHealthDamage > 0.0)
							LogPrint(playerPrefix + " hit by " + ammo);	
					}
					else if ( source.GetType() == "AreaDamageManager" )  
					{
						EntityAI parent = EntityAI.Cast( source );
						if ( parent )
						{
							LogPrint( playerPrefix + " hit by " + parent.GetType() + " with " + ammo );	
						}
					}
					else
					{
						m_DisplayName = source.GetType();
										
						LogPrint( playerPrefix + " hit by " + m_DisplayName + " with " + ammo );
					}
					break;
											
				default:
				
					LogPrint("DEBUG: PlayerHitBy() unknown damageType: " + ammo );
					break;
			}
		}
		else
		{
			LogPrint("DEBUG: player/source does not exist");
		}
	}
	
	void UnconStart( PlayerBase player )	//  PlayerBase.c  
	{
		m_PlayerPrefix = GetPlayerPrefix( player ,  player.GetIdentity() );
		
		LogPrint( m_PlayerPrefix + " is unconscious" );
	}
	
	void UnconStop( PlayerBase player ) //  PlayerBase.c 
	{	
		if ( player.IsAlive() ) 	// Do not log uncon stop for dead players
		{
			m_PlayerPrefix = GetPlayerPrefix( player ,  player.GetIdentity() );
			
			LogPrint( m_PlayerPrefix + " regained consciousness" );		
		}
	}
		
	void OnPlacementComplete( Man player, ItemBase item ) // ItemBase.c
	{
		if ( m_PlacementFilter == 1 )
		{		
			m_Source = PlayerBase.Cast( player ); 
			m_PlayerPrefix = GetPlayerPrefix( m_Source , m_Source.GetIdentity() );		
			m_DisplayName = item.GetDisplayName();
			
			if ( m_DisplayName == "" )
			{
				LogPrint( m_PlayerPrefix + " placed Nameless Object" + "<" + item.GetType() + ">" );
			} 
			else
			{
				LogPrint( m_PlayerPrefix + " placed " + m_DisplayName + "<" + item.GetType() + ">");
			}
		}
	}
	
	void OnContinouousAction( ActionData action_data )	// ActionContinouousBase.c
	{
		if ( m_ActionsFilter == 1 )
		{						
			m_Message = action_data.m_Action.GetAdminLogMessage(action_data);
			
			if(m_Message == "")
				return;
			
			m_PlayerPrefix = GetPlayerPrefix( action_data.m_Player , action_data.m_Player.GetIdentity() );
			
			LogPrint( m_PlayerPrefix + m_Message );
		}	
	}
	
	void OnEmote(PlayerBase player, EmoteBase emote)
	{
		m_PlayerPrefix = GetPlayerPrefix( player ,  player.GetIdentity() );
		
		ItemBase item = player.GetItemInHands();
		if (item)
			LogPrint(m_PlayerPrefix + " performed " + emote.GetInputActionName() + " with " + item.GetType());
		else 
			LogPrint(m_PlayerPrefix + " performed " + emote.GetInputActionName());
	}
	
	void Suicide( PlayerBase player )  // EmoteManager.c 
	{
		m_PlayerPrefix = GetPlayerPrefix( player ,  player.GetIdentity() );
		
		LogPrint( m_PlayerPrefix + " committed suicide" );
	}
	
	void BleedingOut( PlayerBase player )  // Bleeding.c
	{
		m_PlayerPrefix = GetPlayerPrefix( player ,  player.GetIdentity() );
		
		LogPrint( m_PlayerPrefix + " bled out" );
	}
	
	//"top" == 'true' for flag all the way at the top, 'false' for all the way at the bottom
	void TotemFlagChange(bool top, notnull PlayerBase player, notnull EntityAI totem)
	{
		if (m_ActionsFilter !=1)
			return;

		string prefix = GetPlayerPrefix(player, player.GetIdentity());
		string flagType = totem.FindAttachmentBySlotName("Material_FPole_Flag").ClassName();
		string action;
		
		if (top)
			action = "raised ";
		else
			action = "lowered ";
		
		LogPrint( prefix + " has " + action + flagType + " on " + totem.ClassName() + " at " + totem.GetPosition());
	}
	
	void PlayerList()
	{
		g_Game.GetPlayers( m_PlayerArray );
		
		if ( m_PlayerArray.Count() != 0 )
		{	
			LogPrint( "##### PlayerList log: " + m_PlayerArray.Count().ToString() + " players" );
						
			foreach ( Man player: m_PlayerArray )
			{
				m_Player = PlayerBase.Cast(player);
				m_PlayerPrefix = GetPlayerPrefix( m_Player ,  m_Player.GetIdentity() );
				
				LogPrint( m_PlayerPrefix );							
			}
			
			LogPrint( "#####" );
		}
	}
	
	void PlayerTeleportedLog(notnull PlayerBase player, vector startPos, vector targetPos, string reason)
	{
		m_PlayerPrefix = GetPlayerPrefix(player, player.GetIdentity());
		
		LogPrint(m_PlayerPrefix + " was teleported from: " + startPos.ToString() + " to: " + targetPos.ToString() + ". Reason: " + reason);
	}
	
	void DirectAdminLogPrint(string str)
	{
		LogPrint(str);
	}
}