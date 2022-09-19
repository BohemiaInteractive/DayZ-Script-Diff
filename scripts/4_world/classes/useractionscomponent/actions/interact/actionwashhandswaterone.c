class ActionWashHandsWaterOneCB : ActionInteractLoopBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAInteractLoop(UATimeSpent.WASH_HANDS);
	}
};

class ActionWashHandsWaterOne extends ActionInteractLoopBase
{
	void ActionWashHandsWaterOne()
	{
		m_CallbackClass		= ActionWashHandsWaterOneCB;
		m_CommandUID		= DayZPlayerConstants.CMD_ACTIONFB_WASHHANDSPOND;
		m_FullBody			= true;
		m_StanceMask		= DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "#wash_hands";
	}

	override void CreateConditionComponents()  
	{		
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTWaterSurface(UAMaxDistances.DEFAULT, UAWaterType.ALL);
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		// Other conditions are in CCTWaterSurface
		return ( GetGame().IsMultiplayer() && GetGame().IsServer() ) || ( player.HasBloodyHands() && !player.GetItemInHands() && !player.GetItemOnSlot("Gloves") );
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