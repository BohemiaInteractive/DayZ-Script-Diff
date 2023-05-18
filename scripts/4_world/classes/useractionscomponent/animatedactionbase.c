class ActionBaseCB : HumanCommandActionCallback
{
	protected ActionData				m_ActionData;
	protected SoundOnVehicle 			m_SoundObject;//object of sound playing on entity
	protected bool 						m_Canceled;//helps prevent doubled calling of actionbase End method
	protected bool						m_Interrupted;//force callback to wait till action syncs its interruption
		
		
	void ActionBaseCB()
	{
	}
	
	int GetPossileStanceMask()
	{
		return m_ActionData.m_PossibleStanceMask;
	}
	
	//Command events
	override void OnFinish(bool pCanceled)	
	{
		if ( LogManager.IsActionLogEnable() )
		{
			if ( m_ActionData)
				Debug.ActionLog("Time stamp: " + m_ActionData.m_Player.GetSimulationTimeStamp(), m_ActionData.m_Action.ToString() , "n/a", "OnFinish", m_ActionData.m_Player.ToString() );
			else
				Debug.ActionLog("n/a", this.ToString() , "n/a", "OnFinish", "n/a" );
		}
		if ( m_SoundObject ) 
		{
			GetGame().ObjectDelete(m_SoundObject);
		}
		if ( m_ActionData && m_ActionData.m_Player )
		{
			if ( pCanceled ) 
			{
				m_ActionData.m_State = m_ActionData.m_ActionComponent.Interrupt(m_ActionData);
			}
			
			AnimatedActionBase action = AnimatedActionBase.Cast(m_ActionData.m_Action);
			
			if(action)
				action.End(m_ActionData);
		}
	}
	
	void SetCommand( int command_uid )
	{		
		InternalCommand(command_uid);
	}

	//---------------------------------------------------------------
	//Action Component handlers
	void CreateActionComponent() 
	{
		m_ActionData.m_ActionComponent = new CADummy;
	}
	
	void InitActionComponent()
	{
	}
	
	void ProgressActionComponent()
	{
		if ( m_ActionData.m_ActionComponent )
		{ 
			m_ActionData.m_State = m_ActionData.m_ActionComponent.Execute(m_ActionData);
		}
	}	
	
	void EndActionComponent()
	{
	}	
	
	//// THIS Interrupt is use for initialize interrupt - WARNING Interrupt with parametr is used for handling interrupt
	void Interrupt()
	{
		if ( GetGame().IsServer() )
		{
			if ( LogManager.IsActionLogEnable() )
			{
				Debug.ActionLog("n/a", this.ToString() , "n/a", "Interrupt", m_ActionData.m_Player.ToString() );	
			}
			if ( GetGame().IsMultiplayer() )
			{
				DayZPlayerSyncJunctures.SendActionInterrupt(m_ActionData.m_Player);
			}
		}
		else
		{
			if (ScriptInputUserData.CanStoreInputUserData())
			{
				ScriptInputUserData ctx = new ScriptInputUserData;
				ctx.Write(INPUT_UDT_STANDARD_ACTION_END_REQUEST);
				ctx.Write(DayZPlayerConstants.CMD_ACTIONINT_INTERRUPT);
				ctx.Send();
			}
		}
		m_Interrupted = true;
	}
	
	void SetActionData(ActionData action_data )
	{
		m_ActionData = action_data;
	}
	
	override bool IsUserActionCallback()
	{
		return true;
	}
	
	float GetActionComponentProgress()
	{
		if ( m_ActionData.m_ActionComponent )
		{
			return m_ActionData.m_ActionComponent.GetProgress();
		}
		return 0;
	}
	
	int GetActionState()
	{
		return m_ActionData.m_State;
	}
	
	ActionTarget GetTarget()
	{
		return m_ActionData.m_Target;
	}
};



class AnimatedActionBase : ActionBase
{	
	protected int 					m_CommandUID;	//tells which command should be used for callback
	protected int					m_CommandUIDProne;
	protected typename 				m_CallbackClass; //tells which callback should be instantiated

	
	//-----------------------------------------------------
	// 	Action events and methods
	//-----------------------------------------------------
	void AnimatedActionBase() 
	{
		// definable
		m_CallbackClass = ActionBaseCB;
	}
	
