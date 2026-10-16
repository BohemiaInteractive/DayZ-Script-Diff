class ActionPickUpMotorbike: ActionInteractBase
{
	void ActionPickUpMotorbike()
	{
		m_CommandUID 	= DayZPlayerConstants.CMD_ACTIONMOD_OPENDOORFW;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_Text = "#STR_PickUpBike";
	}

	override void CreateConditionComponents()  
	{
		m_ConditionItem 	= new CCINone();
		m_ConditionTarget	= new CCTCursor();
	}
	
	override bool HasProgress()
	{
		return false;
	}

	override bool IsLockTargetOnUse()
	{
		return false;
	}
	
	override bool IsInstant()
	{
		return true;
	}
	
	override bool CanBeUsedInVehicle()
	{
		return false;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		Motorbike trans	= null;

		if (!target)
		{
			return false;
		}

		if (!Class.CastTo(trans, target.GetObject()))
		{
			return false;
		}
		
		if (player.GetItemInHands() && player.GetItemInHands().IsHeavyBehaviour())
		{
			return false;
		}

		int componentIndex	= target.GetComponentIndex();
		int crew_index 		= trans.CrewPositionIndex(componentIndex);

		// not targetting a seat
		if (crew_index >= 0) 
		{
			return false;
		}

		return trans.CanPickUp();
	}

	override void Start(ActionData action_data)
	{
		super.Start( action_data );
		
		Motorbike trans = Motorbike.Cast(action_data.m_Target.GetObject());
		if (trans)
		{
			trans.PickUp();
		}
	}
		
	override int GetActionCategory()
	{
		return AC_INTERACT;
	}
	
	override bool AddActionJuncture(ActionData action_data)
	{
		Transport trans = Transport.Cast(action_data.m_Target.GetObject());
		bool accepted = false;
		int nextSeat = trans.CrewPositionIndex( action_data.m_Target.GetComponentIndex() );
		Transport transport = Transport.Cast(action_data.m_Target.GetObject());
		InventoryLocation il = new InventoryLocation;
		if (transport)
		{
			il.SetVehicle(transport, action_data.m_Player, nextSeat);
		
			//Lock target
			if (GetGame().AddInventoryJunctureEx(action_data.m_Player, action_data.m_Player, il, true, 10000))
			{
				accepted = true;
				action_data.m_ReservedInventoryLocations.Insert(il);
			}
		}
		
		return accepted;
	}
};
