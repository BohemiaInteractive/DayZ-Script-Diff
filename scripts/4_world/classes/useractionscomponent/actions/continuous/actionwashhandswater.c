class ActionWashHandsWaterCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousRepeat(UATimeSpent.WASH_HANDS);
	}
}

class ActionWashHandsWater : ActionContinuousBase
{
	void ActionWashHandsWater()
	{
		m_CallbackClass	= ActionWashHandsWaterCB;
		m_CommandUID	= DayZPlayerConstants.CMD_ACTIONFB_WASHHANDSPOND;
		m_FullBody		= true;
		m_StanceMask	= DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text 			= "#wash_hands";
	}
	
	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}

	override void CreateConditionComponents()
	{		
		m_ConditionItem		= new CCINone();
		m_ConditionTarget 	= new CCTWaterSurfaceEx(UAMaxDistances.DEFAULT, LIQUID_GROUP_WATER - LIQUID_SNOW);
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		// Other conditions are in CCTWaterSurface
		return (player.HasBloodyHands() && !player.GetItemInHands() && !player.GetItemOnSlot("Gloves"));
	}
	
	override void OnFinishProgressServer( ActionData action_data )
	{
		PluginLifespan moduleLifespan = PluginLifespan.Cast(GetPlugin(PluginLifespan));
		moduleLifespan.UpdateBloodyHandsVisibility(action_data.m_Player, false);
		action_data.m_Player.ClearBloodyHandsPenaltyChancePerAgent(eAgents.SALMONELLA);
	}
}
