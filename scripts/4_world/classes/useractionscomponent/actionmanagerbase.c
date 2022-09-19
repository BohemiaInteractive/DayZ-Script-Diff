typedef Param3<int, int, string> TSelectableActionInfo;

class TSelectableActionInfoArray extends array<ref TSelectableActionInfo>
{
	bool IsSameAs(TSelectableActionInfoArray other)
	{
		if( this.Count() != other.Count() )
		{
			return false;
		}
		
		for( int i = 0; i < Count(); ++i )
		{
			TSelectableActionInfo ai1 = this.Get(i);
			TSelectableActionInfo ai2 = other.Get(i);
			
			if( ai1.param2 != ai2.param2 )
			{
				return false;
			}
			
			if( ai1.param3 != ai2.param3 )
			{
				return false;
			}
		}
		return true;
	}
};

class ActionManagerBase
{
	PlayerBase									m_Player;
	
	protected ActionTarget						m_TestedActionTarget;
	protected ItemBase							m_TestedActionItem;
		
	protected ActionBase						m_PrimaryAction;
	protected ActionTarget						m_PrimaryActionTarget;
	protected ItemBase							m_PrimaryActionItem;
	protected ActionBase						m_SecondaryAction;
	protected ActionTarget						m_SecondaryActionTarget;
	protected ItemBase							m_SecondaryActionItem;
	bool										m_PrimaryActionEnabled;
	bool										m_SecondaryActionEnabled;
	bool										m_TertiaryActionEnabled;
	ref TSelectableActionInfoArray				m_SelectableActions;
	int                             			m_SelectedActionIndex;
	bool                            			m_SelectableActionsHasChanged;
	bool										m_Interrupted;
	static ref array<ref ActionBase>			m_ActionsArray;
	static ref map<typename, ActionBase>		m_ActionNameActionMap;
	protected bool								m_ActionWantEndRequest;
	protected bool								m_ActionInputWantEnd;
	protected bool								m_ActionsEnabled;
	protected bool								m_ActionsAvaibale;
		
//Pending actions waiting for acknowledgment
	protected int 					m_PendingActionAcknowledgmentID;
	
	protected ref ActionData		m_CurrentActionData;

	void ActionManagerBase(PlayerBase player)
	{
		m_Player = player;
		if ( m_Player )
		{
			m_SelectableActions = new TSelectableActionInfoArray;
			m_SelectedActionIndex = 0;
			m_SelectableActionsHasChanged = false;
			
			m_PendingActionAcknowledgmentID = -1;
			
			m_CurrentActionData = NULL;
			m_Interrupted = false;
			
			ActionConstructor ac = new ActionConstructor;
			if ( !m_ActionsArray )
			{
				ac.ConstructActions( m_ActionsArray, m_ActionNameActionMap );
			}
			m_ActionWantEndRequest = false;
			m_ActionInputWantEnd = false;
		}
		
		m_ActionsEnabled = true;
		m_ActionsAvaibale = true;
	}
	
	ActionBase GetRunningAction()
	{
		if ( m_CurrentActionData )
		{
			return m_CurrentActionData.m_Action;
		}
		return NULL;
	}
	
	ItemBase GetRunningActionMainitem()
	{
		if ( m_CurrentActionData )
			return m_CurrentActionData.m_MainItem;
		
		return null;
	}
	
	void EnableActions(bool enable)
	{
		m_ActionsEnabled = enable;
	}

	void Update(int pCurrentCommandID)
	{
		if (m_CurrentActionData)
		{
			if (m_Interrupted)
			{
				LocalInterrupt();
				m_Interrupted = false;
			}
			else if (m_CurrentActionData.m_State != UA_AM_PENDING && m_CurrentActionData.m_State != UA_AM_REJECTED && m_CurrentActionData.m_State != UA_AM_ACCEPTED)
			{
				m_CurrentActionData.m_Action.OnUpdate(m_CurrentActionData);
			}
		}
	}
	
	void OnSyncJuncture(int pJunctureID, ParamsReadContext pCtx)
	{
		int AcknowledgmentID;
		switch (pJunctureID)
		{
			case DayZPlayerSyncJunctures.SJ_ACTION_ACK_ACCEPT:
				pCtx.Read(AcknowledgmentID);
				if (m_CurrentActionData && AcknowledgmentID == m_PendingActionAcknowledgmentID)
					m_CurrentActionData.m_State = UA_AM_ACCEPTED;
				break;
			case DayZPlayerSyncJunctures.SJ_ACTION_ACK_REJECT:
				pCtx.Read(AcknowledgmentID);
				if (m_CurrentActionData && AcknowledgmentID == m_PendingActionAcknowledgmentID)
					m_CurrentActionData.m_State = UA_AM_REJECTED;
				break;
			case DayZPlayerSyncJunctures.SJ_ACTION_INTERRUPT:
				m_Interrupted = true;
				break;
		}
	}
		
	ActionTarget FindActionTarget()
	{
	}
	
	void StartDeliveredAction()
	{
	}
	
