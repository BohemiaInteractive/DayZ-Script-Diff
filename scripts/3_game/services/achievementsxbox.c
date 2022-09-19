enum EAchievementError
{
	ERR_OK,
	ERR_NOT_FOUND, 
	ERR_BAD_DATA,
	ERR_NOT_IMPLEMENTED,
	ERR_UNKNOWN,
};

enum EAchievementTargetId
{
	TARGET_SURVIVOR,
	TARGET_INFECTED,
};

enum EAchievementRankId
{
	RANK_SURVIVOR,				//!< must be TARGET_SURVIVOR
	RANK_INFECTED,				//!< must be TARGET_INFECTED
	RANK_INFECTED_SOLDIER,		//!< must be TARGET_INFECTED
};

enum EAchievementRangeId
{
	RANGE_OTHER,
	RANGE_MELEE,
	RANGE_RANGED,
};

enum EAchievementHitId
{
	HIT_OTHER,
	HIT_HEADSHOT,
};

enum EAchievementActionId
{
	ACTION_EAT,
	ACTION_DRINK,
	ACTION_EQUIP_GEAR,
	ACTION_COOK_STEAK,
	ACTION_IGNITE_FIRE_MATCHBOX,
	ACTION_IGNITE_FIRE_ROAD_FLARE,
	ACTION_IGNITE_FIRE_HAND_DRILL,
	ACTION_SHAVE,
	ACTION_GUT_DEER,
	ACTION_APPLY_MEDS_ON_SURVIVOR,
	ACTION_HANDCUFF_SURVIVOR,
};

class Achievements
{
	private void Achievements();
	private void ~Achievements();
	
	static proto EAchievementError SendEventAction(EAchievementActionId action_id);
	static proto EAchievementError SendEventKill(EAchievementTargetId target_id, EAchievementRankId rank_id, EAchievementRangeId range_id, EAchievementHitId hit_id, float distance);
	
	//===================================
	// OnActionEat
	//===================================
	static void OnActionEat()
	{
		CheckError( SendEventAction( EAchievementActionId.ACTION_EAT ) );
	}
	
	//===================================
	// OnActionDrink
	//===================================
	static void OnActionDrink()
	{
		CheckError( SendEventAction( EAchievementActionId.ACTION_DRINK ) );
	}
	
	//===================================
	// OnEquipdFullGear
	//===================================
	static void OnEquippedFullGear()
	{
		CheckError( SendEventAction( EAchievementActionId.ACTION_EQUIP_GEAR ) );
	}
	
	//===================================
	// OnCookedSteak
	//===================================
	static void OnCookedSteak()
	{
		CheckError( SendEventAction( EAchievementActionId.ACTION_COOK_STEAK ) );
	}
	
	//===================================
	// OnActionIgniteMatchbox
	//===================================
	static void OnActionIgniteMatchbox()
	{
		CheckError( SendEventAction( EAchievementActionId.ACTION_IGNITE_FIRE_MATCHBOX ) );
	}
	
	//===================================
	// OnActionIgniteRoadflare
	//===================================
	static void OnActionIgniteRoadflare()
	{
		CheckError( SendEventAction( EAchievementActionId.ACTION_IGNITE_FIRE_ROAD_FLARE ) );
	}
	
	//===================================
	// OnActionIgniteDrill
	//===================================
	static void OnActionIgniteDrill()
	{
		CheckError( SendEventAction( EAchievementActionId.ACTION_IGNITE_FIRE_HAND_DRILL ) );
	}
	
	//===================================
	// OnActionShave
	//===================================
	static void OnActionShave()
	{
		CheckError( SendEventAction( EAchievementActionId.ACTION_SHAVE ) );
	}
	
	//===================================
	// OnActionGutDeer
	//===================================
	static void OnActionGutDeer()
	{
		CheckError( SendEventAction( EAchievementActionId.ACTION_GUT_DEER ) );
	}
	
	//===================================
	// OnActionMedsSurvivor
	//===================================
	static void OnActionMedsSurvivor()
	{
		CheckError( SendEventAction( EAchievementActionId.ACTION_APPLY_MEDS_ON_SURVIVOR ) );
	}
	
	//===================================
	// OnActionHandcuff
	//===================================
	static void OnActionHandcuff()
	{
		CheckError( SendEventAction( EAchievementActionId.ACTION_HANDCUFF_SURVIVOR ) );
	}	
	
	//===================================
	// OnPlayerKilled
	//===================================
	static void OnPlayerKilled(EntityAI victim, EntityAI killer, EntityAI source, bool is_headshot)
	{		
		EAchievementTargetId	target_id		= EAchievementTargetId.TARGET_SURVIVOR;
		EAchievementRankId		target_rank_id	= EAchievementRankId.RANK_SURVIVOR;
		EAchievementRangeId		range_id		= EAchievementRangeId.RANGE_OTHER;
		EAchievementHitId		hit_id			= EAchievementHitId.HIT_OTHER;
		float 					distance		= 0;
		
		if ( victim.IsZombie() )
		{
			target_id = EAchievementTargetId.TARGET_INFECTED;
			target_rank_id = EAchievementRankId.RANK_INFECTED;
			
			if ( victim.IsZombieMilitary() )
			{
				target_rank_id = EAchievementRankId.RANK_INFECTED_SOLDIER;
			}
		}		
		
		if ( source )
		{
			if ( source.IsMeleeWeapon() )
			{
				range_id = EAchievementRangeId.RANGE_MELEE;
			}
			else if ( source.IsWeapon() && killer )
			{
				range_id = EAchievementRangeId.RANGE_RANGED;
				distance = vector.Distance( killer.GetPosition(), victim.GetPosition() );
				
				if ( is_headshot )
				{
					hit_id = EAchievementHitId.HIT_HEADSHOT;
				}
			}
		}
		
		CheckError( SendEventKill(target_id, target_rank_id, range_id, hit_id, distance) );
	}
	
	//-----------------------------------
	// CheckError
	//-----------------------------------
	private static void CheckError(EAchievementError error)
	{
		if ( error != EAchievementError.ERR_OK )
		{
			Print("Achievements: Cannot send achievement event. Error ID: " + error);
		}
	}
};

typedef Achievements AchievementsXbox;