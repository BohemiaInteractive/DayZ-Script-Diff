class ActionEatMeatCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousQuantityEdible(UAQuantityConsumed.EAT_NORMAL,UATimeSpent.DEFAULT);
	}
};

class ActionEatMeat: ActionEatBig
{
	void ActionEatMeat()
	{
		m_CallbackClass = ActionEatMeatCB;
	}
			
	override void ApplyModifiers( ActionData action_data )
	{
		Edible_Base food_item = Edible_Base.Cast( action_data.m_MainItem );
		if ( food_item )
		{			
			if ( food_item.IsMeat() && food_item.IsFoodRaw() )
			{
				PluginLifespan module_lifespan = PluginLifespan.Cast( GetPlugin( PluginLifespan ) );
				if( module_lifespan )
				{
					module_lifespan.UpdateBloodyHandsVisibility( action_data.m_Player, true );
				}
			}
		}
	}
}
