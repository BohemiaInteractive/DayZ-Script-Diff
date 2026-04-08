class ActionRepairCarEngineCB : ActionRepairVehiclePartCB
{}

class ActionRepairCarEngine : ActionRepairVehiclePartBase
{
	typename m_LastValidType; // deprecated
	int m_LastValidComponentIndex = -1;
	
	void ActionRepairCarEngine()
	{
		m_CallbackClass 	= ActionRepairCarEngineCB;
		m_SpecialtyWeight 	= UASoftSkillsWeight.PRECISE_LOW;
		m_CommandUID 		= DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
		m_StanceMask 		= DayZPlayerConstants.STANCEMASK_ERECT;
		m_FullBody 			= true;
		m_LockTargetOnUse 	= false;
		m_Text 				= "#repair";
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (player.GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS)
			return false;
		
		CarScript car = CarScript.Cast(target.GetObject());
		if (!car || car.EngineIsOn())
			return false;
	
		if (g_Game.IsDedicatedServer())
			return true;
			
		if (m_LastValidType != target.Type() || m_LastValidComponentIndex != target.GetComponentIndex() || m_CurrentDamageZone == "")
		{
			string damageZone = "";
			array<string> selections = new array<string>();
			car.GetActionComponentNameList(target.GetComponentIndex(), selections, LOD.NAME_VIEW);

			foreach (string selection : selections)
			{
				if (car && DamageSystem.GetDamageZoneFromComponentName(car, selection, damageZone))
				{
					if (damageZone == "Engine")
					{
						int zoneHP = car.GetHealthLevel(damageZone);
						if (zoneHP > GameConstants.STATE_WORN && zoneHP < GameConstants.STATE_RUINED)
						{
							m_CurrentDamageZone = damageZone;
							m_LastValidComponentIndex = target.GetComponentIndex();
								
							return true;
						}
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

// deprecated
class RepairCarEngineActionReciveData : RepairVehiclePartActionReciveData
{}

class RepairCarEngineActionData : RepairVehiclePartActionData
{}
