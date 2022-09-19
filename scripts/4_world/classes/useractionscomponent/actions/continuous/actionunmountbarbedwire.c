class ActionUnmountBarbedWireCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.DEFAULT_CONSTRUCT);
	}
};

class ActionUnmountBarbedWire: ActionContinuousBase
{
	float m_DamageAmount;
	string m_SlotName;
	
	void ActionUnmountBarbedWire()
	{
		m_CallbackClass = ActionUnmountBarbedWireCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT;
		
		m_DamageAmount = 2;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_HIGH;
		m_Text = "#unmount_barbed_wire";
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTNonRuined( UAMaxDistances.BASEBUILDING );
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		Object targetObject = target.GetObject();
		
		if ( targetObject && targetObject.CanUseConstruction() )
		{
			BaseBuildingBase base_building = BaseBuildingBase.Cast( targetObject );
			if (!base_building.IsPlayerInside(player,""))
				return false;
			
			string selection = targetObject.GetActionComponentName( target.GetComponentIndex() );
			
			if ( selection.Length() > 0 )
			{
				int delimiter_index = selection.IndexOfFrom( 0, "_mounted" );
				if ( delimiter_index > -1 )
				{
					selection = selection.Substring( 0, delimiter_index );
					
					BarbedWire barbed_wire = BarbedWire.Cast( base_building.FindAttachmentBySlotName( selection ) );
					if ( barbed_wire && barbed_wire.IsMounted() )
					{
						m_SlotName = selection;
						
						return true;
					}					
				}
			}			
		}
		
		return false;
	}
		
	override void OnFinishProgressServer( ActionData action_data )
	{	
		BaseBuildingBase base_building = BaseBuildingBase.Cast( action_data.m_Target.GetObject() );
		BarbedWire barbed_wire = BarbedWire.Cast( base_building.FindAttachmentBySlotName( m_SlotName ) );
		
		//unmount and refresh parent
		barbed_wire.SetMountedState( false );
		
		action_data.m_Player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );
	}
	
	override string GetAdminLogMessage( ActionData action_data )
	{
		string message = string.Format("Player %1 Unmounted BarbedWire from %2", action_data.m_Player, action_data.m_Target.GetObject() );
		return message;
	}
}