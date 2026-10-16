class ActionAnimateMotorbikeSeat : ActionAnimateSeats
{
	void ActionAnimateMotorbikeSeat()
	{
		m_Text = "#move_seat";
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionItem		= new CCINone();
		m_ConditionTarget	= new CCTCursor(UAMaxDistances.SMALL);
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if(!target)
			return false;

		MotorbikeScript bike;
		if (Class.CastTo(bike, target.GetObject()))
		{
			if (bike.CrewMember(DayZPlayerConstants.VEHICLESEAT_DRIVER))
				return false;

			TStringArray selections = new TStringArray();
			bike.GetActionComponentNameList(target.GetComponentIndex(), selections, LOD.NAME_VIEW);

			vector playerPosition =  player.GetPosition();
			foreach (string selection : selections)
			{
				if (bike.CanReachSeatFromDoors(selection, playerPosition, 1.0))
				{
					m_AnimSource = bike.GetAnimSourceFromSelection(selection);
					if (m_AnimSource != "" && m_AnimSource == "SeatDriver")
						return true;
				}
			}
		}

		return false;
	}
	
	override bool CanBeUsedInVehicle()
	{
		return false;
	}
}
