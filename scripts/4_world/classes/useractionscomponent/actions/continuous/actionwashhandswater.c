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
		m_ConditionTarget 	= new CCTLiquid(LIQUID_GROUP_WATER - LIQUID_SNOW - LIQUID_HOTWATER, UAMaxDistances.DEFAULT, UAMaxDistances.DEFAULT);
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		//! Other conditions are handled in CCTLiquid
		if (item && item.IsHeavyBehaviour()) //! Don't allow washing hands with heavy items in hands
			return false;
		
		return (player.HasBloodyHands() && !player.GetItemOnSlot("Gloves"));
	}
	
	override void OnStart(ActionData action_data)
	{
		super.OnStart(action_data);
		
		action_data.m_Player.TryHideItemInHands(true);
	}
	
	override void OnEnd(ActionData action_data)
	{
		action_data.m_Player.TryHideItemInHands(false);
	}
	
	override void OnFinishProgressServer( ActionData action_data )
	{
		PluginLifespan moduleLifespan = PluginLifespan.Cast(GetPlugin(PluginLifespan));
		moduleLifespan.UpdateBloodyHandsVisibility(action_data.m_Player, false);
		action_data.m_Player.ClearBloodyHandsPenaltyChancePerAgent(eAgents.SALMONELLA);
	}
	
	override bool IsLockTargetOnUse()
	{
		return false;
	}
}
