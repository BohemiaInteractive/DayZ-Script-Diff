enum ActionInputType
{
	AIT_CONTINUOUS,				//React to hold input and release after it
	AIT_SINGLE,					//React to click input - single use actions
	AIT_DOUBLECLICK,			//React to double click input - single use actions
	AIT_HOLDSINGLE,				//React to hold input - single use actions
	AIT_CLICKCONTINUOUS,		//React to click input for start and click for end
	AIT_NOINPUTCONTROL,
	AIT_INVENTORYINPUT,			//Inventory specific actions
}

class ForcedActionData
{
	ActionBase m_Action;
	ref ActionTarget m_Target;
	ItemBase m_Item;
}

class ActionInput : ActionInput_Basic
{
	UAIDWrapper m_input;			//Input ID automaticly generated from name.
	int m_InputType;			//Use from action manager for handling input
	int m_Priority;
	bool m_DetectFromItem;		//Can be used action from item in hands?
	bool m_DetectFromTarget;	//Can be used action from target in vicinity?
	bool m_DetectFromPlayer;	//Can be used action from player?
	bool m_Enabled;
	bool m_JustActivate;
	bool m_HasTarget;
	
	PlayerBase m_Player;
	
	ref ActionTarget m_ForcedTarget;
	
	ref ForcedActionData m_ForcedActionData;
	
	ref ActionTarget m_Target;
	ItemBase m_MainItem;
	int m_ConditionMask;
	
	bool m_Active;
	
	void ActionInput()
	{
		m_Active = false;
		m_Enabled = true;
		m_ForcedTarget = null;
		m_ForcedActionData = null;
		
		m_HasTarget = false;
		
		m_JustActivate = false;
		m_DetectFromTarget = false;
		m_DetectFromItem = true;
		m_DetectFromPlayer = true;
		
		m_Priority = 100;
	}
	
	void Init(PlayerBase player, ActionManagerClient am)
	{
		m_Player = player;
		
		if ( LogManager.IsActionLogEnable())
		{
			Debug.ActionLog("n/a",this.ToString(), "n/a","Init()", player.ToString());
		}
		
	}
	
	void SetEnablity(bool value)
	{
		m_Enabled = value;
	}
	
	protected void SetInput(string input_name)
	{
		m_input = GetUApi().GetInputByName(input_name).GetPersistentWrapper();
		
		if ( LogManager.IsActionLogEnable())
		{
			if (m_input && m_input.InputP())
				Debug.ActionLog("(+) input set to " + input_name ,this.ToString(), "n/a","SetInput()", "n/a");
			else
				Debug.ActionLog("(-) input is not set to " + input_name ,this.ToString(), "n/a","SetInput()","n/a");
		}
	}
	
	int GetInputType()
	{
		return m_InputType;
	}
	
	UAInput GetUAInput()
	{
		return m_input.InputP();
	}
	
	bool JustActivate()
	{
		return m_JustActivate;
	}

	bool IsActive()
	{
		return m_Active;
	}
		
	bool WasEnded()
	{
		return !m_Active;
	}
	
	void Update()
	{
		if(	!m_Enabled )
		{
			m_Active = false;
			m_JustActivate = false;
			return;
		}
		
		switch ( m_InputType )
		{
			case ActionInputType.AIT_CONTINUOUS:
				m_JustActivate = false;
				if(m_Active)
				{
					m_Active = m_input.InputP().LocalHold();
				}
				else
				{
					m_Active = m_input.InputP().LocalHoldBegin();
					m_JustActivate = m_Active;
				}
				break;
			case ActionInputType.AIT_SINGLE:
				m_Active = m_input.InputP().LocalClick();
				m_JustActivate = m_Active;
				break;
			case ActionInputType.AIT_DOUBLECLICK:
				m_Active = m_input.InputP().LocalDoubleClick();
				m_JustActivate = m_Active;
				break;
			break;
			case ActionInputType.AIT_HOLDSINGLE:
				m_Active = m_input.InputP().LocalHoldBegin();
				m_JustActivate = m_Active;
				break;
			break;
			case ActionInputType.AIT_CLICKCONTINUOUS:
				m_JustActivate = false;
				if(m_Active)
				{
					if ( m_input.InputP().LocalClick() )
					{
						m_Active = false;
					}
				}
				else
				{
					m_Active = m_input.InputP().LocalClick();
					m_JustActivate = m_Active;
				}
				break;
			break;
			default:
				break;
		}
	}
	
