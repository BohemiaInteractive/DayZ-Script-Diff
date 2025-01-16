class ActionForceConsumeSingleCB : ActionSingleUseBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CASingleUseQuantityEdible(UAQuantityConsumed.DEFAULT);
	}
}

class ActionForceConsumeSingle : ActionSingleUseBase
{
	const int DEFAULT_CONSUMED_QUANTITY = 1;
	
	void ActionForceConsumeSingle()
	{
		m_CallbackClass = ActionForceConsumeSingleCB;
		m_CommandUID 	= DayZPlayerConstants.CMD_ACTIONFB_INJECTIONTARGET;
		m_StanceMask 	= DayZPlayerConstants.STANCEIDX_ERECT | DayZPlayerConstants.STANCEIDX_CROUCH;
		m_FullBody 		= true;
		m_Text			= "#feed";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem 	= new CCINonRuined();
		m_ConditionTarget	= new CCTMan(UAMaxDistances.DEFAULT);
	}
		
	int GetConsumedQuantity()
	{
		return DEFAULT_CONSUMED_QUANTITY;
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!super.ActionCondition(player, target, item))
			return false;
		
		if (item.GetQuantity() <= item.GetQuantityMin())
			return false;
		
		PlayerBase targetPlayer = PlayerBase.Cast(target.GetObject());
		ConsumeConditionData dta = new ConsumeConditionData(targetPlayer,item);
		return targetPlayer && targetPlayer.CanEatAndDrink() && targetPlayer.CanConsumeFood(dta) && item.CanBeConsumed(dta);
	}
	
	override void OnEndServer(ActionData action_data)
	{
		super.OnEndServer(action_data);
		
		if (action_data.m_MainItem && action_data.m_MainItem.GetQuantity() <= 0) 
			action_data.m_MainItem.SetQuantity(0);
	}
	
	override void OnExecuteServer(ActionData action_data)
	{
		PlayerBase target = PlayerBase.Cast(action_data.m_Target.GetObject());
		
		if (target && action_data.m_MainItem)
		{
			PlayerConsumeData consumeData = new PlayerConsumeData();
			consumeData.m_Type = EConsumeType.ITEM_SINGLE_TIME;
			consumeData.m_Amount = GetConsumedQuantity();
			consumeData.m_Source = action_data.m_MainItem;
			consumeData.m_Agents = action_data.m_Player.GetBloodyHandsPenaltyAgents();
			
			target.Consume(consumeData);
		}
	}
}
