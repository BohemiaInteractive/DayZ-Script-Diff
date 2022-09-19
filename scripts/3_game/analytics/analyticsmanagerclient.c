class AnalyticsManagerClient
{
	static const int GEAR_COUNT = 3; 
	static string m_FullGear[GEAR_COUNT] = {"Shoulder","Melee","Back"};
	
	void RegisterEvents()
	{
		ClientData.SyncEvent_OnEntityKilled.Insert(Event_OnEntityKilled);
		ClientData.SyncEvent_OnPlayerIgnitedFireplace.Insert(Event_OnPlayerIgnitedFireplace);
	}
	
	void UnregisterEvents()
	{
		ClientData.SyncEvent_OnEntityKilled.Remove(Event_OnEntityKilled);
		ClientData.SyncEvent_OnPlayerIgnitedFireplace.Remove(Event_OnPlayerIgnitedFireplace);
	}
	
	//===================================
	// OnActionEat
	//===================================
	void OnActionEat()
	{
		Achievements.OnActionEat();
	}
	
	//===================================
	// OnActionDrink
	//===================================
	void OnActionDrink()
	{
		Achievements.OnActionDrink();
	}
	
	//===================================
	// OnActionCookedSteak - not implemented
	//===================================
	void OnActionCookedSteak()
	{
		Achievements.OnCookedSteak();
	}
	
	//===================================
	// OnActionFinishedShaveSelf
	//===================================
	void OnActionFinishedShaveSelf()
	{
		Achievements.OnActionShave();
	}
	
	//===================================
	// OnActionFinishedGutDeer
	//===================================
	void OnActionFinishedGutDeer()
	{
		Achievements.OnActionGutDeer();
	}
	
	//===================================
	// OnActionRestrain
	//===================================
	void OnActionRestrain()
	{		
		Achievements.OnActionHandcuff();
	}
	
	//===================================
	// OnActionBandageTarget
	//===================================
	void OnActionBandageTarget()
	{	
		Achievements.OnActionMedsSurvivor();
	}
	
	//===================================
	// OnItemAttachedAtPlayer
	//===================================
	void OnItemAttachedAtPlayer(EntityAI item, string slot_name)
	{
		bool weapon_present;
		bool melee_present;
		bool backpack_present;
		HumanInventory inventory;

		if ( GetDayZGame().GetGameState() != DayZGameState.IN_GAME )
		{
			return;
		}
		
		Man player = GetGame().GetPlayer();
		if (!player)
		{
			return;
		}
		
		inventory = player.GetHumanInventory();
			
		if ( player && inventory )
		{
			for ( int i = 0; i < GEAR_COUNT; ++i )
			{
				int slot_id = InventorySlots.GetSlotIdFromString(m_FullGear[i]);
				EntityAI att_item = inventory.FindAttachment( slot_id ); // Boris V [27.2.2019]: Consider using player.GetItemOnSlot(m_FullGear[i]) instead.
					
				if ( !att_item )
				{
					//Print("index: "+ i +" slot_id: "+ slot_id +" = "+ att_item + " EMPTY");
					continue;
				}
				
				//checks for firearm
				if (att_item.IsWeapon())
					weapon_present = true;
				//checks for melee weapon
				else if (!att_item.IsWeapon() && att_item.GetInventory().HasInventorySlot(InventorySlots.GetSlotIdFromString("Melee")))
					melee_present = true;
				//checks for backpack
				else if (!att_item.IsWeapon() && att_item.GetInventory().HasInventorySlot(InventorySlots.GetSlotIdFromString("Back")))
					backpack_present = true;
				//Print("index: "+ i +" slot_id: "+ slot_id +" = "+ att_item + " ATTACHED");
			}
				
			//separate check for hand slot; TODO remove duplicates
			att_item = inventory.GetEntityInHands();
			if ( att_item )
			{
				//checks for firearm
				if (att_item.IsWeapon())
					weapon_present = true;
				//checks for melee weapon
				else if (!att_item.IsWeapon() && att_item.GetInventory().HasInventorySlot(InventorySlots.GetSlotIdFromString("Melee")) )
					melee_present = true;
				//checks for backpack
				else if (!att_item.IsWeapon() && att_item.GetInventory().HasInventorySlot(InventorySlots.GetSlotIdFromString("Back")))
					backpack_present = true;
			}
				
			if (weapon_present && melee_present && backpack_present)
			{
				//Print("---EAchievementActionId.ACTION_EQUIP_GEAR");
				Achievements.OnEquippedFullGear();
			}
		}
	}
	
	//===================================
	// Event_OnPlayerIgnitedFireplace
	//===================================
	void Event_OnPlayerIgnitedFireplace( EFireIgniteType ignite_type )
	{
		switch ( ignite_type )
		{
			case EFireIgniteType.Matchbox:
			{
				Achievements.OnActionIgniteMatchbox();
				break;
			}
			case EFireIgniteType.Roadflare:
			{
				Achievements.OnActionIgniteRoadflare();
				break;
			}
			case EFireIgniteType.HandDrill:
			{
				Achievements.OnActionIgniteDrill();
				break;
			}
		}
	}
	
	//===================================
	// Event_OnEntityKilled
	//===================================
	void Event_OnEntityKilled(EntityAI victim, EntityAI killer, EntityAI source, bool is_headshot)
	{
		if ( killer != null && killer.IsPlayer() && killer.GetID() == GetGame().GetPlayer().GetID() )
		{
			Achievements.OnPlayerKilled(victim, killer, source, is_headshot);
		}
	}
}