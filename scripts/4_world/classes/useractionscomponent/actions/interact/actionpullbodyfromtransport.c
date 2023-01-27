class ActionPullBodyFromTransport: ActionInteractBase
{
	void ActionPullBodyFromTransport()
	{
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ALL;
		m_Text = "#pull_out_body";
	}

	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTNone;
	}

	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}
	
	override bool HasProgress()
	{
		return false;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		PlayerBase targetPlayer = PlayerBase.Cast(target.GetObject());
		if (!targetPlayer)
			return false;
		
		if (targetPlayer.IsAlive())
			return false;
		
		IEntity parent = targetPlayer.GetParent();
		if (!parent || !parent.IsInherited(Transport))
			return false;
		
		bool found = false;
		Transport transportTarget = Transport.Cast(targetPlayer.GetParent());
		for (int i = 0; i < transportTarget.CrewSize(); ++i)
		{
			if (transportTarget.CrewMember(i) == targetPlayer)
			{
				found = true;
				break;
			}
		}
		
		CarScript car = CarScript.Cast(transportTarget);
		return found && transportTarget.CrewCanGetThrough(i);
	}
	
	/*
	bool IsCarDoorClosed(CarScript car, int seatIdx)
	{
		if (car == null)
			return false;
		
		string doorName = car.GetDoorInvSlotNameFromSeatPos(seatIdx);
		return car.GetCarDoorsState( doorName ) == CarDoorState.DOORS_CLOSED;
	}
	*/
	
	override void OnStartServer( ActionData action_data )
	{
		super.OnStartServer( action_data );
		
		DayZPlayerImplement player;
		if (!Class.CastTo(player, action_data.m_Target.GetObject()))
		{
			return;
		}
		
		player.TriggerPullPlayerOutOfVehicle();
	}
	
	/*override bool CanBeUsedInVehicle()
	{
		return true;
	}*/
	
	override bool IsInstant()
	{
		return true;
	}
};
