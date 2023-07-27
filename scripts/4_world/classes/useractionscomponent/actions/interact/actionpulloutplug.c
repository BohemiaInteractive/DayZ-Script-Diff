class ActionPullOutPlug : ActionInteractBase
{
	void ActionPullOutPlug()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;

		m_Text = "#pull_out_plug";
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		EntityAI targetEntity = EntityAI.Cast(target.GetObject());
		
		if (targetEntity.HasEnergyManager())
		{
			string selection = targetEntity.GetActionComponentName(target.GetComponentIndex());
			
			
			if (GetGame().IsServer())
				return targetEntity.GetCompEM().GetPlugOwner(selection) != null;
			else
				return targetEntity.GetCompEM().IsSelectionAPlug(selection);
		}
		
		return false;
	}

	override void OnExecuteServer(ActionData action_data)
	{	
		Object targetObject = action_data.m_Target.GetObject();
		if (targetObject)
		{
			EntityAI targetEntity = EntityAI.Cast(targetObject);
			string selection = targetObject.GetActionComponentName(action_data.m_Target.GetComponentIndex());
			
			EntityAI device = EntityAI.Cast(targetEntity.GetCompEM().GetPlugOwner(selection));
			if (device)
				device.GetCompEM().UnplugThis();
			
			// Disable Advanced Placement
			if (action_data.m_Player.IsPlacingServer())
				action_data.m_Player.PlacingCancelServer();
		}
	}
}