	void Reset()
	{
		m_Active = false;
		ActionsSelectReset();
	}
	
	void UpdatePossibleActions(PlayerBase player, ActionTarget target, ItemBase item, int action_condition_mask)
	{	
	}
	
	ActionBase GetAction()
	{
		return NULL;
	}
	
	ActionTarget GetUsedActionTarget()
	{
		return m_Target;
	}
	
	ItemBase GetUsedMainItem()
	{
		return m_MainItem;
	}
	
	array<ActionBase> GetPossibleActions()
	{
		return NULL;
	}
	
	ActionBase GetPossibleAction()
	{
		return NULL;
	}
	
	int GetPossibleActionIndex()
	{
		return -1;
	}
	
	bool HasTarget()
	{
		return false;
	}
	
	void OnActionStart()
	{
		ActionsSelectReset();
	}
	
	void OnActionEnd()
	{
		Reset();
	}
	
	void ActionsSelectReset()
	{}
	
	void ForceAction(ActionBase action, ActionTarget target, ItemBase item )
	{
		m_ForcedActionData = new ForcedActionData;
		m_ForcedActionData.m_Action = action;
		m_ForcedActionData.m_Target = target;
		m_ForcedActionData.m_Item = item;
	}
	
	void ForceActionTarget( ActionTarget target )
	{
		m_ForcedTarget = target;
	}
	
	void ClearForcedAction()
	{
		m_ForcedActionData = null;
	}
	
	void ClearForcedTarget()
	{
		m_ForcedTarget = NULL;
	}
	
	bool ForceActionCheck(PlayerBase player)
	{
		if (m_ForcedActionData)
		{
			if (m_ForcedActionData.m_Action.Can(player, m_ForcedActionData.m_Target, m_ForcedActionData.m_Item))
			{
				m_MainItem = m_ForcedActionData.m_Item;
				m_Target = m_ForcedActionData.m_Target;
				return true;
			}
		}
		return false;
	}
	
	void SelectNextAction()
	{
	}
	
	void SelectPrevAction()
	{}
	
	int GetPossibleActionsCount()
	{
		return -1;
	}
	
	bool HasInput()
	{
		return m_input != NULL;
	}
	
	int GetPriority()
	{
		return m_Priority;
	}
}

class StandardActionInput : ActionInput
{
	ref array<ActionBase>	m_SelectActions;
	int 					m_selectedActionIndex;
	
	void StandardActionInput(PlayerBase player)
	{
		m_selectedActionIndex = 0;
		m_SelectActions = new array<ActionBase>;
	}
	
	override void ForceAction(ActionBase action, ActionTarget target, ItemBase item )
	{
		super.ForceAction(action, target, item);

		m_selectedActionIndex = 0;
	}
	
	void _GetSelectedActions( Object action_source_object, out array<ActionBase> select_actions_all, out bool has_any_action_target)
	{
		if ( !action_source_object )
			return;
		
		array<ActionBase_Basic> possible_actions;
		array<ref ActionBase> variant_actions;
		action_source_object.GetActions(this.Type(), possible_actions);
		ActionBase action;
		
		if(possible_actions)
		{
			for (int i = 0; i < possible_actions.Count(); i++)
			{
				action = ActionBase.Cast(possible_actions.Get(i));
						
				if ( action.HasVariants() )
				{
					action.UpdateVariants( m_MainItem, m_Target.GetObject(), m_Target.GetComponentIndex() );
					action.GetVariants( variant_actions);
					for (int j = 0; j < variant_actions.Count(); j++)
					{
						action = variant_actions[j];
						if ( action.Can(m_Player, m_Target, m_MainItem, m_ConditionMask) )
						{
							select_actions_all.Insert(action);
							if (action.HasTarget())
								has_any_action_target = true;
						}
					}
				}
				else
				{
					if ( action.Can(m_Player, m_Target, m_MainItem, m_ConditionMask) )
					{
						select_actions_all.Insert(action);
						if (action.HasTarget())
							has_any_action_target = true;
					}
				}
			}
		}
	}
	
