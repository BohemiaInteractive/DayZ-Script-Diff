class ActionWashHandsWellCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousRepeat(UATimeSpent.WASH_HANDS);
	}
}

class ActionWashHandsWell: ActionContinuousBase
{
	void ActionWashHandsWell()
	{
		m_CallbackClass 	= ActionWashHandsWellCB;
		m_CommandUID		= DayZPlayerConstants.CMD_ACTIONFB_WASHHANDSWELL;
		m_FullBody			= true;
		m_StanceMask		= DayZPlayerConstants.STANCEMASK_CROUCH;

		m_Text = "#wash_hands";
	}
	
	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}
	
	override void CreateConditionComponents()  
	{		
		m_ConditionItem		= new CCINone();
		m_ConditionTarget	= new CCTObject(UAMaxDistances.DEFAULT);
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		return player.HasBloodyHands() && !player.GetItemInHands() && target.GetObject() && (target.GetObject().GetWaterSourceObjectType() != EWaterSourceObjectType.NONE || target.GetObject().IsWell()) && !player.GetItemOnSlot("Gloves");
	}

	override void OnFinishProgressServer(ActionData action_data)
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
