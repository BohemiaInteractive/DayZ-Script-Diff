class AnalyticsManagerServer
{
	const string STAT_DISTANCE 				= "dist";
	const string STAT_PLAYTIME 				= "playtime";
	const string STAT_PLAYERS_KILLED 		= "players_killed";
	const string STAT_INFECTED_KILLED 		= "infected_killed";
	const string STAT_LONGEST_SURVIVOR_HIT 	= "longest_survivor_hit";
	
	void OnPlayerConnect( Man player )
	{
		player.StatRegister( STAT_DISTANCE );
		player.StatRegister( STAT_PLAYTIME );
		player.StatRegister( STAT_PLAYERS_KILLED );
		player.StatRegister( STAT_INFECTED_KILLED );
		player.StatRegister( STAT_LONGEST_SURVIVOR_HIT );

		//player.StatSyncToClient();
	}
		
	void OnPlayerDisconnect( Man player )
	{
		player.StatUpdateByPosition( STAT_DISTANCE );
		player.StatUpdateByTime( STAT_PLAYTIME );
		
		/*
		Print("");
		Print("-------------");
		Print( STAT_DISTANCE 				+ " " + player.StatGet( STAT_DISTANCE ).ToString() );
		Print( STAT_PLAYTIME 				+ " " + player.StatGet( STAT_PLAYTIME ).ToString() );
		Print( STAT_PLAYERS_KILLED 			+ " " + player.StatGet( STAT_PLAYERS_KILLED ).ToString() );
		Print( STAT_INFECTED_KILLED			+ " " + player.StatGet( STAT_INFECTED_KILLED ).ToString() );
		Print( STAT_LONGEST_SURVIVOR_HIT 	+ " " + player.StatGet( STAT_LONGEST_SURVIVOR_HIT ).ToString() );
		Print("-------------");
		Print("");
		*/
		
		//player.StatSyncToClient();
	}
	
	//Entity-Entity hit
	void OnEntityHit( EntityAI source, Man target )
	{
		if (source)
		{
			Man survivor = source.GetHierarchyRootPlayer();
			if ( survivor && source.IsWeapon() )
			{
				OnPlayerToPlayerHit( survivor, target );
			}
		}
	}
	
	protected void OnPlayerToPlayerHit( Man shooter, Man target )
	{
		//calculate distance
		float longest_hit_dist = shooter.StatGet( STAT_LONGEST_SURVIVOR_HIT );
		float current_dist = vector.Distance( shooter.GetPosition(), target.GetPosition() );
		float dist_update;
		
		if ( longest_hit_dist < current_dist )
		{
			dist_update = current_dist - longest_hit_dist;
		}
		
		/*
		Print("");
		Print("-------------");
		Print(shooter.GetDisplayName() + " hit " + target.GetDisplayName() + " from " + longest_hit_dist.ToString() + " m" );
		Print("-------------");
		Print("");
		*/
		
		//update stat
		shooter.StatUpdate( STAT_LONGEST_SURVIVOR_HIT, dist_update );
	}
	
	//Entity-Entity kill
	void OnEntityKilled( Object killer, EntityAI target )
	{
		EntityAI killer_entity = EntityAI.Cast( killer );
		if ( killer_entity )
		{
			Man killer_survivor = killer_entity.GetHierarchyRootPlayer();
			if ( killer_survivor )
			{
				if ( target.IsPlayer() )
				{
					OnPlayerKilled( killer_survivor, target );
				}
				else if ( target.IsZombie() )
				{
					OnInfectedKilled( killer_survivor, target );
				}
			}
		}
	}
	
	protected void OnPlayerKilled( Man killer, EntityAI target )
	{
		/*
		Print("");	
		Print("-------------");
		Print(killer.GetDisplayName() + " killed player " + target.GetDisplayName() );
		Print("-------------");
		Print("");
		*/
			
		//update stat
		killer.StatUpdate( STAT_PLAYERS_KILLED, 1 );
//		killer.StatSyncToClient();		
	}
	
	protected void OnInfectedKilled( Man killer, EntityAI target )
	{
		/*
		Print("");
		Print("-------------");
		Print(killer.GetDisplayName() + " killed infected " + target.GetDisplayName() );
		Print("-------------");
		Print("");
		*/
			
		//update stat
		killer.StatUpdate( STAT_INFECTED_KILLED, 1 );
//		killer.StatSyncToClient();		
	}
}