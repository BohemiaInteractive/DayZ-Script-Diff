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
		m_HitFilter = GetGame().ServerConfigGetInt("adminLogPlayerHitsOnly");  //  1 - log player hits only / 0 - log all hits ( animals/infected )
		m_PlacementFilter = GetGame().ServerConfigGetInt("adminLogPlacement"); //  1 - log placement ( traps, tents )
		m_ActionsFilter = GetGame().ServerConfigGetInt("adminLogBuildActions"); //  1 - log basebuilding actions ( build, dismantle, destroy )
		m_PlayerListFilter = GetGame().ServerConfigGetInt("adminLogPlayerList"); // 1 - log periodic player list with position every 5 minutes
		
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
		GetGame().AdminLog( message );
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
		
		if ( !identity ) 	// return partial message even if it fails to fetch identity 
		{
			return "Player \"" + "Unknown/Dead Entity" + "\" (id=" + "Unknown" + " pos=<" +  m_PosArray[0] + ", " + m_PosArray[1] + ", " + m_PosArray[2] + ">)";
		}
		
		m_PlayerName = "\"" + identity.GetName() + "\"";
		m_Pid = identity.GetId();
		
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
		
		if ( player && source )
		{
			m_PlayerPrefix = this.GetPlayerPrefix( player , player.GetIdentity() );
			
			if( player == source )	// deaths not caused by another object (starvation, dehydration)
			{
				m_StatWater = player.GetStatWater();
				m_StatEnergy = player.GetStatEnergy();
				m_BleedMgr = player.GetBleedingManagerServer();
				
				if ( m_StatWater && m_StatEnergy && m_BleedMgr )
				{
					LogPrint( m_PlayerPrefix + " died. Stats> Water: " + m_StatWater.Get().ToString() + " Energy: " + m_StatEnergy.Get().ToString() + " Bleed sources: " + m_BleedMgr.GetBleedingSourcesCount().ToString() );
				}
				else if ( m_StatWater && m_StatEnergy && !m_BleedMgr )
				{
					LogPrint( m_PlayerPrefix + " died. Stats> Water: " + m_StatWater.Get().ToString() + " Energy: " + m_StatEnergy.Get().ToString() );
				}
				else
				{
					LogPrint( m_PlayerPrefix + " died. Stats> could not fetch");
				}
			}
			else if ( source.IsWeapon() || source.IsMeleeWeapon() )  // player
			{				
				m_Source = PlayerBase.Cast( EntityAI.Cast( source ).GetHierarchyParent() );
				m_PlayerPrefix2 = "";
				if(m_Source)
				{
					m_PlayerPrefix2 = this.GetPlayerPrefix( m_Source ,  m_Source.GetIdentity() );
				}
				
				if ( source.IsMeleeWeapon() )
				{	
					LogPrint( m_PlayerPrefix + " killed by " + m_PlayerPrefix2 + " with " + source.GetDisplayName() );	
				}
				else
				{
					m_Distance = vector.Distance( player.GetPosition(), m_Source.GetPosition() );
					LogPrint( m_PlayerPrefix + " killed by " + m_PlayerPrefix2 + " with " + source.GetDisplayName() + " from " + m_Distance + " meters " );
				}
			}
			else					// others
			{
				LogPrint( m_PlayerPrefix + " killed by " + source.GetType() );
			}
		}
		else 
		{
			LogPrint("DEBUG: PlayerKilled() player/source does not exist");
		}	
	}
		
	void PlayerHitBy( TotalDamageResult damageResult, int damageType, PlayerBase player, EntityAI source, int component, string dmgZone, string ammo ) // PlayerBase.c 
	{
		if ( player && source )		
		{
			m_PlayerPrefix = this.GetPlayerPrefix( player ,  player.GetIdentity() ) + "[HP: " + player.GetHealth().ToString() + "]";
			m_HitMessage = this.GetHitMessage( damageResult, component, dmgZone, ammo );
			
			switch ( damageType )
			{
				case DT_CLOSE_COMBAT:	// Player melee, animals, infected 
				
					if ( m_HitFilter != 1 && ( source.IsZombie() || source.IsAnimal() ) )  // Infected & Animals
					{
						m_DisplayName = source.GetDisplayName();
												
						LogPrint( m_PlayerPrefix + " hit by " + m_DisplayName + m_HitMessage );	
					}			
					else if ( source.IsPlayer() )				// Fists
					{
						m_Source = PlayerBase.Cast( source );
						m_PlayerPrefix2 = this.GetPlayerPrefix( m_Source ,  m_Source.GetIdentity() );
					
						LogPrint( m_PlayerPrefix + " hit by " + m_PlayerPrefix2 + m_HitMessage );
					}
					else if ( source.IsMeleeWeapon() )			// Melee weapons
					{				
						m_ItemInHands = source.GetDisplayName();		
						m_Source = PlayerBase.Cast( source.GetHierarchyParent() );
						m_PlayerPrefix2 = this.GetPlayerPrefix( m_Source ,  m_Source.GetIdentity() );
			
						LogPrint( m_PlayerPrefix + " hit by " + m_PlayerPrefix2 + m_HitMessage + " with " + m_ItemInHands );				
					}
					else
					{
						m_DisplayName = source.GetType();
					
						LogPrint( m_PlayerPrefix + " hit by " + m_DisplayName + m_HitMessage );					
					} 
					break;
				
				case DT_FIRE_ARM:	// Player ranged
				
					if ( source.IsWeapon() )
					{
						m_ItemInHands = source.GetDisplayName();				
						m_Source = PlayerBase.Cast( source.GetHierarchyParent() );
						m_PlayerPrefix2 = this.GetPlayerPrefix( m_Source ,  m_Source.GetIdentity() );
						m_Distance = vector.Distance( player.GetPosition(), m_Source.GetPosition() );
					
						LogPrint( m_PlayerPrefix + " hit by " + m_PlayerPrefix2 + m_HitMessage + " with " + m_ItemInHands + " from " + m_Distance + " meters ");
					}
					else 
					{
						m_DisplayName = source.GetType();
					
						LogPrint( m_PlayerPrefix + " hit by " + m_DisplayName + m_HitMessage );			
					}
					break;
				
				case DT_EXPLOSION:	// Explosion
				
					LogPrint( m_PlayerPrefix + " hit by explosion (" + ammo + ")" );
					break;
						
				case DT_STUN: 		// unused atm
				
					LogPrint( m_PlayerPrefix + " stunned by " + ammo );
					break;
						
				case DT_CUSTOM:		// Others (Vehicle hit, fall, fireplace, barbed wire ...)
								
					if ( ammo == "FallDamage" )			// Fall
					{
						LogPrint( m_PlayerPrefix + " hit by " + ammo );	
					}
					else if ( source.GetType() == "AreaDamageManager" )  
					{
						EntityAI parent = EntityAI.Cast( source );
						if ( parent )
						{
							LogPrint( m_PlayerPrefix + " hit by " + parent.GetType() + " with " + ammo );	
						}
					}
					else
					{
						m_DisplayName = source.GetType();
										
						LogPrint( m_PlayerPrefix + " hit by " + m_DisplayName + " with " + ammo );
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
		m_PlayerPrefix = this.GetPlayerPrefix( player ,  player.GetIdentity() );
		
		LogPrint( m_PlayerPrefix + " is unconscious" );
	}
	
	void UnconStop( PlayerBase player ) //  PlayerBase.c 
	{	
		if ( player.IsAlive() ) 	// Do not log uncon stop for dead players
		{
			m_PlayerPrefix = this.GetPlayerPrefix( player ,  player.GetIdentity() );
			
			LogPrint( m_PlayerPrefix + " regained consciousness" );		
		}
	}
		
	void OnPlacementComplete( Man player, ItemBase item ) // ItemBase.c
	{
		if ( m_PlacementFilter == 1 )
		{		
			m_Source = PlayerBase.Cast( player ); 
			m_PlayerPrefix = this.GetPlayerPrefix( m_Source , m_Source.GetIdentity() );		
			m_DisplayName = item.GetDisplayName();
			
			if ( m_DisplayName == "" )
			{
				LogPrint( m_PlayerPrefix + " placed unknown object" );
			} 
			else
			{
				LogPrint( m_PlayerPrefix + " placed " + m_DisplayName );
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
			
			m_PlayerPrefix = this.GetPlayerPrefix( action_data.m_Player , action_data.m_Player.GetIdentity() );
			
			LogPrint( m_PlayerPrefix + m_Message );
		}	
	}
	
	void Suicide( PlayerBase player )  // EmoteManager.c 
	{
		m_PlayerPrefix = this.GetPlayerPrefix( player ,  player.GetIdentity() );
		
		LogPrint( m_PlayerPrefix + " committed suicide" );
	}
	
	void BleedingOut( PlayerBase player )  // Bleeding.c
	{
		m_PlayerPrefix = this.GetPlayerPrefix( player ,  player.GetIdentity() );
		
		LogPrint( m_PlayerPrefix + " bled out" );
	}
	
	void PlayerList()
	{
		GetGame().GetPlayers( m_PlayerArray );
		
		if ( m_PlayerArray.Count() != 0 )
		{	
			LogPrint( "##### PlayerList log: " + m_PlayerArray.Count().ToString() + " players" );
						
			foreach ( Man player: m_PlayerArray )
			{
				m_Player = PlayerBase.Cast(player);
				m_PlayerPrefix = this.GetPlayerPrefix( m_Player ,  m_Player.GetIdentity() );
				
				LogPrint( m_PlayerPrefix );							
			}
			
			LogPrint( "#####" );
		}
	}
	
	void DirectAdminLogPrint(string str)
	{
		LogPrint(str);
	}
}