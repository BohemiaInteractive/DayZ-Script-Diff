class ActionWashHandsItemContinuousCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.WASH_HANDS);
	}
};

class ActionWashHandsItemContinuous: ActionContinuousBase
{
	void ActionWashHandsItemContinuous()
	{
		m_CallbackClass = ActionWashHandsItemContinuousCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_CLEANHANDSBOTTLE;
		m_FullBody		= false;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_CLEANHANDSBOTTLE;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_Text = "#wash_hands";
	}
	
	override bool HasProneException()
	{
		return true;
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINotRuinedAndEmpty;
		m_ConditionTarget = new CCTSelf;
	}

	override bool HasTarget()
	{
		return false;
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if ( player.GetItemOnSlot("Gloves") )
			return false;
		bool result = player.HasBloodyHands() && ( item.GetQuantity() >= item.GetDisinfectQuantity() && !item.GetIsFrozen());
		return result;
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		PluginLifespan module_lifespan = PluginLifespan.Cast( GetPlugin( PluginLifespan ) );
		module_lifespan.UpdateBloodyHandsVisibility( action_data.m_Player, false );
		action_data.m_Player.ClearBloodyHandsPenaltyChancePerAgent(eAgents.SALMONELLA);
		action_data.m_MainItem.AddQuantity( -action_data.m_MainItem.GetDisinfectQuantity(), false );
	}
};