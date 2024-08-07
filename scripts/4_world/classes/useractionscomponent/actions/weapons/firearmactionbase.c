class FirearmActionBase : ActionBase
{	
	void FirearmActionBase();
	
	override int GetStanceMask(PlayerBase player)
	{
		return DayZPlayerConstants.STANCEMASK_ALL;
	}
	
	override int GetActionCategory()
	{
		return AC_SINGLE_USE;
	}

	override typename GetInputType()
	{
		return DefaultActionInput;
	}

	override bool ActionConditionContinue(ActionData action_data)
	{
		Weapon_Base wpn = Weapon_Base.Cast(action_data.m_MainItem);
		return !wpn.IsIdle();
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		Weapon_Base wpn = Weapon_Base.Cast(item);
		return wpn && wpn.CanProcessWeaponEvents() && !player.GetDayZPlayerInventory().IsProcessing();
	}
	
	override void Start(ActionData action_data)
	{
		super.Start(action_data);

		action_data.m_State = UA_PROCESSING;
	}
	
	override bool CanBePerformedFromInventory()
	{
		return true;
	}
	
	override bool CanBeUsedOnBack()
	{
		return true;
	}
	
	override bool CanBeUsedRaised()
	{
		return true;
	}

	override void OnUpdate(ActionData action_data)
	{
		super.OnUpdate(action_data);

		Weapon_Base wpn = Weapon_Base.Cast(action_data.m_MainItem);
		
		if (!wpn || action_data.m_Player.GetItemInHands() != wpn || !action_data.m_Player.GetWeaponManager().IsRunning())
		{
			End(action_data);
		}
	}
	
	override float GetProgress(ActionData action_data)
	{
		return -1;
	}
	
	override bool AddActionJuncture(ActionData action_data)
	{
		bool accepted;
		accepted = super.AddActionJuncture(action_data);
		
		Weapon_Base wpn = Weapon_Base.Cast(action_data.m_MainItem);
		
				
		if (wpn)
		{
			Magazine mag = wpn.GetMagazine(wpn.GetCurrentMuzzle());
			
			if (mag)
			{
				InventoryLocation magIl = new InventoryLocation();
				mag.GetInventory().GetCurrentInventoryLocation(magIl);
			
				if (!GetGame().AddInventoryJunctureEx(action_data.m_Player, mag, magIl, true, 10000))
				{
						accepted = false;
						ClearActionJuncture(action_data);
				}
				else
				{
					action_data.m_ReservedInventoryLocations.Insert(magIl);
				}
			}
		}
		return accepted;
	}
}
