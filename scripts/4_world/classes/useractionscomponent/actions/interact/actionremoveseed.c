class ActionRemoveSeed: ActionInteractBase
{
	SeedBase m_Seed;
	
	void ActionRemoveSeed()
	{
		m_Text = "#take";
	}
	
	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTCursor(UAMaxDistances.SMALL);
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if ( player.GetCommand_Vehicle() )
			return false;
		
		GardenBase garden_base;
		if ( Class.CastTo( garden_base, target.GetObject() ) )
		{
			Slot slot;
			
			array<string> selections = new array<string>;
			garden_base.GetActionComponentNameList( target.GetComponentIndex(), selections );
			string selection;

			for (int s = 0; s < selections.Count(); s++)
			{
				selection = selections[s];
				slot = garden_base.GetSlotBySelection( selection );
				if ( slot )
					break;
			}
			
			//Can only remove seed if slot is not watered to prevent VME
			if ( slot && slot.GetSeed() )
			{
				if ( slot.GetWateredState() != 0 )
					return false;
				
				m_Seed = SeedBase.Cast( slot.GetSeed() );
				
				if ( m_Seed )
				{
					if ( player.GetInventory().CanAddEntityIntoInventory( m_Seed ) && m_Seed.GetHierarchyRootPlayer() != player )
						return true;
					else
						return player.GetInventory().CanAddEntityIntoHands( m_Seed );
				}
			}
		}
		return false;
	}

	override bool InventoryReservation(ActionData action_data)
	{
		bool success = true;
		
		InventoryLocation il = new InventoryLocation;
		
		if ( m_Seed )
		{
			action_data.m_Player.GetInventory().FindFreeLocationFor( m_Seed , FindInventoryLocationType.ANY, il );
			if ( action_data.m_Player.GetInventory().HasInventoryReservation( m_Seed, il ) )
			{
				success = false;
			}
			else
			{
				action_data.m_Player.GetInventory().AddInventoryReservationEx( m_Seed, il, GameInventory.c_InventoryReservationTimeoutMS );
			}
		}	
		
		if ( success )
		{
			if ( il )
				action_data.m_ReservedInventoryLocations.Insert( il );
		}
		
		return success;
	}
	
	override void OnExecute( ActionData action_data )
	{
		if ( GetGame().IsMultiplayer() && GetGame().IsServer() )
			return;
		
		PlayerBase player = action_data.m_Player;
		float stackable;
		InventoryLocation il;
		
		//We place in inventory
		if ( !player.GetInventory().CanAddEntityIntoHands( m_Seed ) )
		{
			il = action_data.m_ReservedInventoryLocations.Get( 0 );
			InventoryLocation targetInventoryLocation = new InventoryLocation;
			m_Seed.GetInventory().GetCurrentInventoryLocation( targetInventoryLocation );
			
			stackable = m_Seed.GetTargetQuantityMax( il.GetSlot() );
			
			if ( stackable == 0 || stackable >= m_Seed.GetQuantity() )
			{
				player.PredictiveTakeToDst( targetInventoryLocation, il );
			}
			else
			{
				m_Seed.SplitIntoStackMaxToInventoryLocationClient( il );
			}
		}
		else
		{
			//We place in hands
			ClearInventoryReservationEx( action_data );
			
			stackable = m_Seed.GetTargetQuantityMax( -1 );
			
			if ( stackable == 0 || stackable >= m_Seed.GetQuantity() )
			{
				action_data.m_Player.PredictiveTakeEntityToHands( m_Seed );
			}
			else
			{
				il = new InventoryLocation;
				il.SetHands( action_data.m_Player, m_Seed );
				m_Seed.SplitIntoStackMaxToInventoryLocationClient( il );
			}
		}
	}
};