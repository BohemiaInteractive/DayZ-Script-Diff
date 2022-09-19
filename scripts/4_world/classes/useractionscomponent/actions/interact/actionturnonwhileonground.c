class ActionTurnOnWhileOnGround: ActionInteractBase
{
	void ActionTurnOnWhileOnGround()
	{
		m_CommandUID        = DayZPlayerConstants.CMD_ACTIONMOD_OPENDOORFW;
		m_StanceMask        = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_Text = "#switch_on";
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		EntityAI target_entity = EntityAI.Cast( target.GetObject() );
		
		InventoryLocation loc = new InventoryLocation;
		target_entity.GetInventory().GetCurrentInventoryLocation(loc);
		
		return ( player.IsAlive()  &&  target_entity.HasEnergyManager()  &&  target_entity.GetCompEM().CanSwitchOn() && target_entity.GetCompEM().CanWork() && loc.GetType() == InventoryLocationType.GROUND );
	}

	override void OnExecuteServer( ActionData action_data )
	{
		EntityAI target_entity = EntityAI.Cast( action_data.m_Target.GetObject() );
		
		if ( target_entity.GetCompEM().CanWork() )
		{
			target_entity.GetCompEM().SwitchOn();
			
			InformPlayers( action_data.m_Player, action_data.m_Target, UA_FINISHED );	//Success
		}
		else
		{
			InformPlayers( action_data.m_Player, action_data.m_Target, UA_FAILED );		//No power
		}
	}
};