	override void UpdatePossibleActions(PlayerBase player, ActionTarget target, ItemBase item, int action_condition_mask)
	{
		if ( ForceActionCheck(player))
		{
			m_SelectActions.Clear();
			m_SelectActions.Insert(m_ForcedActionData.m_Action);
			return;
		}
		
		if(m_ForcedTarget)
		{
			target = m_ForcedTarget;
		}

		array<ActionBase> select_actions_all = new array<ActionBase>;
		int i, last_index;
		bool change = false;
		m_HasTarget = false;
		
		m_MainItem = item;
		m_Target = target;
		m_ConditionMask = action_condition_mask;
		
		
		if( m_DetectFromItem )
		{
			_GetSelectedActions( item, select_actions_all, m_HasTarget);
		}
		

		if( m_DetectFromTarget )
		{	
			_GetSelectedActions( target.GetObject(), select_actions_all, m_HasTarget);
			_GetSelectedActions( target.GetParent(), select_actions_all, m_HasTarget);
		}
		
		if( m_DetectFromPlayer )
		{
			_GetSelectedActions( player, select_actions_all, m_HasTarget);
		}
		
		if ( select_actions_all.Count() )
		{
			last_index = 0;
			for ( i = 0; i < select_actions_all.Count(); i++ )
			{
				ActionBase action = select_actions_all[i];
				if ( m_HasTarget )
				{
					if ( action.HasTarget() )
					{
						if ( m_SelectActions.Count() > last_index )
						{
							if ( m_SelectActions[last_index] != action )
							{
								change = true;
								m_SelectActions[last_index] = action;
							}
						}
						else
						{
							change = true;
							m_SelectActions.Insert(action);
						}
						action.OnActionInfoUpdate(player, target, item);
						last_index++;
					}
				}
				else
				{
					if ( m_SelectActions.Count() > last_index )
					{
						if ( m_SelectActions[last_index] != action )
						{
							change = true;
							m_SelectActions[last_index++] = action;
						}
					}
					else
					{
						change = true;
						m_SelectActions.Insert(action);
					}
					action.OnActionInfoUpdate(player, target, item);
					last_index++;
				}
			}
		}
		else
		{
			m_selectedActionIndex = 0;
			m_SelectActions.Clear();
		}
		
		if (m_SelectActions.Count() > last_index)
		{
			change = true;
			for (i = last_index; i < m_SelectActions.Count(); i++)
			{
				m_SelectActions.Remove(last_index);
			}
		}
		
		if ( change )
		{
			m_selectedActionIndex = 0;
			m_Player.GetActionManager().SelectFirstActionCategory();
		}
	}
	
	override array<ActionBase> GetPossibleActions()
	{
		return m_SelectActions;
	}
	
	override int GetPossibleActionsCount()
	{
		return m_SelectActions.Count();
	}
	
	override bool HasTarget()
	{
		return m_HasTarget;
	}
	
	override int GetPossibleActionIndex()
	{
		return m_selectedActionIndex;
	}
	
	override ActionBase GetAction()
	{
		if (m_SelectActions.Count() > 0)
			return m_SelectActions[m_selectedActionIndex];
		return null;
	}
	
	override void ActionsSelectReset()
	{
		m_SelectActions.Clear();
		m_selectedActionIndex = 0;
	}
	
	override void SelectNextAction()
	{
		if(m_SelectActions.Count())
		{
			m_selectedActionIndex++;
			if(m_SelectActions.Count() <= m_selectedActionIndex )
			{
				m_selectedActionIndex = 0;
			}
		}
	}
	
