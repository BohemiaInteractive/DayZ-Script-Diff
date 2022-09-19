class ActionConsumeSingleCB : ActionSingleUseBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CASingleUseQuantityEdible(UAQuantityConsumed.DEFAULT);
	}
};

class ActionConsumeSingle: ActionSingleUseBase
{
	const int DEFAULT_CONSUMED_QUANTITY = 1;
	
	void ActionConsumeSingle()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_LICKBATTERY;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_LICKBATTERY;
		m_Text = "#eat";
	}
	
	int GetConsumedQuantity()
	{
		return DEFAULT_CONSUMED_QUANTITY;
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINotRuinedAndEmpty;
		m_ConditionTarget = new CCTSelf;
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		return super.ActionCondition(player, target, item) && player.CanEatAndDrink();
	}
	
	override bool HasProneException()
	{
		return true;
	}

	override bool HasTarget()
	{
		return false;
	}
	
	override void OnExecuteServer( ActionData action_data )
	{
		PlayerBase player = action_data.m_Player;
		
		if ( player && action_data.m_MainItem )
		{
			player.Consume(action_data.m_MainItem, GetConsumedQuantity(), EConsumeType.ITEM_SINGLE_TIME);
		}
	}
	
	override void OnEndServer( ActionData action_data )
	{
		if ( action_data.m_MainItem && ( action_data.m_MainItem.GetQuantity() <= 0 ) )
		{
			action_data.m_MainItem.SetQuantity( 0 );
		}
		
		if ( action_data.m_Player.HasBloodyHandsEx() == eBloodyHandsTypes.SALMONELA && !action_data.m_Player.GetInventory().FindAttachment( InventorySlots.GLOVES ) && GetProgress(action_data) > 0 )
		{
			action_data.m_Player.SetBloodyHandsPenalty();
		}
	}
};