	protected void OnAlternativeEndServer( PlayerBase player ) //method called when action has not met conditions in action component
	{
	}
	protected void OnAlternativeEndClient( PlayerBase player ) //method called when action has not met conditions in action component
	{
	}
	
	protected void OnInterruptServer( PlayerBase player ) //method called when action has not met conditions in action component
	{
	}
	protected void OnInterruptClient( PlayerBase player ) //method called when action has not met conditions in action component
	{
	}
	
	protected void OnExecute( ActionData action_data )
	{
	}
	
	protected void OnExecuteServer( ActionData action_data )
	{
	}
	
	protected void OnExecuteClient( ActionData action_data )
	{
	}
	
	//TODO MW - add comment 
	void OnAnimationEvent( ActionData action_data )
	{
		if (action_data && !action_data.m_WasExecuted)
		{
			ActionBase action = action_data.m_Action;
			
			if (action && ( !action.UseMainItem() || action_data.m_MainItem ) && ( !action.HasTarget() || action_data.m_Target ))
			{
				if ( LogManager.IsActionLogEnable() )
				{
					Debug.ActionLog("Time stamp: " + action_data.m_Player.GetSimulationTimeStamp(), this.ToString() , "n/a", "OnExecute", action_data.m_Player.ToString() );
				}
				OnExecute(action_data);
			
				if (GetGame().IsServer())
					OnExecuteServer(action_data);
				else
					OnExecuteClient(action_data);

				action_data.m_WasExecuted = true;
				action_data.m_WasActionStarted  = true;
			}
		}
	}
	
	override bool ActionConditionContinue( ActionData action_data ) //condition for action
	{
		return ActionCondition(action_data.m_Player,action_data.m_Target,action_data.m_MainItem);
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item ) //condition for action
	{
		return true;
	}
	
