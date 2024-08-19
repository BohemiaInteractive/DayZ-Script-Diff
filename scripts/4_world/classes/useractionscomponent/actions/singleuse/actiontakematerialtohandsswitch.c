class ActionTakeMaterialToHandsSwitch: ActionSingleUseBase
{
	void ActionTakeMaterialToHandsSwitch()
	{
		m_Text = "#switch_to_next_material";
	}
	
	override void CreateConditionComponents()  
	{		
		m_ConditionItem = new CCINotPresent;
		m_ConditionTarget = new CCTNonRuined( UAMaxDistances.BASEBUILDING );
	}
		
	override bool IsInstant()
	{
		return true;
	}
	
	override bool RemoveForceTargetAfterUse()
	{
		return false;
	}
	
	override bool UseAcknowledgment()
	{
		return true;
	}

	override typename GetInputType()
	{
		return InteractActionInput;
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		EntityAI target_entity = EntityAI.Cast( target.GetObject() );
		if ( target_entity && target_entity.CanUseConstruction() )
		{
			BaseBuildingBase base_building = BaseBuildingBase.Cast( target_entity );
			if(!base_building.IsPlayerInside(player,""))
				return false;
			
			ConstructionActionData construction_action_data = player.GetConstructionActionData();
			
			string main_part_name = target_entity.GetActionComponentName( target.GetComponentIndex() );
			construction_action_data.RefreshAttachmentsToDetach( target_entity, main_part_name );
			
			if ( construction_action_data.GetAttachmentsToDetachCount() > 1 )
			{
				return true;
			}
		}
		
		return false;
	}
	
	override void Start( ActionData action_data )
	{
		super.Start( action_data );
		
		//set next index
		ConstructionActionData construction_action_data = action_data.m_Player.GetConstructionActionData();
		construction_action_data.SetNextAttachmentIndex();
	}	
}