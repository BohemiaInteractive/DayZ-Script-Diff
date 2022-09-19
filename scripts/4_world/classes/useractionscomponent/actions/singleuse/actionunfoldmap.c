class ActionUnfoldMapCB : ActionBaseCB
{
	bool 								m_MapFolding;
	bool 								m_HasReceivedEvent;
	bool 								m_CancelCondition;
	int 								m_InitMovementState;
	int 								m_FinalMovementState;
	
	void ActionUnfoldMapCB()
	{
		RegisterAnimationEvent("ActionExec", UA_ANIM_EVENT);
		EnableStateChangeCallback();
		EnableCancelCondition(true);
	}
	
	void ~ActionUnfoldMapCB()
	{
		if (!CfgGameplayHandler.GetUse3DMap())
		{
			GetGame().GetMission().RemoveActiveInputExcludes({"map"}, false);
		}
		else
		{
			GetGame().GetMission().RemoveActiveInputExcludes({"loopedactions"}, false);
		}
		GetGame().GetMission().RemoveActiveInputRestriction(EInputRestrictors.MAP);
		
		if (!m_ActionData || !m_ActionData.m_Player )
			return;
		
		ItemMap chernomap = ItemMap.Cast(m_ActionData.m_Player.GetItemInHands());
		if (chernomap && m_ActionData.m_Player.IsMapOpen())
		{
			chernomap.SetMapStateOpen(false, m_ActionData.m_Player);
		}
	}
	
	override void SetActionData(ActionData action_data )
	{
		m_ActionData = action_data;
		m_InitMovementState = m_ActionData.m_Player.m_MovementState.m_iStanceIdx;
	}
	
	bool CancelCondition( bool pEnable )
	{
		return m_CancelCondition;
	}
	
	override void OnStateChange(int pOldState, int pCurrentState)
	{
		if (!m_ActionData || !m_ActionData.m_Player)
			return;
				
		if ( ( pOldState == STATE_LOOP_IN && pCurrentState == STATE_LOOP_LOOP ) && !m_HasReceivedEvent )
		{
			m_CancelCondition = true;
		}
	}
	
	override void OnAnimationEvent(int pEventID)
	{
		if (!m_ActionData || !m_ActionData.m_Player)
			return;
		
		switch (pEventID)
		{
			case UA_ANIM_EVENT: 
				m_HasReceivedEvent = true;
				m_FinalMovementState = m_ActionData.m_Player.m_MovementState.m_iStanceIdx;
				
				if ( StateCheck() )
				{
					PerformMapChange();
					m_CancelCondition = false; 
				}
				else
				{
					m_CancelCondition = true; 
				}
			break;
		}
	}
	
	override void OnFinish(bool pCanceled)	
	{
		//super.OnFinish(pCanceled);
		
		//Print(pCanceled);
		if ( m_ActionData && m_ActionData.m_Player )
		{
			if ( m_ActionData && m_ActionData.m_ActionComponent )
				m_ActionData.m_State = m_ActionData.m_ActionComponent.Interrupt(m_ActionData);
			
			if ( (!GetGame().IsDedicatedServer()) && GetGame().GetUIManager() && GetGame().GetUIManager().IsMenuOpen(MENU_MAP) )
			{
				GetGame().GetUIManager().FindMenu(MENU_MAP).Close();
			}
			
			ActionUnfoldMap action = ActionUnfoldMap.Cast(m_ActionData.m_Action);
			m_InitMovementState = -1;
			m_FinalMovementState = -1;
			
			m_ActionData.m_Player.SetMapClosingSyncSet(false); //reset
			
			ItemMap chernomap = ItemMap.Cast(m_ActionData.m_Player.GetItemInHands());
			if (chernomap && m_ActionData.m_Player.IsMapOpen())
			{
				chernomap.SetMapStateOpen(false, m_ActionData.m_Player);
			}
			
			if (action)
				action.End(m_ActionData);
		}
	}
	
	void PerformMapChange()
	{
		if (m_ActionData.m_Player.IsSwimming() || m_ActionData.m_Player.IsClimbing() || m_ActionData.m_Player.IsFalling() || m_ActionData.m_Player.IsClimbingLadder() || m_ActionData.m_Player.IsUnconscious() || m_ActionData.m_Player.IsRestrained())
			return;
		
		if ( m_CancelCondition ) { return; }
		
		ItemMap chernomap = ItemMap.Cast(m_ActionData.m_Player.GetItemInHands());
		if (chernomap && !m_ActionData.m_Player.IsMapOpen() && !m_MapFolding)
		{
			chernomap.SetMapStateOpen(true, m_ActionData.m_Player);
	
			if (!GetGame().IsMultiplayer() || GetGame().IsServer())
			{
				//chernomap.SyncMapMarkers();
			}
			if (!GetGame().IsDedicatedServer())
			{
				UIManager 	m_UIManager;
				UIScriptedMenu 	map_menu;
				m_UIManager = GetGame().GetUIManager();
				m_UIManager.CloseAll();
				if ( !CfgGameplayHandler.GetUse3DMap() )
				{
					map_menu = m_UIManager.EnterScriptedMenu(MENU_MAP, NULL);
					map_menu.InitMapItem(chernomap);
					map_menu.LoadMapMarkers();
					GetGame().GetMission().AddActiveInputExcludes({"map"});
				}
				else
				{
					GetGame().GetMission().AddActiveInputExcludes({"loopedactions"});
				}
				GetGame().GetMission().AddActiveInputRestriction(EInputRestrictors.MAP);
			}			
		}
		else if (chernomap && m_ActionData.m_Player.IsMapOpen())
		{
			chernomap.SetMapStateOpen(false, m_ActionData.m_Player);
			m_MapFolding = true;
			m_ActionData.m_Player.SetMapOpen(false);
		}
	}
	
	bool StateCheck()
	{
		return m_InitMovementState == m_FinalMovementState;
	}
}

