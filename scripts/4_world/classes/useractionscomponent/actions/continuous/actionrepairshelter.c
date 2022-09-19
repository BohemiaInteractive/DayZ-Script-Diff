//used for shelters (non-proxy tent objects)
class ActionRepairShelter: ActionRepairTent
{
	void ActionRepairShelter()
	{
		m_CallbackClass = ActionRepairTentCB;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
		
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
	}

	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTCursor(UAMaxDistances.SMALL);
	}
	
	override bool IsUsingProxies()
	{
		return false;
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		ShelterBase shelter = ShelterBase.Cast( target.GetObject() );
		if ( !shelter )
			return false;

		if ( player && shelter )
		{
			PluginRepairing module_repairing;
			Class.CastTo(module_repairing, GetPlugin(PluginRepairing));
			if (module_repairing.CanRepair(item,shelter))
			{
				return true;
			}
		}
		
		return false;
	}

	override void OnFinishProgressServer( ActionData action_data )
	{
		ShelterBase shelter = ShelterBase.Cast( action_data.m_Target.GetObject() );
		
		if ( shelter ) 
		{
			PluginRepairing module_repairing;
			Class.CastTo(module_repairing, GetPlugin(PluginRepairing));
			module_repairing.Repair(action_data.m_Player,action_data.m_MainItem,shelter,m_SpecialtyWeight);
		}
	}
};