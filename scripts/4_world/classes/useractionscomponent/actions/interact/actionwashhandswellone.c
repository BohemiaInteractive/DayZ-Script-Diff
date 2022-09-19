class ActionWashHandsWellOneCB : ActionInteractLoopBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAInteractLoop(UATimeSpent.WASH_HANDS);
	}
};

class ActionWashHandsWellOne extends ActionInteractLoopBase
{
	void ActionWashHandsWellOne()
	{
		m_CallbackClass		= ActionWashHandsWellOneCB;
		m_CommandUID		= DayZPlayerConstants.CMD_ACTIONFB_WASHHANDSWELL;
		m_FullBody			= true;
		m_StanceMask		= DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "#wash_hands";
	}
	
	override void CreateConditionComponents()  
	{		
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTObject(UAMaxDistances.DEFAULT);
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item )
	{	
		return player.HasBloodyHands() && !player.GetItemInHands() && !player.GetItemOnSlot("Gloves") && target.GetObject() && target.GetObject().IsWell();
	}

	override void OnEndServer( ActionData action_data )
	{
		if (action_data.m_State == UA_FINISHED)
		{
			PluginLifespan module_lifespan = PluginLifespan.Cast( GetPlugin( PluginLifespan ) );
			module_lifespan.UpdateBloodyHandsVisibility( action_data.m_Player, false );
		}
	}	
};