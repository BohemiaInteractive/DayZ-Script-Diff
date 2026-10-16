class ActionRepairMotorbikeChassis : ActionRepairVehiclePartBase
{
	protected int m_LastValidComponentIndex;
	
	void ActionRepairMotorbikeChassis()
	{
		m_CallbackClass 			= ActionRepairVehiclePartCB;
		m_CommandUID 				= DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
		m_StanceMask				= DayZPlayerConstants.STANCEMASK_ERECT;
		m_FullBody 					= true;
		m_LockTargetOnUse			= false;
		m_Text 						= "#repair";
		
		m_LastValidComponentIndex 	= -1;
	}
	
	override bool CanBeUsedWithBrokenLegs()
	{
		return false;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{		
		Motorbike motorbike = Motorbike.Cast(target.GetObject());
		if (!motorbike)
			return false;
		
		if (g_Game.IsMultiplayer() && g_Game.IsServer())
			return true;
		
		if (m_LastValidComponentIndex != target.GetComponentIndex() || m_CurrentDamageZone != "Engine")
		{
			array<string> selections = new array<string>();
			motorbike.GetActionComponentNameList(target.GetComponentIndex(), selections);
			
			foreach (string selection : selections)
			{
				string damageZone = "";
				if (motorbike && DamageSystem.GetDamageZoneFromComponentName(motorbike, selection, damageZone))
				{
					int zoneHP = motorbike.GetHealthLevel(damageZone);
					if (zoneHP > GameConstants.STATE_WORN && zoneHP < GameConstants.STATE_RUINED)
					{
						m_CurrentDamageZone = damageZone;
						m_LastValidComponentIndex = target.GetComponentIndex();

						return true;
					}
				}
			}
		}
		
		return false;
	}
}
