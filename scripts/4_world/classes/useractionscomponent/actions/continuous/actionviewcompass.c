class ActionRaiseAndViewCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime( -1.0 );
		//EnableStateChangeCallback();
	}

	override void OnStateChange(int pOldState, int pCurrentState)
	{
		//Print("pOldState = " + pOldState);
		//Print("pCurrentState = " + pCurrentState);
		
		if (pOldState != STATE_LOOP_LOOP)
			return;
		
		if (m_ActionData.m_Action && ActionViewOptics.Cast(m_ActionData.m_Action))
		{
			//ActionViewOptics.Cast(m_ActionData.m_Action).ExitOptics( ItemOptics.Cast(m_ActionData.m_MainItem), m_ActionData.m_Player );
		}
	}
};

class ActionViewCompass : ActionContinuousBase
{
	void ActionViewCompass()
	{
		m_CallbackClass = ActionRaiseAndViewCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_OPENITEM_ONCE;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_OPENITEM_ONCE;
		m_Text = "#use_compas";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTNone;
	}
	
	override bool HasProgress()
	{
		return false;
	}

	override bool HasTarget()
	{
		return false;
	}

	override bool HasProneException()
	{
		return true;
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		return item.IsOpen();
	}
	
	/*override void OnCompleteServer( ActionData action_data )
	{
		if (action_data.m_Player)
		{
			string message = Rangefinder.Cast( action_data.m_MainItem ).DoMeasurement(action_data.m_Player);
			SendMessageToClient(action_data.m_Player,message);
		}
		
		action_data.m_Player.GetSoftSkillManager().AddSpecialty( m_SpecialtyWeight );
	}*/
}