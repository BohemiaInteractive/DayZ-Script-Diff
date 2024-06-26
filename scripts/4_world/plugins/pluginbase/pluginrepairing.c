class PluginRepairing extends PluginBase
{
	bool Repair(PlayerBase player, ItemBase repair_kit, Object item, float specialty_weight, string damage_zone = "", bool use_kit_qty = true)
	{
		switch (item.GetHealthLevel(damage_zone))
		{
			case GameConstants.STATE_PRISTINE:
				break;
			case GameConstants.STATE_RUINED:
				#ifdef DEVELOPER
				Debug.Log("repairing from GameConstants.STATE_RUINED");
				#endif
				CalculateHealth(player, repair_kit, item, specialty_weight, damage_zone, use_kit_qty);
				break;
			case GameConstants.STATE_WORN:
				if (CanRepairToPristine(player) || CanBeRepairedToPristine(item))
				{
					CalculateHealth(player, repair_kit, item, specialty_weight,/* GameConstants.DAMAGE_PRISTINE_VALUE,*/ damage_zone, use_kit_qty);
				}
				break;
			default:
				CalculateHealth(player, repair_kit, item, specialty_weight, damage_zone, use_kit_qty);
				break;
		}

		return true;
	}

	void CalculateHealth(PlayerBase player, ItemBase kit, Object item, float specialty_weight, string damage_zone = "", bool use_kit_qty = true)
	{
		EntityAI entity;
		Class.CastTo(entity,item);
		
		if (entity != null)
		{
			entity.SetAllowDamage(true);
		}
		
		int health_levels_count = item.GetNumberOfHealthLevels(damage_zone);
		float kit_repair_cost_adjusted; //used with specialty_weight, disconnected
		float new_quantity;
		
		int target_level = Math.Clamp(item.GetHealthLevel(damage_zone) - 1, 0, health_levels_count - 1);
		float health_coef;
		if (!CanRepairToPristine(player) && !CanBeRepairedToPristine(item))
		{
			target_level = Math.Clamp(target_level, GameConstants.STATE_WORN, health_levels_count - 1);
		}
		health_coef = item.GetHealthLevelValue(target_level,damage_zone);
		
		//handles kit depletion; TODO: move to separate method.
		if (kit && kit.ConfigGetInt("repairKitType"))
		{
			bool kit_has_quantity = kit.HasQuantity();
			float cur_kit_quantity = kit.GetQuantity();
			float kit_repair_cost_per_level = GetKitRepairCost(kit, item);
			
			if (cur_kit_quantity > kit_repair_cost_per_level)
			{
				kit_repair_cost_adjusted = kit_repair_cost_per_level; //TODO: removed speciality weight for now, it should affect speed only (?).
				//kit_repair_cost_adjusted = player.GetSoftSkillsManager().SubtractSpecialtyBonus(kit_repair_cost_per_level, specialty_weight);
				kit_repair_cost_adjusted = Math.Clamp(kit_repair_cost_adjusted, 0, 100);
				if (use_kit_qty)
				{
					new_quantity = kit.GetQuantity() - kit_repair_cost_adjusted;
					kit.SetQuantity(new_quantity);
				}
			}
			else if (!kit_has_quantity) //"kit" without quantity (hammers and such) for your every day repairing needs
			{
			}
			else
			{
				if (use_kit_qty)
				{
					kit.SetQuantity(0);
				}
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

	bool CanRepair(ItemBase repair_kit, Object item, string damage_zone = "")
	{
		int state = item.GetHealthLevel(damage_zone);
		
		if (state != GameConstants.STATE_RUINED && (item.CanBeRepairedToPristine() && state >= GameConstants.STATE_WORN) || (!item.CanBeRepairedToPristine() && state >= GameConstants.STATE_DAMAGED))
		{
			int repair_kit_type = repair_kit.ConfigGetInt("repairKitType");
			if (!repair_kit_type) //outside of regular repair kit logic for some reason, bypass check
			{
				return true;
			}
			
			array<int> repairable_with_types = new array<int>;
			item.ConfigGetIntArray("repairableWithKits", repairable_with_types);	
			
			for (int i = 0; i < repairable_with_types.Count(); i++)
			{
				int repairable_with_type = repairable_with_types.Get(i);
				
				if (IsRepairValid(repair_kit_type, repairable_with_type))
				{
					return true;
				}
			}
		}
		return false;
		
	}
	
	private bool IsRepairValid(int repair_kit_type, int repairable_with_type)
	{
		if (repair_kit_type > 0 && repairable_with_type > 0)
		{
			return repair_kit_type == repairable_with_type;
		}
		
		return false;
	}

	//! Player can repair items to 100%; currently unused
	private bool CanRepairToPristine(PlayerBase player)
	{
		return false; 
	}
	
	//! Item can be repaired to 100%
	private bool CanBeRepairedToPristine(Object item)
	{
		return item.CanBeRepairedToPristine();
	}
	
	private float GetKitRepairCost(ItemBase repair_kit, Object item)
	{
		array<int> allowedRepairKitTypes 	= new array<int>();	
		array<float> repairKitCosts 		= new array<float>();
		
		item.ConfigGetIntArray("repairableWithKits", allowedRepairKitTypes);
		item.ConfigGetFloatArray("repairCosts", repairKitCosts);
		
		int repairKitType = repair_kit.ConfigGetInt("repairKitType");
		
		foreach (int i, int allowedKitType : allowedRepairKitTypes)
		{
			if (allowedKitType == repairKitType)
			{
				return repairKitCosts.Get(i);
			}
		} 
					
		return 0;
	}
}
