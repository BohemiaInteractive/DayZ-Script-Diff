class PluginRepairing extends PluginBase
{
	bool Repair(PlayerBase player, ItemBase repair_kit, Object item, float specialty_weight, string damage_zone = "", bool use_kit_qty = true)
	{
		switch ( item.GetHealthLevel(damage_zone) ) 
		{
			case GameConstants.STATE_PRISTINE:
				break;
			case GameConstants.STATE_RUINED:
#ifdef DEVELOPER
				Print("PluginRepairing - repairing from GameConstants.STATE_RUINED");
#endif
				CalculateHealth( player, repair_kit, item, specialty_weight, damage_zone, use_kit_qty );
				break;
			case GameConstants.STATE_WORN:
				if( CanRepairToPristine( player ) || CanBeRepairedToPristine( item ) )
				{
					CalculateHealth( player, repair_kit, item, specialty_weight,/* GameConstants.DAMAGE_PRISTINE_VALUE,*/ damage_zone, use_kit_qty );
				}
				break;
			default:
				CalculateHealth( player, repair_kit, item, specialty_weight, damage_zone, use_kit_qty );
				break;
		}

		return true;
	}

	void CalculateHealth( PlayerBase player, ItemBase kit, Object item, float specialty_weight, string damage_zone = "", bool use_kit_qty = true )
	{
		EntityAI entity;
		Class.CastTo(entity,item);
		
		if (entity != null)
		{
			//Print("" + item + " - damage allowed");
			entity.SetAllowDamage(true);
		}
		
		bool kit_has_quantity = kit.HasQuantity();
		int health_levels_count = item.GetNumberOfHealthLevels(damage_zone);
		float cur_kit_quantity = kit.GetQuantity();
		float kit_repair_cost_per_level = GetKitRepairCost( kit, item );
		float kit_repair_cost_adjusted; //used with specialty_weight, disconnected
		float new_quantity;
		
		int target_level = Math.Clamp(item.GetHealthLevel(damage_zone) - 1, 0, health_levels_count - 1);
		float health_coef;
		if ( !CanRepairToPristine( player ) && !CanBeRepairedToPristine( item ) )
		{
			target_level = Math.Clamp(target_level, GameConstants.STATE_WORN, health_levels_count - 1);
		}
		health_coef = item.GetHealthLevelValue(target_level,damage_zone);
		
		//handles kit depletion; TODO: move to separate method.
		if ( cur_kit_quantity > kit_repair_cost_per_level )
		{
			kit_repair_cost_adjusted = kit_repair_cost_per_level; //TODO: removed speciality weight for now, it should affect speed only (?).
			//kit_repair_cost_adjusted = player.GetSoftSkillsManager().SubtractSpecialtyBonus( kit_repair_cost_per_level, specialty_weight );
			kit_repair_cost_adjusted = Math.Clamp( kit_repair_cost_adjusted, 0, 100 );
			if(use_kit_qty)
			{
				new_quantity = kit.GetQuantity() - kit_repair_cost_adjusted;
				kit.SetQuantity( new_quantity );
			}
		}
		else if (!kit_has_quantity) //"kit" without quantity (hammers and such) for your every day repairing needs
		{
		}
		else
		{
			if(use_kit_qty)
			{
				kit.SetQuantity( 0 );
			}
		}
		
		if (item.GetHealth01(damage_zone,"Health") < health_coef)
		{
			item.SetHealth01(damage_zone,"Health",health_coef);
		}
		
		if (entity != null)
		{
			entity.ProcessInvulnerabilityCheck(entity.GetInvulnerabilityTypeString());
		}
	}

	bool CanRepair( ItemBase repair_kit, Object item, string damage_zone = "" )
	{
		int state = item.GetHealthLevel(damage_zone);
		
		if ( state != GameConstants.STATE_RUINED && (item.CanBeRepairedToPristine() && state >= GameConstants.STATE_WORN) || (!item.CanBeRepairedToPristine() && state >= GameConstants.STATE_DAMAGED ) )
		{
			int repair_kit_type = repair_kit.ConfigGetInt( "repairKitType" );
			
			ref array<int> repairable_with_types = new array<int>;
			item.ConfigGetIntArray( "repairableWithKits", repairable_with_types );	
			
			for ( int i = 0; i < repairable_with_types.Count(); i++ )
			{
				int repairable_with_type = repairable_with_types.Get(i);
				
				if ( IsRepairValid( repair_kit_type, repairable_with_type ) )
				{
					return true;
				}
			}
		}
		return false;
		
	}
	
	private bool IsRepairValid(int repair_kit_type, int repairable_with_type)
	{
		if ( repair_kit_type > 0 && repairable_with_type > 0 )
		{
			if ( repair_kit_type == repairable_with_type )
			{
				return true;
			}
		}
		
		return false;
	}

	//! Player can repair items to 100%; currently unused
	private bool CanRepairToPristine( PlayerBase player )
	{
// temporary disabled
/*
		float specialty = player.GetStatSpecialty().Get();
		
		if ( specialty <= -0.5 || specialty >= 0.5 )
		{
			return true;
		}
		else
		{
			return false; 
		}
*/
		return false; 
	}
	
	//! Item can be repaired to 100%
	private bool CanBeRepairedToPristine( Object item )
	{
		return item.CanBeRepairedToPristine();
	}
	
	private float GetKitRepairCost( ItemBase repair_kit, Object item )
	{
		ref array<int> repair_kit_types = new array<int>;	
		ref array<float> repair_kit_costs = new array<float>;	
		
		item.ConfigGetIntArray( "repairableWithKits", repair_kit_types );
		item.ConfigGetFloatArray( "repairCosts", repair_kit_costs );
		
		int eligible_kit;
		int kit_in_hands = repair_kit.ConfigGetInt( "repairKitType" );
		
		for ( int i = 0; i < repair_kit_types.Count(); i++ )
		{
			eligible_kit = repair_kit_types.Get(i);
			
			if ( eligible_kit == kit_in_hands )
			{
				float repair_cost = repair_kit_costs.Get(i);
			}
		} 
					
		return repair_cost;
	}
}
