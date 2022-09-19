class ActionInjectSelf: ActionSingleUseBase
{
	void ActionInjectSelf()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_INJECTION;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_MEDIUM;
		m_Text = "#inject";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTSelf;
	}

	override bool HasTarget()
	{
		return false;
	}

	override void OnExecuteServer( ActionData action_data )
	{	
		if (action_data.m_MainItem)
		{
			ApplyModifiers(action_data);
			action_data.m_Player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );
		}
	}
	
	override void OnEndServer( ActionData action_data )
	{
		if (action_data.m_WasExecuted && action_data.m_MainItem )
		{
			action_data.m_MainItem.Delete();
		}
	}
	
	override void OnEndClient( ActionData action_data )
	{
		if (action_data.m_WasExecuted && action_data.m_MainItem )
		{
			action_data.m_MainItem.DeleteOnClient();
		}
	}
	
	override void ApplyModifiers( ActionData action_data )
	{
		action_data.m_MainItem.OnApply(action_data.m_Player);
		//action_data.m_Player.m_ModifiersManager.DeactivateModifier(eModifiers.MDF_HEART_ATTACK);
		
	}
};