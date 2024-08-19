class ActionWorldCraftSwitch: ActionSingleUseBase
{
	void ActionWorldCraftSwitch()
	{
		m_Text = "#next_recipe";
	}
	
	override void CreateConditionComponents()  
	{		
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTObject(UAMaxDistances.DEFAULT);
	}
	
	override bool IsLocal()
	{
		return true;
	}
	
	override bool IsInstant()
	{
		return true;
	}
	
	override bool RemoveForceTargetAfterUse()
	{
		return false;
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item )
	{	
		if ( player.GetCraftingManager().GetRecipesCount() > 1 )
		{
			return true;
		}
		return false;
	}
	
	override void Start( ActionData action_data ) //Setup on start of action
	{
		super.Start( action_data );
		action_data.m_Player.GetCraftingManager().SetNextRecipe();
	}
	
};