	override void SelectPrevAction()
	{
		if(m_SelectActions.Count())
		{
			m_selectedActionIndex--;
			if(0 > m_selectedActionIndex )
			{
				m_selectedActionIndex = m_SelectActions.Count() - 1;
			}
		}
	}

}

class ContinuousInteractActionInput : StandardActionInput
{
	void ContinuousInteractActionInput(PlayerBase player)
	{
		SetInput("UAAction");
		m_Priority = 60;
		m_InputType = ActionInputType.AIT_CONTINUOUS;
		m_DetectFromTarget = true;
		m_DetectFromItem = false;
		m_DetectFromPlayer = true;
	}
};

class InteractActionInput : ContinuousInteractActionInput
{
	void InteractActionInput(PlayerBase player)
	{
		m_InputType = ActionInputType.AIT_SINGLE;
		m_Priority = 80;
	}
	
	override void OnActionStart()
	{
		super.OnActionStart();
		m_Active = false;
	}
	
	override bool WasEnded()
	{
		return false;
	}
};

class NoIndicationActionInputBase : ActionInput
{
	void NoIndicationActionInputBase(PlayerBase player)
	{
		m_DetectFromTarget = false;
	}
	
	override void UpdatePossibleActions(PlayerBase player, ActionTarget target, ItemBase item, int action_condition_mask)
	{
		m_MainItem = item;
		m_Target = target;
		m_ConditionMask = action_condition_mask;
	}
	
	override ActionBase GetAction()
	{
		if ( ForceActionCheck(m_Player) )
			return m_ForcedActionData.m_Action;
		
		if(m_MainItem)
		{
			array<ActionBase_Basic> possible_actions;
			ActionBase action;
			
			m_MainItem.GetActions(this.Type(), possible_actions);
			if(possible_actions)
			{
				for (int i = 0; i < possible_actions.Count(); i++)
				{
					action = ActionBase.Cast(possible_actions.Get(i));
					if ( action.Can(m_Player, m_Target, m_MainItem, m_ConditionMask) )
					{
						return action;
					}
				}
			}
		
		}
		return NULL;
	}

	override ActionBase GetPossibleAction()
	{
		return GetAction();
	}
}



class ContinuousDefaultActionInput : StandardActionInput
{
	protected ActionBase m_SelectAction; //Do nothing only for back compatibilit, relevant only for inherited class
	void ContinuousDefaultActionInput(PlayerBase player)
	{
		SetInput("UADefaultAction");
		m_InputType = ActionInputType.AIT_CONTINUOUS;
		m_Priority = 20;
		m_DetectFromTarget = false;
		m_DetectFromItem = true;
		m_DetectFromPlayer = true;
		
		m_SelectAction = null;
	}
	
	override ActionBase GetPossibleAction()
	{
		return m_SelectAction;
	}
	
	override void ActionsSelectReset()
	{
		super.ActionsSelectReset();
		m_SelectAction = NULL;
	}
};

class DefaultActionInput : ContinuousDefaultActionInput
{
	void DefaultActionInput(PlayerBase player)
	{
		m_InputType = ActionInputType.AIT_SINGLE;
		m_Priority = 40;
	}
	
	override void OnActionStart()
	{
		super.OnActionStart();
		m_Active = false;
	}
	
	override bool WasEnded()
	{
		return false;
	}
};

class DropActionInput : NoIndicationActionInputBase
{
	void DropActionInput(PlayerBase player)
	{
		SetInput("UADropitem");
		m_InputType = ActionInputType.AIT_HOLDSINGLE;
	}
};

class CarHornShortActionInput : ContinuousDefaultActionInput
{
	ref ActionTarget targetNew;

	void CarHornShortActionInput(PlayerBase player)
	{
		SetInput("UACarHorn");
		m_InputType 		= ActionInputType.AIT_SINGLE;
		m_Priority 			= 100;
		m_DetectFromItem 	= false;
		m_DetectFromTarget	= false;
		m_DetectFromPlayer	= false;
	}
	
