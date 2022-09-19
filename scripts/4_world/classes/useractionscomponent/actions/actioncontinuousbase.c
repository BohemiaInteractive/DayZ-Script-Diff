class ActionContinuousBaseCB : ActionBaseCB
{	
	bool m_inLoop = false;
	bool m_callLoopEnd = false;
		
	bool CancelCondition()
	{
		/*if(m_Interrupted)
		{
			return DefaultCancelCondition();
		}*/
		//SetCommand(DayZPlayerConstants.CMD_ACTIONINT_ACTIONLOOP);
		//Print("cancel condition enabled: " + GetState().ToString() );
		if ( !m_ActionData )
		{
			return DefaultCancelCondition();
		}
		if ( !m_Interrupted && (GetState() == STATE_LOOP_LOOP || GetState() == STATE_LOOP_LOOP2 || m_inLoop) )
		{
			ActionContinuousBase actionS = ActionContinuousBase.Cast(m_ActionData.m_Action);	
			if ( m_ActionData.m_State == UA_INITIALIZE )
			{
				actionS.OnStartAnimationLoop( m_ActionData );
				m_ActionData.m_State = UA_PROCESSING;
				m_callLoopEnd = true;
			}
			
			actionS.Do(m_ActionData, m_ActionData.m_State);
		}
		else if (m_callLoopEnd == true)
		{
			ActionContinuousBase actionE = ActionContinuousBase.Cast(m_ActionData.m_Action);
			actionE.OnEndAnimationLoop( m_ActionData );
			m_callLoopEnd = false;
		}
		return DefaultCancelCondition();
	}
	
	
	override void OnAnimationEvent(int pEventID)	
	{
		if ( m_ActionData && m_ActionData.m_Action )
		{
			AnimatedActionBase action = AnimatedActionBase.Cast(m_ActionData.m_Action);
#ifdef DEVELOPER
			//Print("ActionInteractBase.c | OnAnimationEvent | OnAnimationEvent called");
#endif
			if ( !m_Interrupted && pEventID == UA_IN_START ) 
			{
				m_inLoop = true;
			//ActionContinuousBase.Cast(action).OnStartLoop( m_ActionData );
			}
			else if ( !m_Interrupted && pEventID == UA_IN_END ) 
			{
			
				m_inLoop = false;
				//ActionContinuousBase.Cast(action).OnCompleteLoop( m_ActionData );
			}
			else if ( !m_Interrupted && pEventID == UA_ANIM_EVENT ) 
			{
				action.OnAnimationEvent( m_ActionData );
				//action.OnCompleteLoop( m_ActionData );
			}
		}
		else
		{
			//Debug.LogError("Call OnAnimationEvent ")
		}
	}
	
	override void InitActionComponent()
	{
		m_Interrupted = false;
		m_Canceled = false;
		m_inLoop = false;

		CreateActionComponent();
		if ( m_ActionData.m_ActionComponent ) 
		{
			m_ActionData.m_ActionComponent.Init(m_ActionData);		
		}
		m_ActionData.m_State = UA_INITIALIZE;
		RegisterAnimationEvent("ActionExecStart", UA_IN_START);
		RegisterAnimationEvent("ActionExecEnd", UA_IN_END);
		RegisterAnimationEvent("ActionExec", UA_ANIM_EVENT);
		EnableCancelCondition(true);
		m_SoundObject = m_ActionData.m_Action.PlayActionSound(m_ActionData.m_Player);
	}
	
	override void EndActionComponent()
	{		
		// TODO for second type animation SetCommand(DayZPlayerConstants.CMD_ACTIONINT_FINISH);
		if ( m_ActionData.m_State == UA_FINISHED )
		{
			SetCommand(DayZPlayerConstants.CMD_ACTIONINT_END);
		}
		else if (m_ActionData.m_State == UA_CANCEL )
		{
			ActionContinuousBase action = ActionContinuousBase.Cast(m_ActionData.m_Action);
			if(action.HasAlternativeInterrupt())
			{
				SetCommand(DayZPlayerConstants.CMD_ACTIONINT_FINISH);
			}
			else
			{
				SetCommand(DayZPlayerConstants.CMD_ACTIONINT_END);
			}
			m_Canceled = true;
			return;
			//Cancel();
		}
		else
		{
			m_Canceled = true;
			SetCommand(DayZPlayerConstants.CMD_ACTIONINT_END);
			return;
		}
		m_ActionData.m_State = UA_FINISHED;
	}	
	
	void UserEndsAction()
	{
		if ( m_ActionData.m_ActionComponent ) 
		{
			m_ActionData.m_State = m_ActionData.m_ActionComponent.Cancel(m_ActionData);
		}
		EndActionComponent();
	}
};

