class ActionPickupChicken: ActionInteractBase
{
	void ActionPickupChicken()
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
		m_ConditionTarget = new CCTNone;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if ( player.GetCommand_Vehicle() )
			return false;
		
		Animal_GallusGallusDomesticus chicken;
		if ( Class.CastTo( chicken, target.GetObject() ) )
		{
			return !chicken.IsAlive();
		}
		return false;
	}

	override void OnExecuteServer( ActionData action_data )
	{
		// We will switch the item
		Animal_GallusGallusDomesticus targetChicken = Animal_GallusGallusDomesticus.Cast( action_data.m_Target.GetObject() );
		if ( targetChicken )
		{
			DeadChicken_ColorBase deadChicken = DeadChicken_ColorBase.Cast( action_data.m_Player.GetHumanInventory().CreateInHands( targetChicken.GetDeadItemName() ) );
			
			if ( deadChicken == null )
			{
				deadChicken = DeadChicken_ColorBase.Cast( action_data.m_Player.GetHumanInventory().CreateInInventory( targetChicken.GetDeadItemName() ) );
			
				if ( deadChicken == null )
					deadChicken = DeadChicken_ColorBase.Cast( GetGame().CreateObjectEx( targetChicken.GetDeadItemName(), targetChicken.GetPosition(), ECE_PLACE_ON_SURFACE, RF_ORIGINAL ) );
			}
			targetChicken.Delete();
		}
	}
};