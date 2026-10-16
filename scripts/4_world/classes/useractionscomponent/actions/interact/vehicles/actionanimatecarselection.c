class ActionAnimateCarSelection : ActionInteractBase
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
		m_ConditionItem		= new CCINone();
		m_ConditionTarget	= new CCTNone();
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

		CarScript car = CarScript.Cast(target.GetObject());
		if (!car)
			return false;

		TStringArray selections = new TStringArray();
		car.GetActionComponentNameList(target.GetComponentIndex(), selections);

		foreach (string selection : selections)
		{
			m_AnimSource = car.GetAnimSourceFromSelection(selection);
			if (m_AnimSource != "")
				return true;
		}

		return false;
	}
	
	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);
		
		Entity targetEnt;
		if (Class.CastTo(targetEnt, action_data.m_Target.GetObject()))
		{
			if (targetEnt.GetAnimationPhase(m_AnimSource) > 0.5)
				targetEnt.SetAnimationPhase(m_AnimSource, 0.0);

			if (targetEnt.GetAnimationPhase(m_AnimSource) <= 0.5)
				targetEnt.SetAnimationPhase(m_AnimSource, 1.0);
		}
	}
	
	override void OnEndServer(ActionData action_data)
	{
		super.OnEndServer(action_data);
	}
}
