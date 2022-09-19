class ActionForceFeedMeatCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousQuantityEdible(UAQuantityConsumed.EAT_NORMAL,UATimeSpent.DEFAULT);
	}
};

class ActionForceFeedMeat: ActionForceConsume
{
	void ActionForceFeedMeat()
	{
		m_CallbackClass = ActionForceFeedMeatCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_FORCEFEED;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "#feed";
	}
	
	override void CreateConditionComponents() 
	{
		m_ConditionTarget = new CCTMan(UAMaxDistances.DEFAULT);
		m_ConditionItem = new CCINonRuined;
	}
};