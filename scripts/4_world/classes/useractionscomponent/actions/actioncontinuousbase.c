class ActionContinuousBaseCB : ActionBaseCB
{	
	bool m_inLoop = false;
	bool m_callLoopEnd = false;
		
	bool CancelCondition()
	{
		if ( !m_ActionData )
		{
			return DefaultCancelCondition();
		}
		if ((GetState() == STATE_LOOP_LOOP || GetState() == STATE_LOOP_LOOP2 || m_inLoop) )
		{
			ActionContinuousBase actionS = ActionContinuousBase.Cast(m_ActionData.m_Action);	
			if ( m_ActionData.m_State == UA_INITIALIZE )
			{
				actionS.OnStartAnimationLoop( m_ActionData );
				m_ActionData.m_State = UA_PROCESSING;
				m_callLoopEnd = true;
			}
			
			if (!m_Interrupted)
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
	
	void SetInLoop(bool value)
	{
		 m_inLoop = value;
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
	
	//TODO: consider this for general use
	/*override void EndActionComponent()
	{
		ActionContinuousBase action = ActionContinuousBase.Cast(m_ActionData.m_Action);
		
		if (m_ActionData.m_State != UA_FINISHED)
			m_Canceled = true;
		
		if (m_ActionData.m_State == UA_FINISHED || m_ActionData.m_State == UA_CANCEL)
		{
			if (action.UseAlternativeInterrupt(m_ActionData))
				SetCommand(DayZPlayerConstants.CMD_ACTIONINT_FINISH);
			else
				SetCommand(DayZPlayerConstants.CMD_ACTIONINT_END);
		}
		else
		{
			SetCommand(DayZPlayerConstants.CMD_ACTIONINT_INTERRUPT);
		}
	}*/
	
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
		//EndActionComponent(); //already handled in 'AnimatedActionBase::Do' on cancel
	}
};

class ActionContinuousBase : AnimatedActionBase
{
	PluginAdminLog 	m_AdminLog; 
	
	void ActionContinuousBase() 
	{
		m_CallbackClass = ActionContinuousBaseCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_EAT;
		
		if( g_Game && g_Game.IsServer() )
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
	
	bool UseAlternativeInterrupt(ActionData action_data)
	{
		return false;
	}
	
	// lock default camera while performing the action
	bool IsCameraLockOnPerform()
	{
		return true;
	}
	
	// camera up/down angle when performing a fullbody action
	// values beetwen 0 and -90 (looking at ground) / 0 and 90 (looking above)
	Vector2 GetCameraUDAngle()
	{
		Vector2 udAngle = new Vector2(-70, 45);
		return udAngle;
	}
	
	// camera (heading) left/right angle when performing a fullbody action
	// values beetwen 0 and -180 (left restriction) / 0 and 180 (right restriction)
	Vector2 GetCameraLRAngle()
	{
		Vector2 lrAngle = new Vector2(-70, 70);
		return lrAngle;
	}

	override typename GetInputType()
	{
		return ContinuousDefaultActionInput;
	}

	override int GetActionCategory()
	{
		return AC_CONTINUOUS;
	}
	
	override void OnAnimationEvent(ActionData action_data)	
	{
		super.OnAnimationEvent(action_data);
		ActionContinuousBaseCB callback;
		if (Class.CastTo(callback, action_data.m_Callback))
		{
			if (action_data.m_DelayedAnimationEventID == UA_IN_START) 
			{
				OnStartAnimationLoop(action_data);
				callback.SetInLoop(true);
			}
			else if (action_data.m_DelayedAnimationEventID == UA_IN_END) 
			{
				OnEndAnimationLoop(action_data);
				callback.SetInLoop(false);
			}
		}
	}
	
	void OnStartAnimationLoop( ActionData action_data )
	{
		if ( LogManager.IsActionLogEnable() )
		{
			Debug.ActionLog("Time stamp: " + action_data.m_Player.GetSimulationTimeStamp(), this.ToString() , "n/a", "OnStartAnimationLoop", action_data.m_Player.ToString() );
		}
		
		if (g_Game.IsServer())
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
		if (g_Game.IsServer())
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
		if (g_Game.IsServer())
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