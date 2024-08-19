class ActionForceConsumeCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousQuantityEdible(UAQuantityConsumed.DEFAULT, UATimeSpent.DEFAULT);
	}
}

class ActionForceConsume : ActionContinuousBase
{
	void ActionForceConsume()
	{
		m_CallbackClass = ActionForceConsumeCB;
		m_CommandUID 	= DayZPlayerConstants.CMD_ACTIONFB_FORCEFEED;
		m_StanceMask 	= DayZPlayerConstants.STANCEIDX_ERECT | DayZPlayerConstants.STANCEIDX_CROUCH;
		m_FullBody 		= true;
		m_Text 			= "#feed";
	}

	override void CreateConditionComponents()  
	{
		m_ConditionTarget 	= new CCTMan(UAMaxDistances.DEFAULT);
		m_ConditionItem 	= new CCINonRuined();
	}
	
	override void OnEndServer(ActionData action_data)
	{	
		if (action_data.m_MainItem && (action_data.m_MainItem.GetQuantity() <= 0.01))
		{
			action_data.m_MainItem.SetQuantity(0);
		}
	
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!super.ActionCondition(player, target, item))
			return false;

		return item.GetQuantity() > item.GetQuantityMin();
	}
}