	//TODO MW - add comment 
	protected int GetActionCommand( PlayerBase player )
	{
		if ( HasProneException() )
		{
			if ( player.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT))
				return m_CommandUID;
			else
				return m_CommandUIDProne;
		}
		return m_CommandUID;
	}
	
	protected typename GetCallbackClassTypename()
	{
		return m_CallbackClass; 
	}
	
	// if it is set to true if action have special fullbody animation for prone and additive for crouch and erect
	override bool HasProneException()
	{
		return false;
	}
	
	//TODO MW - add comment 
	protected void CreateAndSetupActionCallback( ActionData action_data )
	{
		//Print("ActionBase.c | CreateAndSetupActionCallback | DBG ACTION CALLBACK CREATION CALLED");
		ActionBaseCB callback;
		if (  IsFullBody(action_data.m_Player) )
		{
			Class.CastTo(callback, action_data.m_Player.StartCommand_Action(GetActionCommand(action_data.m_Player),GetCallbackClassTypename(),GetStanceMask(action_data.m_Player)));	
			//Print("ActionBase.c | CreateAndSetupActionCallback |  DBG command starter");		
		}
		else
		{
			Class.CastTo(callback, action_data.m_Player.AddCommandModifier_Action(GetActionCommand(action_data.m_Player),GetCallbackClassTypename()));
			//Print("ActionBase.c | CreateAndSetupActionCallback |  DBG command modif starter: "+callback.ToString()+"   id:"+GetActionCommand().ToString());
			
		}
		callback.SetActionData(action_data); 
		callback.InitActionComponent(); //jtomasik - tohle mozna patri do constructoru callbacku?
		action_data.m_Callback = callback;
	}
		
		
		
	//	ACTION LOGIC -------------------------------------------------------------------
	// called from actionmanager.c
	override void Start( ActionData action_data ) //Setup on start of action
	{
		super.Start( action_data );
		//Debug.Log("[Action DEBUG] Start time stamp: " + action_data.m_Player.GetSimulationTimeStamp());

		if( !IsInstant() )
		{
			CreateAndSetupActionCallback(action_data);
		}
		/*Debug
		SendMessageToClient( player, "ActionBase.c : Start");
		Print("ActionBase.c : Start");
		*/
	}
	
	// THIS Interrupt is use for ACTUAL handling of intrrupt on action side - WARNING Interrupt without parametr is used for initialize interrupt
	override void Interrupt( ActionData action_data )
	{
		if ( action_data.m_Callback )
		{
			action_data.m_Callback.Cancel();
		}
		else
		{
			End( action_data );
		}
	}
	
	void OnJumpStart()
	{
		
	}
		
	// called from ActionBaseCB.c
	void Do( ActionData action_data, int state )
	{
		if ( state == UA_ERROR || !action_data.m_Callback || !action_data.m_Player || !action_data.m_ActionComponent ) //jtomasik - tohle mozna muze byt v CancelCondtion metodu callbacku?
		{
#ifdef DIAG_DEVELOPER
			if ( LogManager.IsActionLogEnable() )
			{
				Debug.ActionLog("ABRUPT CANCEL, CONDITIONS NOT MET", this.ToString() , "n/a", "Do", action_data.m_Player.ToString() );
			}
#endif
			if ( action_data.m_Callback && action_data.m_Player )
			{			
				action_data.m_Callback.Interrupt();
			}
			else
			{
				Debug.ActionLog("PLAYER LOST", this.ToString() , "n/a", "Do", action_data.m_Player.ToString() );
			}	
		}
		else
		{
			switch ( state )
			{
				case UA_PROCESSING:	
					if ( CanContinue(action_data) )
					{	
						action_data.m_Callback.ProgressActionComponent();
						InformPlayers(action_data.m_Player, action_data.m_Target, UA_PROCESSING);
					}
					else
					{
						action_data.m_Callback.Interrupt();
						InformPlayers(action_data.m_Player, action_data.m_Target, UA_CANCEL);
						Do(action_data, UA_CANCEL);
					}
					break;
				
				case UA_FINISHED:
					InformPlayers(action_data.m_Player,action_data.m_Target,UA_FINISHED);
					action_data.m_Callback.EndActionComponent();
					break;
				
				case UA_CANCEL:
					InformPlayers(action_data.m_Player,action_data.m_Target,UA_CANCEL);
					action_data.m_Callback.EndActionComponent();
					break;	
				
				case UA_INITIALIZE:
					if ( !CanContinue(action_data) )
					{						
						action_data.m_Callback.Interrupt();
						InformPlayers(action_data.m_Player, action_data.m_Target, UA_CANCEL);
						Do(action_data, UA_CANCEL);
					}
				default:
					Debug.ActionLog("Action component returned wrong value", this.ToString() , "n/a", "Do", action_data.m_Player.ToString() );
					action_data.m_Callback.Interrupt();
					break;
			}
		}
	}
	
	// called from ActionBaseCB.c 
	override void End( ActionData action_data )
	{
		if ( action_data.m_Player ) 
		{
			if ( GetGame().IsServer() )
			{
				OnEndServer(action_data);
				action_data.m_Player.SetSoundCategoryHash(0);
			}
			else
			{
				OnEndClient(action_data);
			}
			OnEnd(action_data);
			
			// Xbox Achievemnts
			if ( action_data.m_WasActionStarted )
			{
				if ( IsEat() )
				{
					GetGame().GetAnalyticsClient().OnActionEat();
				}
				else if ( IsDrink() )
				{
					GetGame().GetAnalyticsClient().OnActionDrink();
				}
				
				action_data.m_WasActionStarted = false;
			}
			
			action_data.m_Player.GetActionManager().OnActionEnd();
			
			
		}
		else
		{
			if ( LogManager.IsActionLogEnable() )
			{
				Debug.ActionLog("Ation could not be finished right at the end", this.ToString() , "n/a", "End", action_data.m_Player.ToString() );
			}
			action_data.m_Callback.Interrupt();
		}
	}
	
	override float GetProgress( ActionData action_data )
	{
		if ( action_data.m_ActionComponent )
		{	 
			return action_data.m_ActionComponent.GetProgress();
		}
		return 0;
	}
};