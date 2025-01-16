class ActionForceFeedCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousQuantityEdible(UAQuantityConsumed.EAT_NORMAL, UATimeSpent.DEFAULT);
	}
}

class ActionForceFeed: ActionForceConsume
{
	void ActionForceFeed()
	{
		m_CallbackClass = ActionForceFeedCB;
		m_CommandUID 	= DayZPlayerConstants.CMD_ACTIONFB_FORCEFEED;
		m_FullBody 		= true;
		m_StanceMask 	= DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;		
		m_Text 			= "#feed";
	}
	
	override void CreateConditionComponents() 
	{
		m_ConditionTarget 	= new CCTMan(UAMaxDistances.DEFAULT);
		m_ConditionItem		= new CCINonRuined();
	}
}


//-----------------SMALL BITES VARIANT-------------------

class ActionForceFeedSmallCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousQuantityEdible(UAQuantityConsumed.EAT_NORMAL,UATimeSpent.DEFAULT);
	}
};

class ActionForceFeedSmall: ActionForceConsume
{
	void ActionForceFeed()
	{
		m_CallbackClass = ActionForceFeedSmallCB;
		m_CommandUID 	= DayZPlayerConstants.CMD_ACTIONFB_FORCEFEED;
		m_FullBody		= true;
		m_StanceMask 	= DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
	}
	
	override void CreateConditionComponents() 
	{
		m_ConditionTarget	= new CCTMan(UAMaxDistances.DEFAULT);
		m_ConditionItem		= new CCINonRuined();
	}
		
	override string GetText()
	{
		return "#feed";
	}
}
