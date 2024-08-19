class ActionAnimateCarSelection: ActionInteractBase
{
	string m_AnimSource = "";

	void ActionAnimateCarSelection()
	{
		m_CommandUID	= DayZPlayerConstants.CMD_ACTIONMOD_OPENDOORFW;
		m_StanceMask	= DayZPlayerConstants.STANCEMASK_ALL;
		m_Text = "#manipulate";
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionItem		= new CCINone;
		m_ConditionTarget	= new CCTNone;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!target)
		{
			return false;
		}

		if (!IsInReach(player, target, UAMaxDistances.DEFAULT))
		{
			return false;
		}

		Object targetObject = target.GetObject();
		Entity targetEnt = Entity.Cast(targetObject);

		array<string> selections = new array<string>();
		targetObject.GetActionComponentNameList(target.GetComponentIndex(), selections);

		CarScript car;
		if (Class.CastTo(car, targetEnt))
		{
			for (int i = 0; i < selections.Count(); i++)
			{
				m_AnimSource = car.GetAnimSourceFromSelection(selections[i]);
				if (m_AnimSource != "")
				{
					return true;
				}
			}
		}

		return false;
	}
	
	override void OnStartServer(ActionData action_data)
	{
		Entity targetEnt;
		if (Class.CastTo(targetEnt, action_data.m_Target.GetObject()))
		{
			if (targetEnt.GetAnimationPhase(m_AnimSource) > 0.5)
			{
				targetEnt.SetAnimationPhase(m_AnimSource, 0.0);
			}

			if (targetEnt.GetAnimationPhase(m_AnimSource) <= 0.5)
			{
				targetEnt.SetAnimationPhase(m_AnimSource, 1.0);
			}
		}
		
		CarScript car;
		if (Class.CastTo(car, action_data.m_Target.GetObject()))
		{
			car.ForceUpdateLightsStart();
		}
	}
	
	override void OnEndServer( ActionData action_data )
	{
		CarScript car;
		if (Class.CastTo(car, action_data.m_Target.GetObject()))
		{
			car.ForceUpdateLightsEnd();
		}
	}
};