class ActionPlaceFireplaceIntoBarrel: ActionSingleUseBase
{
	void ActionPlaceFireplaceIntoBarrel()
	{
		m_CommandUID        = DayZPlayerConstants.CMD_ACTIONMOD_OPENDOORFW;
		m_StanceMask        = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_LOW;
		m_Text = "#place_object";
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTNonRuined( UAMaxDistances.DEFAULT );
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		Object target_object = target.GetObject();
		
		if ( target_object && target_object.IsInherited( BarrelHoles_ColorBase ) )
		{
			BarrelHoles_ColorBase fireplace_barrel = BarrelHoles_ColorBase.Cast( target_object );
			
			if ( fireplace_barrel.IsOpen() && fireplace_barrel.GetInventory().AttachmentCount() == 0 && fireplace_barrel.IsCargoEmpty() )
			{
				return true;
			}
		}
		
		return false;
	}
	
	override void OnExecuteServer( ActionData action_data )
	{	
		FireplaceBase fireplace_in_hands = FireplaceBase.Cast( action_data.m_MainItem );
		BarrelHoles_ColorBase fireplace_barrel = BarrelHoles_ColorBase.Cast( action_data.m_Target.GetObject() );
		
		auto lambda = new MoveEquipToExistingItemAndDestroyOldRootLambda(fireplace_in_hands, "", action_data.m_Player, fireplace_barrel);
		action_data.m_Player.ServerReplaceItemInHandsWithNewElsewhere(lambda);

		action_data.m_Player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );
	}
}
