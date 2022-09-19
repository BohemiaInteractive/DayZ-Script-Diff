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
		if( LogManager.IsActionLogEnable() )
		{
			if( m_ActionData)
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
	
	//data
	void Interrupt()
	{
		if ( GetGame().IsServer() )
		{
			if( LogManager.IsActionLogEnable() )
			{
				Debug.ActionLog("n/a", this.ToString() , "n/a", "Interrupt", m_ActionData.m_Player.ToString() );	
			}
			if( GetGame().IsMultiplayer() )
			{
				DayZPlayerSyncJunctures.SendActionInterrupt(m_ActionData.m_Player);
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
	

	
	// Server version call on single too
	/*protected void OnStartServer( ActionData action_data ) //setup of action parameters, called before performing action
	{
	}
	protected void OnStartClient( ActionData action_data ) //setup of action parameters, called before performing action
	{
	}*/
	
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
	
	/*protected void OnCompleteLoopServer( ActionData action_data ) //method called on succesful finishing of action (before out animation - continuous actions only )
	{
	}
	protected void OnCompleteLoopClient( ActionData action_data ) //method called on succesful finishing of action (before out animation - continuous actions only )
	{
	}*/
	
	/*protected void OnRepeatServer( ActionData action_data ) //method called on succesful finishing of action
	{
	}
	protected void OnRepeatClient( ActionData action_data ) //method called on succesful finishing of action
	{
	}*/
	
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
				if( LogManager.IsActionLogEnable() )
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
		//player.GetActionManager().DisableActions();
			CreateAndSetupActionCallback(action_data);
		}
		/*Debug
		SendMessageToClient( player, "ActionBase.c : Start");
		Print("ActionBase.c : Start");
		*/
	}

	/*override bool Can( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if ( !m_FullBody && (!player || !player.IsPlayerInStance(GetStanceMask(player))) )
		{
			return false;
		}
		if ( m_ConditionItem && m_ConditionItem.Can(player, item) && m_ConditionTarget && m_ConditionTarget.Can(player, target) && ActionCondition(player, target, item) ) 
		{	
			return true;
		}
		return false;
	}
	
	protected override bool CanContinue( ActionData action_data )
	{
		if ( action_data.m_Player.IsPlayerInStance(action_data.m_PossibleStanceMask) && m_ConditionItem && m_ConditionItem.CanContinue(action_data.m_Player,action_data.m_MainItem) && m_ConditionTarget && m_ConditionTarget.CanContinue(action_data.m_Player,action_data.m_Target) && ActionConditionContinue(action_data) )
		{
			return true;
		}
		return false;
	}*/
	
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
#ifdef DEVELOPER
			if( LogManager.IsActionLogEnable() )
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
				//case UA_SETEND_2:
					//OnAlternativeEnd();
					//InformPlayers(player,target,UA_ERROR);
					//callback.Interrupt();
					//callback.SetCommand(DayZPlayerConstants.CMD_ACTIONINT_FINISH);//jtomasik - tohle pak zapoj do toho endovani az ti vse ostatni pobezi,  ted prehraje END2 (a tim to i ukonci), pozdeji by mozna piti prazdne lahve atp. mela byt samostatna akce
					//break;
				
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
				/*case UA_REPEAT:
					if ( CanContinue(action_data) )
					{
						if ( GetGame().IsServer() )  
						{
							OnRepeatServer(action_data);
							//ApplyModifiers(action_data);
						}
						else
						{
							OnRepeatClient(action_data);
						}
						InformPlayers(action_data.m_Player,action_data.m_Target,UA_REPEAT);
						action_data.m_WasExecuted = false;

						Do(action_data, UA_PROCESSING);
					}
					else
					{
						action_data.m_Callback.Interrupt();
						InformPlayers(action_data.m_Player,action_data.m_Target,UA_FINISHED);
						Do(action_data, UA_FINISHED);
					}				
					break;*/
				
				/*case UA_ANIM_EVENT:
					if( CanContinue(action_data))
					{
						if ( GetGame().IsServer() )
						{
							OnExecuteServer(action_data);
							ApplyModifiers(action_data);
						}
						else
						{
							OnExecuteClient(action_data);
						}
						InformPlayers(action_data.m_Player,action_data.m_Target,UA_REPEAT);
						Do(action_data, UA_PROCESSING);
					}
					else
					{
						action_data.m_Callback.Interrupt();
						InformPlayers(action_data.m_Player, action_data.m_Target, UA_CANCEL);
						Do(action_data, UA_CANCEL);
					}
					break;*/
				case UA_FINISHED:
					//OnCompleteLoop(action_data);
					InformPlayers(action_data.m_Player,action_data.m_Target,UA_FINISHED);
					action_data.m_Callback.EndActionComponent();
					break;

				case UA_CANCEL:
					InformPlayers(action_data.m_Player,action_data.m_Target,UA_CANCEL);
					action_data.m_Callback.EndActionComponent();
					/*if ( GetGame().IsServer() )  
					{
						OnInterruptServer(action_data);
					}
					else
					{
						OnInterruptClient(action_data);
					}*/
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
				//ApplyModifiers(action_data);
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
			if( LogManager.IsActionLogEnable() )
			{
				Debug.ActionLog("Ation could not be finished right at the end", this.ToString() , "n/a", "End", action_data.m_Player.ToString() );
			}
			action_data.m_Callback.Interrupt();
		}
		
		//player.GetActionManager().EnableActions();
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