class ActionContinuousBase : AnimatedActionBase
{
	PluginAdminLog 	m_AdminLog; 
	
	void ActionContinuousBase() 
	{
		m_CallbackClass = ActionContinuousBaseCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_EAT;
		
		if( GetGame() && GetGame().IsServer() )
		{
			m_AdminLog = PluginAdminLog.Cast( GetPlugin(PluginAdminLog) );
		}
	}
	
	override void OnEndInput( ActionData action_data )
	{
		ActionContinuousBaseCB callback;
		if( Class.CastTo(callback, action_data.m_Callback) )
		{
			if ( callback.GetState() != callback.STATE_LOOP_END && callback.GetState() != callback.STATE_LOOP_END2 )
			{
				callback.UserEndsAction();
			}
		}
	}
	
	bool HasAlternativeInterrupt()
	{
		return false;
	}

	override typename GetInputType()
	{
		return ContinuousDefaultActionInput;
	}

	override int GetActionCategory()
	{
		return AC_CONTINUOUS;
	}
	
	void OnStartAnimationLoop( ActionData action_data )
	{
		if ( LogManager.IsActionLogEnable() )
		{
			Debug.ActionLog("Time stamp: " + action_data.m_Player.GetSimulationTimeStamp(), this.ToString() , "n/a", "OnStartAnimationLoop", action_data.m_Player.ToString() );
		}
		
		if (GetGame().IsServer())
		{
			OnStartAnimationLoopServer(action_data);
		}
		else
		{
			OnStartAnimationLoopClient(action_data);
		}
		action_data.m_WasExecuted = false;
	}
	
	void OnEndAnimationLoop( ActionData action_data )
	{
		if ( LogManager.IsActionLogEnable() )
		{
			Debug.ActionLog("Time stamp: " + action_data.m_Player.GetSimulationTimeStamp(), this.ToString() , "n/a", "OnEndAnimationLoop", action_data.m_Player.ToString() );
		}
		if (GetGame().IsServer())
		{
			OnEndAnimationLoopServer(action_data);
		}
		else
		{
			OnEndAnimationLoopClient(action_data);
		}
		action_data.m_WasExecuted = false;
	}
	
	void OnFinishProgress( ActionData action_data )
	{	
		if ( LogManager.IsActionLogEnable() )
		{
			Debug.ActionLog("Time stamp: " + action_data.m_Player.GetSimulationTimeStamp(), this.ToString() , "n/a", "OnFinishProgress", action_data.m_Player.ToString() );
		}	
		if (GetGame().IsServer())
		{
			OnFinishProgressServer(action_data);
			
			if ( m_AdminLog )
			{
				m_AdminLog.OnContinouousAction( action_data );
			}
		}
		else
		{
			OnFinishProgressClient(action_data);
		}
		action_data.m_WasExecuted = false;
	}
	
	
	protected void OnStartAnimationLoopServer( ActionData action_data ) //method called on start main animation loop (after in animation part )
	{
		action_data.m_WasActionStarted  = true;
	}
	
	protected void OnStartAnimationLoopClient( ActionData action_data ) //method called on start main animation loop (after in animation part )
	{
		action_data.m_WasActionStarted  = true;
	}
	
	protected void OnEndAnimationLoopServer( ActionData action_data ) //method called on finish main animation loop (before out animation part )
	{
	}
	protected void OnEndAnimationLoopClient( ActionData action_data ) //method called on finish main animation loop (before out animation part )
	{
	}
	
	protected void OnFinishProgressServer( ActionData action_data )
	{
	}
	
	protected void OnFinishProgressClient( ActionData action_data )
	{
	}
};