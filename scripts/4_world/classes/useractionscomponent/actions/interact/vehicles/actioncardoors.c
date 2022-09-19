class ActionCarDoors: ActionInteractBase
{	
	protected int m_CommandUIDPerCrewIdx[4];
	protected bool m_IsOpening = true;
	// --- Backwards compatibility 1.09 -> 1.10 (But please do not use these)
	CarScript m_Car = null;
	string m_AnimSource = "";
	// ---
	
	void ActionCarDoors()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_OPENDOORFW;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ALL;
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem 	= new CCINone;
		m_ConditionTarget	= new CCTNone;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{		
		CarScript car 		= null;
		string animSource	= "";
		
		//! player inside vehicle
		if (player && player.GetCommand_Vehicle())
		{
			if (Class.CastTo(car, player.GetCommand_Vehicle().GetTransport()))
			{
				int crewIdx = car.CrewMemberIndex(player);
				
				//! crewIdx sanity checks and see if there is a door
				if (crewIdx < 0 || crewIdx > 3 || car.GetCarDoorsState(car.GetDoorInvSlotNameFromSeatPos(crewIdx)) == CarDoorState.DOORS_MISSING)
				{
					return false;
				}

				animSource = car.GetAnimSourceFromSelection(car.GetDoorSelectionNameFromSeatPos(crewIdx));
				
				//! see if door is in reach
				if (!car.CanReachDoorsFromSeat(animSource, crewIdx) || !car.IsAreaAtDoorFree(crewIdx))
				{
					return false;
				}
				
				m_CommandUID = m_CommandUIDPerCrewIdx[crewIdx];
				
				float animationPhaseInside = car.GetAnimationPhase(animSource);				
				return (m_IsOpening && animationPhaseInside <= 0.5) || (!m_IsOpening && animationPhaseInside > 0.5);
			}
		}

		return false;
	}

	override void OnStartServer(ActionData action_data)
	{
		PlayerBase player = action_data.m_Player;
		
		float phase;
		if (m_IsOpening)
		{
			phase = 1.0;
		}
		else
		{
			phase = 0.0;
		}
		
		string animSource = "";
		
		CarScript car;
		if (player && player.GetCommand_Vehicle())
		{
			car = CarScript.Cast(player.GetCommand_Vehicle().GetTransport());
			if (car)
			{
				int crewIdx = car.CrewMemberIndex(player);
				animSource = car.GetAnimSourceFromSelection(car.GetDoorSelectionNameFromSeatPos(crewIdx));
			}
		}
		
		if (car)
		{
			car.ForceUpdateLightsStart();
			car.SetAnimationPhase(animSource, phase);
		}
	}
	
	override void OnEndServer(ActionData action_data)
	{
		CarScript car;
		PlayerBase player = action_data.m_Player;
		if (player && player.GetCommand_Vehicle())
		{
			car = CarScript.Cast(player.GetCommand_Vehicle().GetTransport());
		}

		if (car)
		{
			car.ForceUpdateLightsEnd();
		}
	}
	
	override void OnEnd(ActionData action_data)
	{
		if (m_Car || m_AnimSource.Length() > 0)
		{
			Error("[WARNING] m_Car and m_AnimSource are left for backwards compatibility to not crash mods that are using it, but should no longer be used.");
		}
	}
	
	override bool CanBeUsedInVehicle()
	{
		return true;
	}
	
	protected void FillCommandUIDPerCrewIdx(int crewIdx0, int crewIdx1, int crewIdx2, int crewIdx3)
	{
		m_CommandUIDPerCrewIdx[0] = crewIdx0;
		m_CommandUIDPerCrewIdx[1] = crewIdx1;
		m_CommandUIDPerCrewIdx[2] = crewIdx2;
		m_CommandUIDPerCrewIdx[3] = crewIdx3;
	}
	
	protected void FillCommandUIDPerCrewIdx(int evenCrewIdx0, int unevenCrewIdx1)
	{
		FillCommandUIDPerCrewIdx(evenCrewIdx0, unevenCrewIdx1, evenCrewIdx0, unevenCrewIdx1);
	}
};