class ActionUnfoldMap: ActionBase
{
	void ActionUnfoldMap()
	{
		m_Text = "#unfold_map";
	}
	
	override bool IsInstant()
	{
		return false;
	}
	
	override void CreateConditionComponents()
	{
		m_ConditionItem 	= new CCINonRuined();
		m_ConditionTarget 	= new CCTNone();
	}

	override bool HasTarget()
	{
		return false;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (player.m_hac || player.IsMapOpen())
		{
			return false;
		}
		return true;
	}
	
	override int GetActionCategory()
	{
		return AC_SINGLE_USE;
	}
	
	override void OnStartClient(ActionData action_data)
	{
		OpenMap(action_data);
	}
	
	override void OnStartServer( ActionData action_data )
	{
		OpenMap(action_data);
		ItemMap chernomap = ItemMap.Cast(action_data.m_MainItem);
		chernomap.SyncMapMarkers();
	}
	
	override void OnEndRequest(ActionData action_data)
	{
		action_data.m_Callback.InternalCommand(DayZPlayerConstants.CMD_ACTIONINT_END);
		action_data.m_State = UA_FINISHED;
	}
	
	override void Interrupt(ActionData action_data)
	{
		if ( action_data.m_Player.m_hac )
		{
			action_data.m_Player.m_hac.m_MapFolding = true;
			action_data.m_Player.m_hac.PerformMapChange();
		}
		
		super.Interrupt(action_data);
		
		if ( action_data.m_Callback )
		{
			action_data.m_Callback.Cancel();
		}
		else
		{
			End( action_data );
		}
		
		action_data.m_State = UA_INTERRUPT;
	}
	
	void OpenMap( ActionData action_data )
	{
		if (!action_data.m_Player.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_PRONE))
		{
			action_data.m_Player.m_hac = ActionUnfoldMapCB.Cast(action_data.m_Player.AddCommandModifier_Action(DayZPlayerConstants.CMD_ACTIONMOD_VIEWMAP,ActionUnfoldMapCB));
		}
		else
		{
			action_data.m_Player.m_hac = ActionUnfoldMapCB.Cast(action_data.m_Player.StartCommand_Action(DayZPlayerConstants.CMD_ACTIONFB_VIEWMAP,ActionUnfoldMapCB,GetStanceMask(action_data.m_Player)));
		}
		
		// sets player for associated callback to use
		if (action_data.m_Player.m_hac)
		{
			action_data.m_Player.m_hac.SetActionData(action_data);
			action_data.m_Callback = action_data.m_Player.m_hac;
		}
	}
};