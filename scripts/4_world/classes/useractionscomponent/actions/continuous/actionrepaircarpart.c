class RepairCarPartActionReciveData : RepairVehiclePartActionReciveData
{}

class RepairCarPartActionData : RepairVehiclePartActionData
{}

class ActionRepairCarPartCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousRepeat(UATimeSpent.BASEBUILDING_REPAIR_FAST);
	}
}

class ActionRepairCarPart : ActionContinuousBase
{	
	typename m_LastValidType;
	string m_CurrentDamageZone = "";
	int m_LastValidComponentIndex = -1;
	
	void ActionRepairCarPart()
	{
		m_CallbackClass = ActionRepairCarPartCB;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
		
		m_CommandUID 		= DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
		m_StanceMask 		= DayZPlayerConstants.STANCEMASK_ERECT;
		m_FullBody			= true;
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
		CarDoor carDoor = CarDoor.Cast(targetObject);
		
		if (!carDoor || !player)
			return false;	
		
		if (GetGame().IsMultiplayer() && GetGame().IsServer())
			return true;
			
		if (carDoor)
		{
			//Check if item is attached to car -> If so, block repair if door is closed
			if (targetParent != null)
			{
				InventoryLocation loc = new InventoryLocation();
				bool isPresent = carDoor.GetInventory().GetCurrentInventoryLocation(loc);
				
				if (!isPresent || loc.GetSlot() == -1)
					return false;
				
				string slotName = InventorySlots.GetSlotName(loc.GetSlot());
				if (slotName && CarScript.Cast(targetParent).GetCarDoorsState(slotName) != CarDoorState.DOORS_OPEN)
					return false;
			}
			
			//Check health level of door
			int zoneHP = carDoor.GetHealthLevel("");
			return zoneHP > GameConstants.STATE_WORN && zoneHP < GameConstants.STATE_RUINED;
		}
		
		return false;
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		Object tgObject	= action_data.m_Target.GetObject();
		
		string damageZone = RepairCarPartActionData.Cast(action_data).m_DamageZone;
		if (!GetGame().IsMultiplayer())
			damageZone = m_CurrentDamageZone;
		
		if (tgObject)
		{
			CarDoor carDoor = CarDoor.Cast(tgObject);
			if (carDoor)
			{
				int newDmgLevel = Math.Clamp(carDoor.GetHealthLevel(damageZone) - 1, GameConstants.STATE_WORN, GameConstants.STATE_RUINED);
				float zoneMax = carDoor.GetMaxHealth("", "");
				float randomValue = Math.RandomFloatInclusive(zoneMax * 0.05, zoneMax * 0.15);
				
				switch (newDmgLevel)
				{
					case GameConstants.STATE_BADLY_DAMAGED:
						carDoor.SetHealth("", "", (zoneMax * GameConstants.DAMAGE_RUINED_VALUE) + randomValue);
						break;

					case GameConstants.STATE_DAMAGED:
						carDoor.SetHealth("", "", (zoneMax * GameConstants.DAMAGE_BADLY_DAMAGED_VALUE) + randomValue);
						break;

					case GameConstants.STATE_WORN:
						carDoor.SetHealth("", "", (zoneMax * GameConstants.DAMAGE_DAMAGED_VALUE) + randomValue);
						break;
					
					default:
						break;
				}

				if (action_data.m_MainItem.HasQuantity())
				{
					if (action_data.m_MainItem.GetQuantity() > 1)
					{
						int qnt = action_data.m_MainItem.GetQuantity();
						qnt -= action_data.m_MainItem.GetQuantityMax() * 0.25;
						action_data.m_MainItem.SetQuantity(qnt);
					}
					else
						action_data.m_MainItem.Delete();
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
