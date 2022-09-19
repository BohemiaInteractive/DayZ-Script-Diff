class ActionWorldCraftCancel: ActionSingleUseBase
{
	void ActionWorldCraftCancel()
	{
		m_Text = "#cancel";
	}
	
	override void CreateConditionComponents()  
	{		
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTNone;
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		return player.GetCraftingManager().IsInventoryCraft();
	}
	
	override void Start( ActionData action_data ) //Setup on start of action
	{
		super.Start( action_data );
	}
	
	override bool IsLocal()
	{
		return true;
	}
};