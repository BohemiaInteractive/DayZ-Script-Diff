class ActionRestrainSelfCB : ActionContinuousBaseCB
{
	const float DEFAULT_RESTRAIN_TIME = 2;
	
	override void CreateActionComponent()
	{
		float time = DEFAULT_RESTRAIN_TIME;
		
		if( m_ActionData.m_MainItem.ConfigIsExisting("RestrainTime") )
		{
			time = m_ActionData.m_MainItem.ConfigGetFloat("RestrainTime");
		}
		
		if( m_ActionData.m_Player.IsQuickRestrain() )
		{
			time = DEBUG_QUICK_UNRESTRAIN_TIME;
		}
		
		m_ActionData.m_ActionComponent = new CAContinuousTime(time);
	}
};

class ActionRestrainSelf: ActionContinuousBase
{	
	void ActionRestrainSelf()
	{
		m_CallbackClass = ActionRestrainSelfCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_RESTRAINSELF;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		//m_Animation = "INJECTEPIPENS";
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_LOW;
		m_Text = "#restrain_self";
	}
	
	override int GetStanceMask(PlayerBase player)
	{
		if ( player.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_PRONE))
			return DayZPlayerConstants.STANCEMASK_CROUCH;
		else
			return DayZPlayerConstants.STANCEMASK_ERECT;
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTSelf;
	}

	override bool HasTarget()
	{
		return false;
	}
	
	override bool HasAlternativeInterrupt()
	{
		return true;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		if(player.IsRestrained())
		{
			return false;
		}
	
		return true;
	}
	
	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);
		
		action_data.m_Player.SetRestrainStarted(true);
	}

	override void OnFinishProgressServer( ActionData action_data )
	{	
		PlayerBase player = PlayerBase.Cast( action_data.m_Player );
		EntityAI item_in_hands = action_data.m_MainItem;
		if (item_in_hands)
		{
			ClearInventoryReservationEx(action_data);
			//action_data.m_Player.RemoveQuickBarEntityShortcut(item_in_hands);
			string new_item_name = MiscGameplayFunctions.ObtainRestrainItemTargetClassname(item_in_hands);
			TurnItemIntoItemLambdaRestrainLambda lambda = new TurnItemIntoItemLambdaRestrainLambda(item_in_hands, new_item_name, action_data.m_Player);
			MiscGameplayFunctions.TurnItemIntoItemEx(player, lambda);
		}
	}
	
	override void OnFinishProgressClient( ActionData action_data )
	{	
		//action_data.m_Player.RemoveQuickBarEntityShortcut(action_data.m_MainItem);
	}
	
	override void OnEndServer(ActionData action_data)
	{
		super.OnEndServer(action_data);
		
		action_data.m_Player.SetRestrainStarted(false);
	}
};
