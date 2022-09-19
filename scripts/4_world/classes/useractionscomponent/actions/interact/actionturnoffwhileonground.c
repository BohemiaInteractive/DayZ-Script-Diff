class ActionTurnOffWhileOnGround: ActionInteractBase
{	
	void ActionTurnOffWhileOnGround()
	{
		m_CommandUID        = DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;
		m_StanceMask        = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_Text = "#switch_off";
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		EntityAI target_entity = EntityAI.Cast( target.GetObject() );
		
		InventoryLocation loc = new InventoryLocation;
		target_entity.GetInventory().GetCurrentInventoryLocation(loc);
		
		return ( player.IsAlive() && target_entity.HasEnergyManager() && target_entity.GetCompEM().CanSwitchOff()) && loc.GetType() == InventoryLocationType.GROUND );
	}

	override void OnExecuteServer( ActionData action_data )
	{
		EntityAI target_entity = EntityAI.Cast( action_data.m_Target.GetObject() );
		
		if ( target_entity.GetCompEM().CanSwitchOff() )
		{
			target_entity.GetCompEM().SwitchOff();
			
			InformPlayers( action_data.m_Player, action_data.m_Target, UA_FINISHED );	//Success
		}
		else
		{
			InformPlayers( action_data.m_Player, action_data.m_Target, UA_FAILED );		//Already turned off
		}	
	}
}