	static ActionBase GetActionVariant(typename actionName)
	{
		if (m_ActionNameActionMap)
		{
			ActionBase base_action = m_ActionNameActionMap.Get(actionName);
			ActionBase new_action = ActionBase.Cast(actionName.Spawn());
			
			new_action.CreateConditionComponents();
			new_action.SetID(base_action.GetID());
			new_action.SetInput(base_action.GetInput());
			
			return new_action;
		}
		return null;
	}
	
	static ActionBase GetAction(typename actionName)
	{
		if (m_ActionNameActionMap)
			return m_ActionNameActionMap.Get(actionName);
		return null;
	}
	
	static ActionBase GetAction(int actionID)
	{
		return m_ActionsArray.Get(actionID);
	}
	
	ActionBase GetContinuousAction()
	{
		if ( m_PrimaryAction )
		{
			return m_PrimaryAction;
		}
		return NULL;
	}
	
	ActionBase GetSingleUseAction()
	{
		if ( m_SecondaryAction )
		{ 
			return m_SecondaryAction;
		}
		return NULL;
	}
			
	TSelectableActionInfoArray GetSelectableActions()
	{
		return m_SelectableActions;
	}

	int GetSelectedActionIndex()
	{
		return m_SelectedActionIndex;
	}
	
	typename GetSelectedActionCategory()
	{
	}
	
	void SelectFirstActionCategory()
	{
	}
	
	void SelectNextActionCategory()
	{	
	}
	
	void SelectPrevActionCategory()
	{
	}
	
	void SelectNextAction()
	{
	}
	
	void SelectPrevAction()
	{
	}
	
	bool IsSelectableActionsChanged()
	{
		return m_SelectableActionsHasChanged;
	}
	//------------------------------------------------------
	bool ActionPossibilityCheck(int pCurrentCommandID)
	{	
		if ( !m_ActionsEnabled || m_Player.IsSprinting() || m_Player.IsUnconscious() || m_Player.GetCommandModifier_Action() || m_Player.GetCommand_Action() || m_Player.IsEmotePlaying() || m_Player.GetThrowing().IsThrowingAnimationPlaying() || m_Player.GetDayZPlayerInventory().IsProcessing() || m_Player.IsItemsToDelete() )
			return false;
		
		if ( m_Player.GetWeaponManager().IsRunning() )
			return false;
		
		return (pCurrentCommandID == DayZPlayerConstants.COMMANDID_ACTION || pCurrentCommandID == DayZPlayerConstants.COMMANDID_MOVE || pCurrentCommandID == DayZPlayerConstants.COMMANDID_SWIM || pCurrentCommandID == DayZPlayerConstants.COMMANDID_LADDER || pCurrentCommandID == DayZPlayerConstants.COMMANDID_VEHICLE);
	}	
	//------------------------------------------------------
	protected void SetActionContext( ActionTarget target, ItemBase item )
	{
		m_TestedActionTarget = target;
		m_TestedActionItem = item;
	}
	
	//------------------------------------------------------
		
	int GetActionState(ActionBase action)
	{
		if( m_CurrentActionData )
		{
			return m_CurrentActionData.m_State;
		}
		return UA_NONE; // TODO check if it is correct constant
	}
	
	//---------------------------------
	// EVENTS
	//---------------------------------
	void OnContinuousStart()
	{
	}
	
	void OnContinuousCancel()
	{	
	}
		
	void OnSingleUse()
	{
	}
	
	void Interrupt()
	{
	}
	protected void LocalInterrupt()
	{
		if (m_CurrentActionData && m_CurrentActionData.m_Action)
			m_CurrentActionData.m_Action.Interrupt(m_CurrentActionData);
	} 
	
	void OnInteractAction() //Interact
	{
	}

	void OnInstantAction(typename user_action_type, Param data = NULL)
	{
	}
	
	void OnActionEnd( )
	{
		if ( LogManager.IsActionLogEnable() )
		{
			if (m_CurrentActionData)
				Debug.ActionLog("n/a", m_CurrentActionData.m_Action.ToString() , "n/a", "OnActionEnd", m_CurrentActionData.m_Player.ToString() );
			Debug.ActionLog("Action data cleared ", this.ToString() , "n/a", "ActionEnd", m_CurrentActionData.m_Player.ToString() );
		}
		if (m_CurrentActionData)
			m_CurrentActionData.m_Action.ActionCleanup(m_CurrentActionData);
		m_CurrentActionData = NULL;
		
		m_Player.ResetActionEndInput();
	}
	
	void OnJumpStart()
	{
	}

	bool OnInputUserDataProcess(int userDataType, ParamsReadContext ctx)
	{
		return false;
	}
	
	float GetActionComponentProgress()
	{
		if(m_CurrentActionData)
			return m_CurrentActionData.m_Action.GetProgress(m_CurrentActionData);
		return 0.0;
	}
	
	int GetActionState()
	{
		if(m_CurrentActionData)
			return m_CurrentActionData.m_Action.GetState(m_CurrentActionData);
		return UA_NONE;
	}
	
	ActionReciveData GetReciveData()
	{
		return NULL;
	}
};