	override void UpdatePossibleActions(PlayerBase player, ActionTarget target, ItemBase item, int action_condition_mask)
	{
		if (ForceActionCheck(player))
		{
			m_SelectAction = m_ForcedActionData.m_Action;
			return;
		}
		
		m_SelectAction = null;
		array<ActionBase_Basic> possibleActions;
		ActionBase action;
		int i;

		m_MainItem = null;
		if (player && player.IsInVehicle())
		{
			HumanCommandVehicle vehCommand = player.GetCommand_Vehicle();
			if (vehCommand)
			{
				Transport trans = vehCommand.GetTransport();
				if (trans)
				{
					targetNew = new ActionTarget(trans, null, -1, vector.Zero, -1);
					ForceActionTarget(targetNew);
				}
			}
			
			if (!targetNew)
			{
				ClearForcedTarget();
			}
		}
		
		target = m_ForcedTarget;
		m_Target = m_ForcedTarget;
		
		if (target && target.GetObject())
		{
			target.GetObject().GetActions(this.Type(), possibleActions);
			if (possibleActions)
			{
				for (i = 0; i < possibleActions.Count(); i++)
				{
					action = ActionBase.Cast(possibleActions.Get(i));
					if (action.Can(player, target, m_MainItem, action_condition_mask))
					{
						m_SelectAction = action;
						return;
					}
				}
			}
		}
	}
	
	override ActionBase GetAction()
	{
		return m_SelectAction;
	}
}

class CarHornLongActionInput : CarHornShortActionInput
{
	void CarHornLongActionInput(PlayerBase player)
	{
		SetInput("UACarHorn");
		m_InputType 		= ActionInputType.AIT_CONTINUOUS;
		m_Priority 			= 101;
		m_DetectFromItem 	= false;
		m_DetectFromTarget	= false;
		m_DetectFromPlayer	= false;
	}
}

class ToggleLightsActionInput : DefaultActionInput
{
	ref ActionTarget target_new;
	
	void ToggleLightsActionInput(PlayerBase player)
	{
		SetInput("UAToggleHeadlight");
		m_InputType = ActionInputType.AIT_SINGLE;
		m_Priority = 100;
	}
	
	override void UpdatePossibleActions(PlayerBase player, ActionTarget target, ItemBase item, int action_condition_mask)
	{
		if( ForceActionCheck(player) )
		{
			m_SelectAction = m_ForcedActionData.m_Action;
			return;
		}
		//ForceActionTarget(player.m_PlayerLightManager.
		
		m_SelectAction = NULL;
		array<ActionBase_Basic> possible_actions;
		ActionBase action;
		int i;

		m_MainItem = NULL;
		if ( player && !player.IsInVehicle() ) 
		{
			Clothing headgear = Clothing.Cast(player.FindAttachmentBySlotName("Headgear"));
			Clothing eyewear = Clothing.Cast(player.FindAttachmentBySlotName("Eyewear"));
			//TODO - extend this to allow for a switchable control over all possible light sources (depth 0 or 1 only?)
			
			if ( headgear && headgear.GetLightSourceItem() )
			{
				target_new = new ActionTarget(headgear, null, -1, vector.Zero, -1);
				ForceActionTarget(target_new);
			}
			else if ( eyewear && eyewear.GetLightSourceItem() )
			{
				target_new = new ActionTarget(eyewear, null, -1, vector.Zero, -1);
				ForceActionTarget(target_new);
			}
			else
				ClearForcedTarget();
		}
		else if ( player && player.IsInVehicle() )
		{
			HumanCommandVehicle vehCommand = player.GetCommand_Vehicle();
			if( vehCommand )
			{
				Transport trans = vehCommand.GetTransport();
				if( trans )
				{
					target_new = new ActionTarget(trans, null, -1, vector.Zero, -1);
					ForceActionTarget(target_new);
				}
			}
			
			if ( !target_new )
				ClearForcedTarget();
		}
		
		target = m_ForcedTarget;
		m_Target = m_ForcedTarget;
		
		if(target && target.GetObject())
		{
			target.GetObject().GetActions(this.Type(), possible_actions);
			if(possible_actions)
			{
				for (i = 0; i < possible_actions.Count(); i++)
				{
					action = ActionBase.Cast(possible_actions.Get(i));
					if ( action.Can(player, target, m_MainItem, action_condition_mask) )
					{
						m_SelectAction = action;
						return;
					}
				}
			}
		}
	}
	
