class ActionInjectTarget: ActionSingleUseBase
{
	void ActionInjectTarget()
	{
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_MEDIUM;
		
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_INJECTIONTARGET;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "#inject_target";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTMan(UAMaxDistances.DEFAULT);
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
	
	override void ApplyModifiers( ActionData action_data )
	{
		action_data.m_MainItem.OnApply( PlayerBase.Cast(action_data.m_Target.GetObject()));
		//PlayerBase ntarget = PlayerBase.Cast( action_data.m_Target.GetObject() );
		//ntarget.GiveShock(100);
	}
};