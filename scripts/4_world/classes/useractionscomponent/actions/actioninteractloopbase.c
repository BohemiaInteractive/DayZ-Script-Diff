class ActionInteractLoopBaseCB : ActionInteractBaseCB
{
	
	bool CancelCondition()
	{
		if ( !m_ActionData )
		{
			return DefaultCancelCondition();
		}
		//SetCommand(DayZPlayerConstants.CMD_ACTIONINT_ACTIONLOOP);
		//Print("cancel condition enabled: " + GetState().ToString() );
		if ( !m_Interrupted && (GetState() == STATE_LOOP_LOOP || GetState() == STATE_LOOP_LOOP2) )
		{	
			AnimatedActionBase action = AnimatedActionBase.Cast(m_ActionData.m_Action);
			action.Do(m_ActionData,m_ActionData.m_State);
		}
		return DefaultCancelCondition(); 
	}
	
	override void OnAnimationEvent(int pEventID)	
	{
	}
	
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAInteract;
	}
	
	override void InitActionComponent()
	{
		super.InitActionComponent();
		EnableCancelCondition(true);
	}
	
};

class ActionInteractLoopBase : ActionInteractBase
{	
	void ActionInteractLoopBase() 
	{
		m_CallbackClass = ActionInteractLoopBaseCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
	}
};