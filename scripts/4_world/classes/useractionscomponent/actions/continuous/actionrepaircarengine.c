class RepairCarEngineActionReciveData : ActionReciveData
{
	string m_DamageZoneRecived;
}

class RepairCarEngineActionData : ActionData
{
	string m_DamageZone;
}

class ActionRepairCarEngineCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.BASEBUILDING_REPAIR_FAST);
	}
}

class ActionRepairCarEngine : ActionContinuousBase
{
	typename m_LastValidType;
	string m_CurrentDamageZone = "";
	int m_LastValidComponentIndex = -1;
	
	void ActionRepairCarEngine()
	{
		m_CallbackClass 	= ActionRepairTentPartCB;
		m_SpecialtyWeight 	= UASoftSkillsWeight.PRECISE_LOW;
		m_CommandUID 		= DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
		m_StanceMask 		= DayZPlayerConstants.STANCEMASK_ERECT;
		m_FullBody 			= true;
		m_LockTargetOnUse 	= false;
		m_Text 				= "#repair";
	}

	override void CreateConditionComponents()  
	{
		m_ConditionItem 	= new CCINonRuined();
		m_ConditionTarget 	= new CCTCursor(UAMaxDistances.REPAIR);
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (player.GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS)
			return false;
		
		Object targetObject = target.GetObject();
		Object targetParent = target.GetParent();
		CarScript car 		= CarScript.Cast(targetObject);
		EntityAI carEntity 	= EntityAI.Cast(targetObject);
		
		if (!car || !player)
			return false;
		
		if (car.EngineIsOn())
			return false;

		if (GetGame().IsMultiplayer() && GetGame().IsServer())
			return true;
			
		if (m_LastValidType != target.Type() || m_LastValidComponentIndex != target.GetComponentIndex() || m_CurrentDamageZone == "")
		{
			string damageZone = "";
			array<string> selections = new array<string>();
			targetObject.GetActionComponentNameList(target.GetComponentIndex(), selections, LOD.NAME_VIEW);

			foreach (string selection : selections)
			{
				//NOTE: relevant fire geometry and view geometry selection names MUST match in order to get a valid damage zone
				if (carEntity && DamageSystem.GetDamageZoneFromComponentName(carEntity, selection, damageZone))
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

	override void OnFinishProgressServer(ActionData action_data)
	{
		Object tgObject = action_data.m_Target.GetObject();
		
		string damageZone = RepairCarPartActionData.Cast(action_data).m_DamageZone;
		if (!GetGame().IsMultiplayer())
			damageZone = m_CurrentDamageZone;

		if (tgObject && damageZone != "")
		{
			CarScript car = CarScript.Cast(tgObject);
			if (car)
			{
				int newDmgLevel = Math.Clamp(car.GetHealthLevel(damageZone) - 1, GameConstants.STATE_WORN, GameConstants.STATE_RUINED);
				float zoneMax = car.GetMaxHealth(damageZone, "");
				float randomValue = Math.RandomFloatInclusive(zoneMax * 0.05, zoneMax * 0.15);
				
				switch (newDmgLevel)
				{
					case GameConstants.STATE_BADLY_DAMAGED:
						car.SetHealth(damageZone, "", (zoneMax * GameConstants.DAMAGE_RUINED_VALUE) + randomValue);
						break;

					case GameConstants.STATE_DAMAGED:
						car.SetHealth(damageZone, "", (zoneMax * GameConstants.DAMAGE_BADLY_DAMAGED_VALUE) + randomValue);
						break;

					case GameConstants.STATE_WORN:
						car.SetHealth(damageZone, "", (zoneMax * GameConstants.DAMAGE_DAMAGED_VALUE) + randomValue);
						break;
					
					default:
						break;
				}

				MiscGameplayFunctions.DealAbsoluteDmg(action_data.m_MainItem, UADamageApplied.BUILD);
			}
		}
	}

	override ActionData CreateActionData()
	{
		RepairCarPartActionData actionData = new RepairCarPartActionData();
		return actionData;
	}
	
	override void WriteToContext(ParamsWriteContext ctx, ActionData action_data)
	{
		super.WriteToContext(ctx, action_data);
		RepairCarPartActionData repairActionData;

		if (HasTarget() && Class.CastTo(repairActionData, action_data))
		{
			repairActionData.m_DamageZone = m_CurrentDamageZone;
			ctx.Write(repairActionData.m_DamageZone);
		}
	}
	
	override bool ReadFromContext(ParamsReadContext ctx, out ActionReciveData action_recive_data)
	{
		if (!action_recive_data)
			action_recive_data = new RepairCarPartActionReciveData();

		super.ReadFromContext(ctx, action_recive_data);
		RepairCarPartActionReciveData reciveDataRepair = RepairCarPartActionReciveData.Cast(action_recive_data);

		if (HasTarget())
		{
			string zone;
			if (!ctx.Read(zone))
				return false;

			reciveDataRepair.m_DamageZoneRecived = zone;
		}

		return true;
	}
	
	override void HandleReciveData(ActionReciveData action_recive_data, ActionData action_data)
	{
		super.HandleReciveData(action_recive_data, action_data);

		RepairCarPartActionReciveData reciveDataRepair = RepairCarPartActionReciveData.Cast(action_recive_data);
		RepairCarPartActionData.Cast(action_data).m_DamageZone = reciveDataRepair.m_DamageZoneRecived;
	}
}
