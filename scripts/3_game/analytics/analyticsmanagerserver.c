class AnalyticsManagerServer
{
	const string STAT_DISTANCE 				= "dist";
	const string STAT_PLAYTIME 				= "playtime";
	const string STAT_PLAYERS_KILLED 		= "players_killed";
	const string STAT_INFECTED_KILLED 		= "infected_killed";
	const string STAT_LONGEST_SURVIVOR_HIT 	= "longest_survivor_hit";
	
	void OnPlayerConnect(Man player)
	{
		player.StatRegister(STAT_DISTANCE);
		player.StatRegister(STAT_PLAYTIME);
		player.StatRegister(STAT_PLAYERS_KILLED);
		player.StatRegister(STAT_INFECTED_KILLED);
		player.StatRegister(STAT_LONGEST_SURVIVOR_HIT);
	}
		
	void OnPlayerDisconnect(Man player)
	{
		player.StatUpdateByPosition(STAT_DISTANCE);
		player.StatUpdateByTime(STAT_PLAYTIME);
	}
	
	//Entity-Entity hit
	void OnEntityHit(EntityAI source, Man target)
	{
		if (source)
		{
			Man survivor = source.GetHierarchyRootPlayer();
			if (survivor && source.IsWeapon())
				OnPlayerToPlayerHit(survivor, target);
		}
	}
	
	protected void OnPlayerToPlayerHit(Man shooter, Man target)
	{
		float longestHitDistance = shooter.StatGet(STAT_LONGEST_SURVIVOR_HIT);
		float currentDistance = vector.Distance(shooter.GetPosition(), target.GetPosition());
		float distanceUpdate;
		
		if (longestHitDistance < currentDistance)
			distanceUpdate = currentDistance - longestHitDistance;
		
		shooter.StatUpdate(STAT_LONGEST_SURVIVOR_HIT, distanceUpdate);
	}
	
	//Entity-Entity kill
	void OnEntityKilled(Object killer, EntityAI target)
	{
		EntityAI killerEntity = EntityAI.Cast(killer);
		if (killerEntity)
		{
			Man killerSurvivor = killerEntity.GetHierarchyRootPlayer();
			if (killerSurvivor)
			{
				if (target.IsPlayer())
					OnPlayerKilled(killerSurvivor, target);
				else if (target.IsZombie())
					OnInfectedKilled(killerSurvivor, target);
			}
		}
	}
	
	protected void OnPlayerKilled(Man killer, EntityAI target)
	{
		killer.StatUpdate(STAT_PLAYERS_KILLED, 1);
	}
	
	protected void OnInfectedKilled(Man killer, EntityAI target)
	{
		killer.StatUpdate(STAT_INFECTED_KILLED, 1);
	}
}