	override ActionBase GetAction()
	{
		return m_SelectAction;
	}
};

class ToggleNVGActionInput : DefaultActionInput
{
	ref ActionTarget target_new;
	
	void ToggleNVGActionInput(PlayerBase player)
	{
		SetInput("UAToggleNVG");
		m_InputType = ActionInputType.AIT_HOLDSINGLE;
		m_Priority = 100;
	}
	
	override void UpdatePossibleActions(PlayerBase player, ActionTarget target, ItemBase item, int action_condition_mask)
	{
		if( ForceActionCheck(player) )
		{
			m_SelectAction = m_ForcedActionData.m_Action;
			return;
		}
		//ForceActionTarget(player.m_PlayerLightManager.
		
		m_SelectAction = NULL;
		array<ActionBase_Basic> possible_actions;
		ActionBase action;
		int i;

		m_MainItem = NULL;
		if ( player ) 
		{
			Mich2001Helmet helmet = Mich2001Helmet.Cast(player.FindAttachmentBySlotName("Headgear"));
			NVGHeadstrap headstrap = NVGHeadstrap.Cast(player.FindAttachmentBySlotName("Eyewear"));
			if ( helmet )
			{
				//m_MainItem = Headtorch_ColorBase.Cast(player.FindAttachmentBySlotName("Headgear"));
				target_new = new ActionTarget(helmet, null, -1, vector.Zero, -1);
				ForceActionTarget(target_new);
			}
			else if ( headstrap )
			{
				target_new = new ActionTarget(headstrap, null, -1, vector.Zero, -1);
				ForceActionTarget(target_new);
			}
			else
				ClearForcedTarget();
		}
		
		target = m_ForcedTarget;
		m_Target = m_ForcedTarget;
		
		if(target && target.GetObject())
		{
			target.GetObject().GetActions(this.Type(), possible_actions);
			if(possible_actions)
			{
				for (i = 0; i < possible_actions.Count(); i++)
				{
					action = ActionBase.Cast(possible_actions.Get(i));
					if ( action.Can(player, target, m_MainItem, action_condition_mask) )
					{
						m_SelectAction = action;
						return;
					}
				}
			}
		}
	}
	
	override ActionBase GetAction()
	{
		return m_SelectAction;
	}
};

class ContinuousWeaponManipulationActionInput : NoIndicationActionInputBase
{
	void ContinuousWeaponManipulationActionInput(PlayerBase player)
	{
		SetInput("UAReloadMagazine");
		m_InputType = ActionInputType.AIT_CONTINUOUS;
	}
};

class WeaponManipulationActionInput : NoIndicationActionInputBase
{	
	void WeaponManipulationActionInput(PlayerBase player)
	{
		SetInput("UAReloadMagazine");
		m_InputType = ActionInputType.AIT_SINGLE;
	}
};

class ExternalControlledActionInput : NoIndicationActionInputBase
{	
	void ExternalControlledActionInput(PlayerBase player)
	{
		//SetInput("UAReloadMagazine");
		m_InputType = ActionInputType.AIT_NOINPUTCONTROL;
	}
};

class InventoryOnlyActionInput : NoIndicationActionInputBase
{	
	void InventoryOnlyActionInput(PlayerBase player)
	{
		//SetInput("UAReloadMagazine");
		m_InputType = ActionInputType.AIT_INVENTORYINPUT;
	}
};

class QuickaBarActionInput : ExternalControlledActionInput
{	
};


