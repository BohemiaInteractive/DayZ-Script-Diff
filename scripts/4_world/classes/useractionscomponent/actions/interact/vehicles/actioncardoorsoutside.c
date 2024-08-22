class ActionCarDoorsOutside: ActionInteractBase
{	
	protected int m_CommandUIDPerCrewIdx[4];
	protected bool m_IsOpening = true;
	
	void ActionCarDoorsOutside()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_OPENDOORFW;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ALL;
		m_LockTargetOnUse = false;
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem		= new CCINone;
		m_ConditionTarget	= new CCTNone;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{		
		CarScript car = null;
		
		//! reach check from outside of car
		if (!IsInReach(player, target, UAMaxDistances.DEFAULT))
		{
			return false;
		}

		//! player is outside of vehicle
		if (Class.CastTo(car, target.GetParent()))
		{
			array<string> selections = new array<string>();
				
			CarDoor carDoor = CarDoor.Cast(target.GetObject());
			if (carDoor)
			{
				carDoor.GetActionComponentNameList(target.GetComponentIndex(), selections);
				
				string animSource	= "";
				
				for (int i = 0; i < selections.Count(); i++)
				{
					animSource = car.GetAnimSourceFromSelection(selections[i]);
					if (animSource != "")
					{
						int idx = car.GetSeatIndexFromDoor(animSource);
						if (idx != -1 && !car.IsAreaAtDoorFree(idx))
						{
							return false;	//! player is looking at one of the doors, can't open if obstructed
						}
							
						//! if player is in car and cannot reach doors
						m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_OPENDOORFW;
							
						float animationPhase = car.GetAnimationPhase(animSource);	
						//! is in reach, should open the door
						return (m_IsOpening && animationPhase <= 0.5 ) || ( !m_IsOpening && animationPhase > 0.5);
					}
				}
			}
		}
		
		return false;
	}
	
	override void OnStartServer( ActionData action_data )
	{
		super.OnStartServer(action_data);
		
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
		
		CarScript car = CarScript.Cast(action_data.m_Target.GetParent());
		if (car)
		{
			array<string> selections = new array<string>();				
			CarDoor carDoor = CarDoor.Cast(action_data.m_Target.GetObject());
			if (carDoor)
			{
				carDoor.GetActionComponentNameList(action_data.m_Target.GetComponentIndex(), selections);
				for (int i = 0; i < selections.Count(); i++)
				{
					animSource = car.GetAnimSourceFromSelection(selections[i]);
					if (animSource != "")
					{
						break;
					}
				}
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
		super.OnEndServer(action_data);
		
		CarScript car = CarScript.Cast(action_data.m_Target.GetParent());
		if (car)
		{
			car.ForceUpdateLightsEnd();
		}
	}
	
	//--------- Not used for this class
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
	//-----------------------------------
};