class ActionMountBarbedWireCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.DEFAULT_CONSTRUCT);
	}
};

class ActionMountBarbedWire: ActionContinuousBase
{
	float m_DamageAmount;
	string m_SlotName;
	
	void ActionMountBarbedWire()
	{
		m_CallbackClass = ActionMountBarbedWireCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT;
		
		m_DamageAmount = 2;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_HIGH;
		m_Text = "#mount";
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTNonRuined( UAMaxDistances.BASEBUILDING );
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		Object targetObject = target.GetObject();
		
		if ( targetObject && targetObject.CanUseConstruction() && targetObject.CanUseConstructionBuild() )
		{
			BaseBuildingBase base_building = BaseBuildingBase.Cast( targetObject );
			
			string selection = targetObject.GetActionComponentName( target.GetComponentIndex() );
			
			if ( selection.Length() > 0 )
			{
				BarbedWire barbed_wire = BarbedWire.Cast( base_building.FindAttachmentBySlotName( selection ) );
				if ( barbed_wire && !barbed_wire.IsMounted() && !barbed_wire.IsRuined() )
				{
					m_SlotName = selection;
					
					return true;
				}
			}			
		}
		
		return false;
	}
		
	override void OnFinishProgressServer( ActionData action_data )
	{	
		BaseBuildingBase base_building = BaseBuildingBase.Cast( action_data.m_Target.GetObject() );
		BarbedWire barbed_wire = BarbedWire.Cast( base_building.FindAttachmentBySlotName( m_SlotName ) );
		
		//mount and refresh parent
		barbed_wire.SetMountedState( true );
		
		//solution for DamageSystem's case sensitivity
		string zone = "invalid";
		array<string> zones = new array<string>;
		zones.Copy(base_building.GetEntityDamageZoneMap().GetKeyArray());
		
		string tmp = "";
		string test = "";
		for (int i = 0; i < zones.Count(); i++)
		{
			tmp = zones.Get(i);
			test = tmp;
			test.ToLower();
			
			if (test == m_SlotName)
			{
				zone = tmp;
				break;
			}
		}
		
		base_building.SetHealth01(zone,"Health",barbed_wire.GetHealth01("","Health")); //attachment slot names and damagezone names must match
		
		action_data.m_Player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );
	}
	
	override string GetAdminLogMessage( ActionData action_data )
	{
		string message = string.Format("Player %1 Mounted BarbedWire on %2", action_data.m_Player, action_data.m_Target.GetObject() );
		return message;
	}
}