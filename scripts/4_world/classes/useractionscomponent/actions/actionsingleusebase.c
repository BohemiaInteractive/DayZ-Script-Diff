class ActionSingleUseBaseCB : ActionBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CASingleUse;
	}

	override void OnAnimationEvent(int pEventID)	
	{
		if ( m_ActionData )
		{
			if ( !m_Interrupted && pEventID == UA_ANIM_EVENT && m_ActionData.m_Action ) 
			{
				AnimatedActionBase action = AnimatedActionBase.Cast(m_ActionData.m_Action);
				action.OnAnimationEvent(m_ActionData);
			}
		}
	}
	
	override void InitActionComponent()
	{
		m_Interrupted = false;
		m_Canceled = false;

		CreateActionComponent();
		if ( m_ActionData.m_ActionComponent )
		{ 
			m_ActionData.m_ActionComponent.Init(m_ActionData);
		}
		m_ActionData.m_State = UA_PROCESSING;
		RegisterAnimationEvent("ActionExec", UA_ANIM_EVENT);
		m_SoundObject = m_ActionData.m_Action.PlayActionSound(m_ActionData.m_Player);
		}	
	
	override void EndActionComponent()
	{
		m_ActionData.m_State = UA_FINISHED;
	}	
};

class ActionSingleUseBase : AnimatedActionBase
{
	void ActionSingleUseBase()
	{
		m_CallbackClass = ActionSingleUseBaseCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_PICKUP_HANDS;
	}

	override typename GetInputType()
	{
		return DefaultActionInput;
	} 

	override int GetActionCategory()
	{
		return AC_SINGLE_USE;
	}
};