class ActionForceABiteCB : ActionSingleUseBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CASingleUseQuantityEdible(UAQuantityConsumed.EAT_NORMAL);
	}
};

class ActionForceABite: ActionForceConsumeSingle
{
	void ActionForceABite()
	{		
		m_CallbackClass = ActionForceABiteCB;
		m_Text = "#feed_a_bite";
	}
	
	override void CreateConditionComponents()  
	{		
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTMan(UAMaxDistances.DEFAULT);
	}
	
	override void OnEndServer( ActionData action_data )
	{	
		if ( action_data.m_MainItem.GetQuantity() <= 0 )
		{
			action_data.m_MainItem.SetQuantity(0);
		}
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