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

	override void OnFinishProgressServer( ActionData action_data )
	{	
		PlayerBase player = PlayerBase.Cast( action_data.m_Player );
		EntityAI item_in_hands = action_data.m_MainItem;
		if( item_in_hands )
		{
			//action_data.m_Player.RemoveQuickBarEntityShortcut(item_in_hands);
			string new_item_name = MiscGameplayFunctions.ObtainRestrainItemTargetClassname(item_in_hands);
			MiscGameplayFunctions.TurnItemIntoItemEx(player, new TurnItemIntoItemLambdaRestrainLambda(item_in_hands, new_item_name, action_data.m_Player));
		}
	}
	
	override void OnFinishProgressClient( ActionData action_data )
	{	
		//action_data.m_Player.RemoveQuickBarEntityShortcut(action_data.m_MainItem);
	}
};
