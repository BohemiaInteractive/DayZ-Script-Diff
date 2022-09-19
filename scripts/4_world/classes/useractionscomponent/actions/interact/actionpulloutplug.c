class ActionPullOutPlug: ActionInteractBase
{
	void ActionPullOutPlug()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;
		m_Text = "#pull_out_plug";
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		Object targetObject = target.GetObject();
		EntityAI target_EAI = EntityAI.Cast( target.GetObject() );
		
		if ( target_EAI.HasEnergyManager() )
		{
			string selection = targetObject.GetActionComponentName(target.GetComponentIndex());
			
			
			if ( GetGame().IsServer() )
			{
				EntityAI device = target_EAI.GetCompEM().GetPlugOwner(selection);
				
				if ( device )
				{
					return true;
				}
			}
			else
			{
				return target_EAI.GetCompEM().IsSelectionAPlug(selection);
			}
		}
		
		return false;
	}

	override void OnExecuteServer( ActionData action_data )
	{	
		Object targetObject = action_data.m_Target.GetObject();
		if ( targetObject )
		{
			EntityAI target_EAI = EntityAI.Cast( targetObject );
			string selection = targetObject.GetActionComponentName(action_data.m_Target.GetComponentIndex());
			
			EntityAI device = EntityAI.Cast( target_EAI.GetCompEM().GetPlugOwner(selection) );
			if ( device )
			{
				device.GetCompEM().UnplugThis();
			}
			
			// Disable Advanced Placement
			if ( action_data.m_Player.IsPlacingServer() )
			{
				action_data.m_Player.PlacingCancelServer();
			}
		}
	}
};