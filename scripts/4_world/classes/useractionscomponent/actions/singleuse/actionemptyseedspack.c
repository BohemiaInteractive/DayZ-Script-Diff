/*
	User action for emptying packs of seeds (horticulture)
*/


class ActionEmptySeedsPack: ActionSingleUseBase
{
	void ActionEmptySeedsPack()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_EMPTYSEEDSPACK;
		m_FullBody = false;
		m_Text = "#unbox";
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTNone;
	}

	override bool HasTarget()
	{
		return false;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		return true;
	}

	override void OnExecuteServer( ActionData action_data )
	{
		SeedPackBase item_SPB = SeedPackBase.Cast( action_data.m_MainItem );
		
		// The following careful script fixes an issue with VME with unknown repro.
		
		if (item_SPB)
		{
			PlayerBase player;
			
			if (action_data)
				player = action_data.m_Player;
			
			item_SPB.EmptySeedPack( player );
		}
	}
	
	override void OnEndServer( ActionData action_data )
	{
		//spawns wrapping Paper
		ItemBase paper = ItemBase.Cast( GetGame().CreateObjectEx("Paper", action_data.m_Player.GetPosition(), ECE_PLACE_ON_SURFACE) );
	}
};