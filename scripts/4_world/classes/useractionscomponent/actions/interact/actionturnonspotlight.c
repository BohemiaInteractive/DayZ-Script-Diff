class ActionTurnOnSpotlight: ActionInteractBase
{	
	void ActionTurnOnSpotlight()
	{
		m_Text = "#switch_on";
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		Object targetObject = target.GetObject();
		ItemBase target_IB = ItemBase.Cast( targetObject );
		
		Spotlight s = Spotlight.Cast(target_IB);
		
		if (s  &&  s.IsFolded())
				return false;
		
		int component_id = target.GetComponentIndex();
		string selection = targetObject.GetActionComponentName(component_id);

		if ( selection == Spotlight.SEL_REFLECTOR_COMP_U  &&  target_IB.HasEnergyManager()  &&  target_IB.GetCompEM().CanSwitchOn() )
		{
			return true;
		}
		
		return false;
	}

	override void OnExecuteServer( ActionData action_data )
	{
		Object targetObject = action_data.m_Target.GetObject();
		if (targetObject)
		{
			EntityAI target_EAI = EntityAI.Cast( targetObject );
			target_EAI.GetCompEM().SwitchOn();
		}
	}
};