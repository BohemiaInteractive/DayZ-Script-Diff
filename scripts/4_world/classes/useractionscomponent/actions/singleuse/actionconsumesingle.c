class ActionConsumeSingle : ActionSingleUseBase
{
	const int DEFAULT_CONSUMED_QUANTITY = 1;
	
	void ActionConsumeSingle()
	{
		m_CommandUID 		= DayZPlayerConstants.CMD_ACTIONMOD_LICKBATTERY;
		m_CommandUIDProne 	= DayZPlayerConstants.CMD_ACTIONFB_LICKBATTERY;

		m_Text = "#eat";
	}
	
	int GetConsumedQuantity()
	{
		return DEFAULT_CONSUMED_QUANTITY;
	}
	
	override bool HasProneException()
	{
		return true;
	}

	override bool HasTarget()
	{
		return false;
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionItem 	= new CCINotRuinedAndEmpty();
		m_ConditionTarget 	= new CCTSelf();
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{	
		ConsumeConditionData dta = new ConsumeConditionData(player,item);
		return super.ActionCondition(player, target, item) && player.CanEatAndDrink() && player.CanConsumeFood(dta) && item.CanBeConsumed(dta);
	}
	
	override void OnExecuteServer(ActionData action_data)
	{
		PlayerBase player = action_data.m_Player;
		
		if (player && action_data.m_MainItem)
		{
			PlayerConsumeData consumeData = new PlayerConsumeData();
			consumeData.m_Type	= EConsumeType.ITEM_SINGLE_TIME;
			consumeData.m_Amount = GetConsumedQuantity();
			consumeData.m_Source = action_data.m_MainItem;
			consumeData.m_Agents = action_data.m_Player.GetBloodyHandsPenaltyAgents();

			player.Consume(consumeData);
		}
	}
	
	override void OnEndServer(ActionData action_data)
	{
		super.OnEndServer(action_data);
		
		if (action_data.m_MainItem && (action_data.m_MainItem.GetQuantity() <= 0))
			action_data.m_MainItem.SetQuantity(0);
	}
}

//! DEPRECATED
class ActionConsumeSingleCB : ActionSingleUseBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CASingleUseQuantityEdible(UAQuantityConsumed.DEFAULT);
	}
}
