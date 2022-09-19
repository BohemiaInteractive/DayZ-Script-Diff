class ActionUnplugThisByCord: ActionInteractBase
{
	void ActionUnplugThisByCord()
	{
		m_Text = "#unplug_by_cord";
	}

	override bool HasProgress()
	{
		return false;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		Object targetObject = target.GetObject();
		ItemBase target_IB = ItemBase.Cast( targetObject );
		
		if ( target_IB  &&  target_IB.HasEnergyManager() )
		{
			string selection = targetObject.GetActionComponentName(target.GetComponentIndex());			
			if ( selection == ComponentEnergyManager.SEL_CORD_PLUGGED )
			{
				return true;
			}
			
			// Special case for unfolded spotlight
			if ( selection == Spotlight.SEL_CORD_PLUGGED_U )
			{
				return true;
			}
		}
		
		return false;
	}

	override void OnExecuteServer( ActionData action_data )
	{	
		ItemBase target_IB = ItemBase.Cast( action_data.m_Target.GetObject() ); // cast to ItemBase
		target_IB.GetCompEM().UnplugThis();
		
		if ( target_IB.IsInherited(Spotlight) )
		{
			target_IB.HideSelection ( Spotlight.SEL_CORD_PLUGGED_U );
			target_IB.ShowSelection ( Spotlight.SEL_CORD_FOLDED_U );
		}
	}
};