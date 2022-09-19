	//Advanced placement
	//vector entity_position;
	//vector entity_orientation;


class ActionManagerServer: ActionManagerBase
{		
	protected ActionBase 				m_PendingAction;
	ref ActionReciveData				m_PendingActionReciveData;
	
	void ActionManagerServer(PlayerBase player)
	{
		m_PendingAction = NULL;
		m_PendingActionReciveData = NULL;
		//ActionManagerBase(player);
	}
	
	//------------------------------------------
	//EVENTS 
	//------------------------------------------
	// Continuous---------------------------------------------
	override void OnJumpStart()
	{
		if(m_CurrentActionData)
		{
			if( m_CurrentActionData.m_State == UA_AM_PENDING || m_CurrentActionData.m_State == UA_AM_REJECTED || m_CurrentActionData.m_State == UA_AM_ACCEPTED)
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
	
	// Single----------------------------------------------------
	override void OnSingleUse()
	{
//			StartDeliveredAction();
	}
	
	// Interact----------------------------------------------------
	override void OnInteractAction() //Interact
	{
//		if ( m_SelectedActionIndex == 0 ) // tertiary action is always on index 0 now
//		{
//				StartDeliveredAction();
//		}
//		else
//		{
		//	rozbal debug akci
//		}

	}

	override bool OnInputUserDataProcess(int userDataType, ParamsReadContext ctx)
	{		
		switch ( userDataType )
		{
			case INPUT_UDT_STANDARD_ACTION_START:
			{
				bool success = true;
				
				int actionID = 0;
				if (!ctx.Read(actionID) )
					return false;
				
				ActionBase recvAction = GetAction(actionID);
				
				if (!recvAction)
					return false;
				
				if (!recvAction.ReadFromContext(ctx, m_PendingActionReciveData))
				{
					success = false;
				}
				
				m_PendingAction = recvAction;
					
				if (recvAction.UseAcknowledgment())
				{
					int AckID;
					if (!ctx.Read(AckID))
						success = false;
						
					m_PendingActionAcknowledgmentID = AckID;
				}
					
				break;

			}

			case INPUT_UDT_STANDARD_ACTION_END_REQUEST:
			{
				//Debug.Log("Action want end request, STS = " + m_Player.GetSimulationTimeStamp());
				int commandID = -10;
				ctx.Read(commandID);
				
				if ( commandID == DayZPlayerConstants.CMD_ACTIONINT_INTERRUPT )
				{
					//Print("INPUT_UDT_STANDARD_ACTION_END_REQUEST | CMD_ACTIONINT_INTERRUPT");
					DayZPlayerSyncJunctures.SendActionInterrupt(m_Player);
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
				DayZPlayerSyncJunctures.SendActionInterrupt(m_Player);
			}
			//m_CurrentActionData = NULL;
			return false;
		}
		//StartDeliveredAction();
		return true;
		
	}
	
	override void StartDeliveredAction()
	{
		if ( !m_CurrentActionData )
		{
			//! error - expected action data
			//Interrupt();
			return;
		}
		
		m_Interrupted = false;
		
		ActionBase picked_action;
		bool accepted = false;
		bool is_target_free = true;
		ActionTarget target;
		ItemBase item;
		
		picked_action = m_CurrentActionData.m_Action;
		target = m_CurrentActionData.m_Target;
		item = m_CurrentActionData.m_MainItem;

		if ( LogManager.IsActionLogEnable() )
		{
			Debug.ActionLog("Item = " + item + ", " + target.DumpToString(), picked_action.ToString() , "n/a", "DeliveredAction", m_Player.ToString() );
		}

		if ( is_target_free && !m_Player.GetCommandModifier_Action() && !m_Player.GetCommand_Action() && !m_Player.IsSprinting() && picked_action && picked_action.Can(m_Player,target,item)) 
		{
			accepted = true;
			if ( picked_action.HasTarget())
			{
				EntityAI targetEntity;
				if ( EntityAI.CastTo(targetEntity,target.GetObject()) )
				{
					//Man target_owner;
					//target_owner = targetEntity.GetHierarchyRootPlayer();
					//if( target_owner && target_owner != m_Player && target_owner.IsAlive() )
					//{
					//	accepted = false;
					//}
					if ( !AdvancedCommunication.Cast(targetEntity) && !Building.Cast(targetEntity) && !Fireplace.Cast(targetEntity) )
					{
						//Lock target
						if ( !GetGame().AddActionJuncture(m_Player,targetEntity,10000) )
						{
							accepted = false;
						}
					}
				}
			}
		}
		
		if ( accepted )
		{
			if ( LogManager.IsActionLogEnable() )
			{
				Debug.ActionLog("Action accepted", picked_action.ToString() , "n/a", "CheckDeliveredAction", m_Player.ToString() );
			}
			//Debug.Log("[AM] Action acccepted");
			if (picked_action.UseAcknowledgment())
			{
						//Unlock target
						//GetGame().ClearJuncture(m_Player, oldTrgetItem);
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
			if ( LogManager.IsActionLogEnable() )
			{
				Debug.ActionLog("Action rejected", picked_action.ToString() , "n/a", "CheckDeliveredAction", m_Player.ToString() );
			}
			if (picked_action.UseAcknowledgment())
			{
				DayZPlayerSyncJunctures.SendActionAcknowledgment(m_Player, m_PendingActionAcknowledgmentID, false);
			}
			else
			{
				DayZPlayerSyncJunctures.SendActionInterrupt(m_Player);
			}
		}
	}
	
	override void OnActionEnd()
	{
		//Debug.Log("Action ended - hard, STS = " + m_Player.GetSimulationTimeStamp());
		if (m_CurrentActionData)
		{
			if ( m_CurrentActionData.m_Target )
			{
				EntityAI targetEntity;
				if ( targetEntity.CastTo(targetEntity, m_CurrentActionData.m_Target.GetObject()) && !Building.Cast(targetEntity) )
				{
					GetGame().ClearJuncture(m_CurrentActionData.m_Player, targetEntity);
				}
			}
			super.OnActionEnd();
		}
	}
	
	
	override void Update(int pCurrentCommandID)
	{
		super.Update(pCurrentCommandID);
		
		//Debug.Log("m_ActionWantEnd " + m_ActionInputWantEnd);
		
		if (m_PendingAction)
		{
			if ( m_CurrentActionData )
			{
				DayZPlayerSyncJunctures.SendActionAcknowledgment(m_Player, m_PendingActionAcknowledgmentID, false);
			}
			else
			{
				ActionTarget target = new ActionTarget(NULL, NULL, -1, vector.Zero, 0); 
				bool success = true;

				m_ActionWantEndRequest = false;
				m_ActionInputWantEnd = false;
					
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
						DayZPlayerSyncJunctures.SendActionInterrupt(m_Player);
					}
				}
			}
			m_PendingAction = NULL;
			m_PendingActionReciveData = NULL;
		}
	
		if (m_CurrentActionData)
		{			
			if (m_CurrentActionData.m_State != UA_AM_PENDING && m_CurrentActionData.m_State != UA_AM_REJECTED && m_CurrentActionData.m_State != UA_AM_ACCEPTED)
			{
				m_CurrentActionData.m_Action.OnUpdateServer(m_CurrentActionData);
				
				if (m_CurrentActionData.m_RefreshJunctureTimer > 0)
				{
					m_CurrentActionData.m_RefreshJunctureTimer--;
				}
				else
				{
					m_CurrentActionData.m_RefreshJunctureTimer = m_CurrentActionData.m_Action.GetRefreshReservationTimerValue();
					if ( m_CurrentActionData.m_Target )
					{
						EntityAI targetEntity;
						if ( targetEntity.CastTo(targetEntity, m_CurrentActionData.m_Target.GetObject()) && !Building.Cast(targetEntity) )
						{
							GetGame().ExtendActionJuncture(m_CurrentActionData.m_Player, targetEntity, 10000);
						}
					}
				}
			}
			
			//Debug.Log("m_CurrentActionData.m_State: " + m_CurrentActionData.m_State +" m_ActionWantEnd: " + m_ActionWantEndRequest );
			switch (m_CurrentActionData.m_State)
			{
				case UA_AM_PENDING:
					break;
			
				case UA_AM_ACCEPTED:
					// check pCurrentCommandID before start or reject 
				
					int condition_mask = ActionBase.ComputeConditionMask( m_Player, m_CurrentActionData.m_Target, m_CurrentActionData.m_MainItem );
					bool can_be_action_done = m_CurrentActionData.m_Action.Can(m_Player,m_CurrentActionData.m_Target,m_CurrentActionData.m_MainItem);
					if ( can_be_action_done && ActionPossibilityCheck(pCurrentCommandID) )
					{
						m_CurrentActionData.m_State = UA_START;
						m_CurrentActionData.m_Action.Start(m_CurrentActionData);
					
						if ( m_CurrentActionData.m_Action && m_CurrentActionData.m_Action.IsInstant() )
							OnActionEnd();
					}
					else
					{
						OnActionEnd();
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
	
	override void Interrupt()
	{
		if ( m_CurrentActionData )
			DayZPlayerSyncJunctures.SendActionInterrupt(m_Player);
	}
	
	override ActionReciveData GetReciveData()
	{
		return m_PendingActionReciveData;
	}
};