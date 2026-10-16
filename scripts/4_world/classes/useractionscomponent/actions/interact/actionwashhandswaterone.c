//!DEPRECATED
[Obsolete("1.30: Not used!")]
class ActionWashHandsWaterOneCB : ActionInteractLoopBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAInteractLoop(UATimeSpent.WASH_HANDS);
	}
};

[Obsolete("1.30: Not used and not registered in action constructor!")]
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
		m_ConditionTarget = new CCTLiquid(LIQUID_GROUP_WATER, UAMaxDistances.DEFAULT, UAMaxDistances.DEFAULT);
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		// Other conditions are in CCTLiquid
		return ( g_Game.IsMultiplayer() && g_Game.IsServer() ) || ( player.HasBloodyHands() && !player.GetItemInHands() && !player.GetItemOnSlot("Gloves") );
	}

	override void OnEndServer( ActionData action_data )
	{
		super.OnEndServer(action_data);
		
		if (action_data.m_State == UA_FINISHED)
		{
			PluginLifespan module_lifespan = PluginLifespan.Cast( GetPlugin( PluginLifespan ) );
			module_lifespan.UpdateBloodyHandsVisibility( action_data.m_Player, false );
			action_data.m_Player.ClearBloodyHandsPenaltyChancePerAgent(eAgents.SALMONELLA);
		}
	}
};