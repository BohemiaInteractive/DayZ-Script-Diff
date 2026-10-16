class ActionRepairMotorbikeEngine : ActionRepairVehiclePartBase
{
	int m_LastValidComponentIndex = -1;
	
	void ActionRepairMotorbikeEngine()
	{
		m_CallbackClass 	= ActionRepairVehiclePartCB;
		m_CommandUID 		= DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
		m_StanceMask 		= DayZPlayerConstants.STANCEMASK_ERECT;
		m_FullBody 			= true;
		m_LockTargetOnUse 	= false;
		m_Text 				= "#repair";
	}
	
	override bool CanBeUsedWithBrokenLegs()
	{
		return false;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		MotorbikeScript motorbike = MotorbikeScript.Cast(target.GetObject());
		if (!motorbike || motorbike.EngineIsOn())
			return false;
	
		if (g_Game.IsDedicatedServer())
			return true;
			
		if (m_LastValidComponentIndex != target.GetComponentIndex() || m_CurrentDamageZone == "Engine")
		{
			array<string> selections = new array<string>();
			motorbike.GetActionComponentNameList(target.GetComponentIndex(), selections, LOD.NAME_VIEW);

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

	override void AdjustVehicleHealthServer(ActionData action_data)
	{
		super.AdjustVehicleHealthServer(action_data);
		
		MiscGameplayFunctions.DealAbsoluteDmg(action_data.m_MainItem, UADamageApplied.BUILD);
	}
}
