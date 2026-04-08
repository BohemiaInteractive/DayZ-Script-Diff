class ActionRepairCarChassisCB : ActionRepairVehiclePartCB
{};

class ActionRepairCarChassis : ActionRepairVehiclePartBase
{
	protected typename m_LastValidType; // deprecated
	protected int m_LastValidComponentIndex;
	
	void ActionRepairCarChassis()
	{
		m_CallbackClass 			= ActionRepairCarChassisCB;
		m_SpecialtyWeight 			= UASoftSkillsWeight.PRECISE_LOW;		
		m_CommandUID 				= DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
		m_StanceMask				= DayZPlayerConstants.STANCEMASK_ERECT;
		m_FullBody 					= true;
		m_LockTargetOnUse			= false;
		m_Text 						= "#repair";
		
		m_LastValidComponentIndex 	= -1;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (player.GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS)
			return false;
		
		CarScript car = CarScript.Cast(target.GetObject());
		if (!car)
		{
			return false;
		}
		
		if (g_Game.IsMultiplayer() && g_Game.IsServer())
		{
			return true;
		}
		
		if (m_LastValidType != target.Type() || m_LastValidComponentIndex != target.GetComponentIndex() || m_CurrentDamageZone == "")
		{
			string damageZone = "";
			array<string> selections = new array<string>();
			car.GetActionComponentNameList(target.GetComponentIndex(), selections);
			
			foreach (string selection : selections)
			{
				//NOTE: relevant fire geometry and view geometry selection names MUST match in order to get a valid damage zone
				if (car && DamageSystem.GetDamageZoneFromComponentName(car, selection, damageZone))
				{
					if (damageZone == "Engine")
						continue;

					int zoneHP = car.GetHealthLevel(damageZone);
					if (zoneHP > GameConstants.STATE_WORN && zoneHP < GameConstants.STATE_RUINED)
					{
						m_CurrentDamageZone = damageZone;
						m_LastValidComponentIndex = target.GetComponentIndex();
					
						//Determine if using a "Special" item for repairing
						WoodenPlank plank = WoodenPlank.Cast(item);
						Fabric tarp = Fabric.Cast(item);
						
						//Prevent planks and tarp from repairing non related areas
						if ((tarp || plank) && (damageZone != "BackWood" && damageZone != "BackTarp"))
							return false;

						return true;
					}
				}
			}
		}
		
		return false;
	}
	
	override void AdjustItemQuantityServer(ActionData action_data)
	{
		if (action_data.m_MainItem.HasQuantity())
		{
			if (action_data.m_MainItem.GetQuantity() > 1)
			{
				int qnt = action_data.m_MainItem.GetQuantity();
				Fabric usedTarp = Fabric.Cast(action_data.m_MainItem);
				WoodenPlank usedPlank = WoodenPlank.Cast(action_data.m_MainItem);
				if (usedTarp || usedPlank)
				{
					qnt -= 1;
				}
				else
				{
					qnt -= action_data.m_MainItem.GetQuantityMax() * 0.25;
				}

				action_data.m_MainItem.SetQuantity(qnt);
			}
			else
			{
				action_data.m_MainItem.Delete();
			}
		}	
	}	
};

// deprecated
class RepairCarChassisActionReciveData : ActionReciveData
{}

class RepairCarChassisActionData : ActionData
{}