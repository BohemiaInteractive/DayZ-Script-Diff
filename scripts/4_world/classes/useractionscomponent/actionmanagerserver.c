class ActionManagerServer: ActionManagerBase
{		
	protected ActionBase 		m_PendingAction;
	ref ActionReciveData		m_PendingActionReciveData;
	
	void ActionManagerServer(PlayerBase player)
	{
		m_PendingAction 			= null;
		m_PendingActionReciveData 	= null;
	}
	
	//------------------------------------------
	//EVENTS 
	//------------------------------------------
	override void OnJumpStart()
	{
		EndOrInterruptCurrentAction();
	}
	
	override void EndOrInterruptCurrentAction()
	{
		if (m_CurrentActionData)
		{
			if (m_CurrentActionData.m_State == UA_AM_PENDING || m_CurrentActionData.m_State == UA_AM_REJECTED || m_CurrentActionData.m_State == UA_AM_ACCEPTED)
			{
				OnActionEnd();
				m_PendingActionAcknowledgmentID = -1;
			}
			else
			{
				m_CurrentActionData.m_Action.Interrupt(m_CurrentActionData);
			}
		}
	}

	override bool OnInputUserDataProcess(int userDataType, ParamsReadContext ctx)
	{		
		switch (userDataType)
		{
			case INPUT_UDT_STANDARD_ACTION_START:
			{
				bool success = true;
				
				int actionID = 0;
				if (!ctx.Read(actionID))
					return false;
				
				ActionBase recvAction = GetAction(actionID);
				if (!recvAction)
					return false;
				
				if (!recvAction.ReadFromContext(ctx, m_PendingActionReciveData))
					success = false;
				
				m_PendingAction = recvAction;
					
				if (recvAction.UseAcknowledgment())
				{
					int ackID;
					if (!ctx.Read(ackID))
						success = false;
						
					m_PendingActionAcknowledgmentID = ackID;
				}
					
				break;

			}

			case INPUT_UDT_STANDARD_ACTION_END_REQUEST:
			{
				//Debug.Log("Action want end request, STS = " + m_Player.GetSimulationTimeStamp());
				int commandID = -10;
				ctx.Read(commandID);
				
				if (commandID == DayZPlayerConstants.CMD_ACTIONINT_INTERRUPT)
				{
					//Print("INPUT_UDT_STANDARD_ACTION_END_REQUEST | CMD_ACTIONINT_INTERRUPT");
					RequestInterruptAction();
				}
				else
				{
					//Print("INPUT_UDT_STANDARD_ACTION_END_REQUEST | m_ActionWantEndRequest");
					m_ActionWantEndRequest = true;
				}
			}
			
			case INPUT_UDT_STANDARD_ACTION_INPUT_END:
			{
				//Debug.Log("Action input ended, STS = " + m_Player.GetSimulationTimeStamp());
				m_ActionInputWantEnd = true;
			}
			default:
				return false;
		}
		
		if (!success)
		{
			//Debug.Log("[AM] OnInputUserDataProcess INPUT_UDT_STANDARD_ACTION_START Error");
			if (recvAction.UseAcknowledgment())
			{
				DayZPlayerSyncJunctures.SendActionAcknowledgment(m_Player, m_PendingActionAcknowledgmentID, false);
			}
			else
			{
				RequestInterruptAction();
			}

			return false;
		}
		return true;
		
	}
	
	override void StartDeliveredAction()
	{
		if (!m_CurrentActionData)
		{
			//! error - expected action data
			return;
		}
		
		m_Interrupted 			= false;

		bool accepted 			= false;		
		ActionBase pickedAction	= m_CurrentActionData.m_Action;
		ActionTarget target		= m_CurrentActionData.m_Target;
		ItemBase item			= m_CurrentActionData.m_MainItem;

		if (LogManager.IsActionLogEnable())
		{
			if (target)
			{
				Debug.ActionLog("Item = " + item + ", " + target.DumpToString(), pickedAction.ToString() , "n/a", "DeliveredAction", m_Player.ToString());
			}
			else
			{
				Debug.ActionLog("Item = " + item + ", NULL", pickedAction.ToString() , "n/a", "DeliveredAction", m_Player.ToString());
			}
		}

		if (!m_Player.GetCommandModifier_Action() && !m_Player.GetCommand_Action() && !m_Player.IsSprinting() && pickedAction && pickedAction.Can(m_Player,target,item)) 
		{
			if (pickedAction.AddActionJuncture(m_CurrentActionData))
				accepted = true;
		}
		
		if (accepted)
		{
			if (LogManager.IsActionLogEnable())
			{
				Debug.ActionLog("Action accepted", pickedAction.ToString() , "n/a", "CheckDeliveredAction", m_Player.ToString() );
			}
			//Debug.Log("[AM] Action acccepted");
			if (pickedAction.UseAcknowledgment())
			{
				//Unlock target
				m_CurrentActionData.m_State = UA_AM_PENDING;
				DayZPlayerSyncJunctures.SendActionAcknowledgment(m_Player, m_PendingActionAcknowledgmentID, true);
			}
			else
			{
				m_CurrentActionData.m_State = UA_AM_ACCEPTED;
			}
		}
		else
		{
			if (LogManager.IsActionLogEnable())
			{
				Debug.ActionLog("Action rejected", pickedAction.ToString() , "n/a", "CheckDeliveredAction", m_Player.ToString() );
			}

			if (pickedAction.UseAcknowledgment())
			{
				DayZPlayerSyncJunctures.SendActionAcknowledgment(m_Player, m_PendingActionAcknowledgmentID, false);
			}
			else
			{
				RequestInterruptAction();
			}
		}
	}
	
	override void OnActionEnd()
	{
		//Debug.Log("Action ended - hard, STS = " + m_Player.GetSimulationTimeStamp());
		if (m_CurrentActionData)
		{
			m_CurrentActionData.m_Action.ClearActionJuncture(m_CurrentActionData);

			super.OnActionEnd();
		}
	}
	
	//pCurrentCommandID is command ID at time of call command handler, some called methods can change actual true value (need call m_Player.GetCurrentCommandID() for actual command ID)
	override void Update(int pCurrentCommandID)
	{
		super.Update(pCurrentCommandID);
		int currentCommandID = m_Player.GetCurrentCommandID();
		
		//Debug.Log("m_ActionWantEnd " + m_ActionInputWantEnd);
		
		if (m_PendingAction)
		{
			if (m_CurrentActionData)
			{
				DayZPlayerSyncJunctures.SendActionAcknowledgment(m_Player, m_PendingActionAcknowledgmentID, false);
			}
			else
			{
				m_ActionWantEndRequest 	= false;
				m_ActionInputWantEnd 	= false;
				
				bool success 			= true;
				ActionTarget target 	= new ActionTarget(null, null, -1, vector.Zero, 0); 
					
				if ( LogManager.IsActionLogEnable() )
				{	
					Debug.ActionLog("n/a", m_PendingAction.ToString() , "n/a", "HandlePendingAction", m_Player.ToString() );
				}

				if (!m_PendingAction.SetupAction(m_Player,target,m_Player.GetItemInHands(),m_CurrentActionData))
				{
					success = false;
				}
				//Debug.Log("[AM] Action data synced (" + m_Player + ") success: " + success);
			
				if (success)
				{
					StartDeliveredAction();
				}
				else
				{
					if (m_PendingAction.UseAcknowledgment())
					{
						DayZPlayerSyncJunctures.SendActionAcknowledgment(m_Player, m_PendingActionAcknowledgmentID, false);
					}
					else
					{
						RequestInterruptAction();
					}
				}
			}

			m_PendingAction 			= null;
			m_PendingActionReciveData 	= null;
		}
	
		if (m_CurrentActionData)
		{			
			if (m_CurrentActionData.m_State != UA_AM_PENDING && m_CurrentActionData.m_State != UA_AM_REJECTED && m_CurrentActionData.m_State != UA_AM_ACCEPTED)
			{
				m_CurrentActionData.m_Action.OnUpdateServer(m_CurrentActionData);
			}
			
			//Debug.Log("m_CurrentActionData.m_State: " + m_CurrentActionData.m_State +" m_ActionWantEnd: " + m_ActionWantEndRequest );
			switch (m_CurrentActionData.m_State)
			{
				case UA_AM_PENDING:
					break;
			
				case UA_AM_ACCEPTED:
					// check currentCommandID before start or reject 
				
					int condition_mask = ActionBase.ComputeConditionMask(m_Player, m_CurrentActionData.m_Target, m_CurrentActionData.m_MainItem);
					bool canActionPerform = ((condition_mask & m_CurrentActionData.m_Action.m_ConditionMask) == condition_mask);
					if (canActionPerform && ActionPossibilityCheck(currentCommandID))
					{
						m_CurrentActionData.m_State = UA_START;
						m_CurrentActionData.m_Action.Start(m_CurrentActionData);
					
						if (m_CurrentActionData.m_Action && m_CurrentActionData.m_Action.IsInstant())
							OnActionEnd();
					}
					else
					{
						RequestInterruptAction();
					}
					m_PendingActionAcknowledgmentID = -1;
					break;
				
				case UA_AM_REJECTED:
					OnActionEnd();
					m_PendingActionAcknowledgmentID = -1;
					break;
			
				default:
					if (m_ActionInputWantEnd)
					{
						m_ActionInputWantEnd = false;
						m_CurrentActionData.m_Action.EndInput(m_CurrentActionData);
					}
				
					if (m_ActionWantEndRequest)
					{
						m_ActionWantEndRequest = false;
						m_CurrentActionData.m_Action.EndRequest(m_CurrentActionData);
					}
					break;
			}
		}
	}
	
	//! server requests action interrupt
	override void RequestInterruptAction()
	{
		if (m_CurrentActionData)
			DayZPlayerSyncJunctures.SendActionInterrupt(m_Player);
	}
	
	override ActionReciveData GetReciveData()
	{
		return m_PendingActionReciveData;
	}
}
