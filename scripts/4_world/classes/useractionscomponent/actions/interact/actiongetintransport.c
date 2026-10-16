class ActionGetInTransport: ActionBase
{
	void ActionGetInTransport()
	{
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_Text = "#get_in_vehicle";
	}


	override void CreateConditionComponents()  
	{
		m_ConditionItem 	= new CCINone;
		m_ConditionTarget	= new CCTCursorNoRuinCheck;
	}

	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}
	
	override bool HasProgress()
	{
		return false;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{

		if (!target)
			return false;

		Transport transport	= null;
		if (!Class.CastTo(transport, target.GetObject()))
			return false;
		
		if (!transport.CanGetIn())
			return false;
		
		if (player.GetItemInHands() && player.GetItemInHands().IsHeavyBehaviour())
			return false;

		if (player.GetCommand_Vehicle())
			return false;

		int componentIndex	= target.GetComponentIndex();
		int crewIndex 		= transport.CrewPositionIndex(componentIndex);
		if (crewIndex < 0)
			return false;

		Human crew = transport.CrewMember(crewIndex);
		if (crew)
			return false;
		
		if (!transport.CrewCanGetThrough(crewIndex) || !transport.IsAreaAtDoorFreeDiag(crewIndex))
			return false;
		
		array<string> selections = new array<string>();
		transport.GetActionComponentNameList(componentIndex, selections);
		int selectionCount = selections.Count();
		vector playerPosition = player.GetPosition();
		
		for (int i = 0; i < selectionCount; ++i)
		{
			if (transport.CanReachSeatFromDoors(selections[i], playerPosition, 1.0))
				return true;
		}

		return false;
	}

	override void Start(ActionData action_data)
	{
		super.Start(action_data);

		Transport transport = Transport.Cast(action_data.m_Target.GetObject());
		int componentIndex = action_data.m_Target.GetComponentIndex();
		int crewIndex = transport.CrewPositionIndex(componentIndex);

		float direction = 0;
		int seat;
		if (transport.HasDirectionalInOutAction())
		{
			vector fwd = transport.GetDirection();
			fwd[1] = 0;
			fwd.Normalize();
	
			vector playerPosition = action_data.m_Player.GetPosition();
			vector toTarget = transport.GetPosition() - playerPosition;
			toTarget[1] = 0;
			toTarget.Normalize();
	
			vector right = Vector(fwd[2], 0.0, -fwd[0]);
	
			float side = vector.Dot(toTarget, right);
	
			if (side < 0.0)
			    direction = 1;
		
			seat = transport.GetSeatAnimationTypeDirectional(crewIndex, direction);
		}
		else
			seat = transport.GetSeatAnimationType(crewIndex);

		HumanCommandVehicle vehCommand = action_data.m_Player.StartCommand_Vehicle(transport, crewIndex, seat);
		if (vehCommand)
		{
			vehCommand.SetVehicleType(transport.GetAnimInstance());

			GetDayZGame().GetBacklit().OnEnterCar();
		}
	}

	override bool CanBeUsedInRestrain()
	{
		return true;
	}
	
	override void OnUpdate(ActionData action_data)
	{
		if (action_data.m_State == UA_START)
		{
			if (!action_data.m_Player.GetCommand_Vehicle() || !action_data.m_Player.GetCommand_Vehicle().IsGettingIn())
			{
				End(action_data);
			}
		}
	}
	
	override int GetActionCategory()
	{
		return AC_INTERACT;
	}
	
	override void OnEnd(ActionData action_data)
	{
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
			if (g_Game.AddInventoryJunctureEx(action_data.m_Player, action_data.m_Player, il, true, 10000, action_data))
			{
				accepted = true;
				action_data.m_ReservedInventoryLocations.Insert(il);
			}
		}
		
		return accepted;
	}
};
