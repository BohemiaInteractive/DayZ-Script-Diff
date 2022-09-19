class ActionForceDrinkCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousQuantityEdible(UAQuantityConsumed.DRINK,UATimeSpent.DEFAULT);
	}
};

class ActionForceDrink: ActionForceConsume
{
	void ActionForceDrink()
	{
		m_CallbackClass = ActionForceDrinkCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_EMPTY_VESSEL;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "#give_drink";
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionTarget = new CCTMan(UAMaxDistances.DEFAULT);
		m_ConditionItem = new CCINonRuined;
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if (!super.ActionCondition( player, target, item))
			return false;
		
		PlayerBase target_player = PlayerBase.Cast(target.GetObject());
		
		if (target_player)
			return target_player.CanEatAndDrink();
		else
			return false;
	}
	
};
	
	