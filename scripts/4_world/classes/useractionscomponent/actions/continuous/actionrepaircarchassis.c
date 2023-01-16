class RepairCarChassisActionReciveData : ActionReciveData
{
	string m_DamageZoneRecived;
}

class RepairCarChassisActionData : ActionData
{
	string m_DamageZone;
}

class ActionRepairCarChassisCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.BASEBUILDING_REPAIR_FAST);
	}
};

class ActionRepairCarChassis: ActionContinuousBase
{
	protected typename m_LastValidType;
	protected string m_CurrentDamageZone;
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

	override void CreateConditionComponents()  
	{
		m_ConditionItem 	= new CCINonRuined();
		m_ConditionTarget 	= new CCTCursor(UAMaxDistances.REPAIR);
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (player.GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS)
			return false;
		
		CarScript car = CarScript.Cast(target.GetObject());
		if (!car || !player)
		{
			return false;
		}
		
		if (GetGame().IsMultiplayer() && GetGame().IsServer())
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
				float zoneMax 		= car.GetMaxHealth(damageZone, "");
				float randomValue 	= Math.RandomFloatInclusive(zoneMax * 0.05, zoneMax * 0.15);
				
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
				}

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
		{
			action_recive_data = new RepairCarPartActionReciveData();
		}

		super.ReadFromContext(ctx, action_recive_data);
		RepairCarPartActionReciveData recieveDataRepair = RepairCarPartActionReciveData.Cast(action_recive_data);

		if (HasTarget())
		{
			string zone;
			if (!ctx.Read(zone))
			{
				return false;
			}

			recieveDataRepair.m_DamageZoneRecived = zone;
		}

		return true;
	}
	
	override void HandleReciveData(ActionReciveData action_recive_data, ActionData action_data)
	{
		super.HandleReciveData(action_recive_data, action_data);

		RepairCarPartActionReciveData recieveDataRepair = RepairCarPartActionReciveData.Cast(action_recive_data);
		RepairCarPartActionData.Cast(action_data).m_DamageZone = recieveDataRepair.m_DamageZoneRecived;
	}
};