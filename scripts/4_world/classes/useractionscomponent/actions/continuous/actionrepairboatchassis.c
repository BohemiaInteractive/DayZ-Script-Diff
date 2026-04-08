class ActionRepairBoatChassis : ActionRepairVehiclePartBase
{
	void ActionRepairBoatChassis()
	{
		m_CallbackClass 			= ActionRepairVehiclePartCB;	
		m_CommandUID 				= DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
		m_StanceMask				= DayZPlayerConstants.STANCEMASK_ERECT;
		m_FullBody 					= true;
		m_LockTargetOnUse			= false;
		m_Text 						= "#repair";
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (player.GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS)
			return false;
		
		BoatScript boat = BoatScript.Cast(target.GetObject());
		if (!boat)
			return false;
		
		if (g_Game.IsDedicatedServer())
			return true;
					
		string damageZone = "";
		array<string> selections = new array<string>();
		boat.GetActionComponentNameList(target.GetComponentIndex(), selections);
		
		foreach (string selection : selections)
		{
			if (boat && DamageSystem.GetDamageZoneFromComponentName(boat, selection, damageZone))
			{
				if (damageZone == "Engine")
					continue;

				int zoneHP = boat.GetHealthLevel(damageZone);
				if (zoneHP > GameConstants.STATE_WORN && zoneHP < GameConstants.STATE_RUINED)
				{
					m_CurrentDamageZone = damageZone;
					return true;
				}
			}
		}
		
		return false;
	}
}