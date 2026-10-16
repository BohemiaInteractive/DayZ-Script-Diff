class RepairMotorbikePartReceiveData : RepairVehiclePartActionReciveData {}
class RepairMotorbikePartActionData : RepairVehiclePartActionData {}

class ActionRepairMotorbikePartCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousRepeat(UATimeSpent.BASEBUILDING_REPAIR_FAST);
	}
}

class ActionRepairMotorbikePart : ActionRepairVehiclePartBase
{	
	void ActionRepairMotorbikePart()
	{
		m_CallbackClass 	= ActionRepairMotorbikePartCB;
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
	
	override bool CanBeUsedWithBrokenLegs()
	{
		return false;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{		
		Object targetObject = target.GetObject();
		Object targetParent = target.GetParent();
		MotorbikePart motorbikePart = MotorbikePart.Cast(targetObject);
		
		if (!motorbikePart || !player)
			return false;	
		
		if (g_Game.IsMultiplayer() && g_Game.IsServer())
			return true;
			
		if (motorbikePart)
		{
			if (targetParent != null)
			{
				InventoryLocation loc = new InventoryLocation();
				bool isPresent = motorbikePart.GetInventory().GetCurrentInventoryLocation(loc);
				
				if (!isPresent || loc.GetSlot() == -1)
					return false;
			}
			
			int zoneHP = motorbikePart.GetHealthLevel("");
			return zoneHP > GameConstants.STATE_WORN && zoneHP < GameConstants.STATE_RUINED;
		}
		
		return false;
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		Object tgObject	= action_data.m_Target.GetObject();
		
		string damageZone = RepairMotorbikePartActionData.Cast(action_data).m_DamageZone;
		if (!g_Game.IsMultiplayer())
			damageZone = m_CurrentDamageZone;
		
		if (tgObject)
		{
			MotorbikePart motorbikePart = MotorbikePart.Cast(tgObject);
			if (motorbikePart)
			{
				int newDmgLevel = Math.Clamp(motorbikePart.GetHealthLevel(damageZone) - 1, GameConstants.STATE_WORN, GameConstants.STATE_RUINED);
				float zoneMax = motorbikePart.GetMaxHealth("", "");
				float randomValue = Math.RandomFloatInclusive(zoneMax * 0.05, zoneMax * 0.15);
				
				switch (newDmgLevel)
				{
					case GameConstants.STATE_BADLY_DAMAGED:
						motorbikePart.SetHealth("", "", (zoneMax * GameConstants.DAMAGE_RUINED_VALUE) + randomValue);
						break;

					case GameConstants.STATE_DAMAGED:
						motorbikePart.SetHealth("", "", (zoneMax * GameConstants.DAMAGE_BADLY_DAMAGED_VALUE) + randomValue);
						break;

					case GameConstants.STATE_WORN:
						motorbikePart.SetHealth("", "", (zoneMax * GameConstants.DAMAGE_DAMAGED_VALUE) + randomValue);
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
		RepairMotorbikePartActionData actionData = new RepairMotorbikePartActionData();
		return actionData;
	}
	
	override void WriteToContext(ParamsWriteContext ctx, ActionData action_data)
	{
		super.WriteToContext(ctx, action_data);
		RepairMotorbikePartActionData repairActionData;

		if (HasTarget() && Class.CastTo(repairActionData, action_data))
		{
			repairActionData.m_DamageZone = m_CurrentDamageZone;
			ctx.Write(repairActionData.m_DamageZone);
		}
	}
	
	override bool ReadFromContext(ParamsReadContext ctx, out ActionReciveData action_recive_data)
	{
		if (!action_recive_data)
			action_recive_data = new RepairMotorbikePartReceiveData();

		super.ReadFromContext(ctx, action_recive_data);
		RepairMotorbikePartReceiveData receiveDataRepair = RepairMotorbikePartReceiveData.Cast(action_recive_data);

		if (HasTarget())
		{
			string zone;
			if (!ctx.Read(zone))
				return false;

			receiveDataRepair.m_DamageZoneRecived = zone;
		}

		return true;
	}
	
	override void HandleReciveData(ActionReciveData action_recive_data, ActionData action_data)
	{
		super.HandleReciveData(action_recive_data, action_data);

		RepairMotorbikePartReceiveData receiveDataRepair = RepairMotorbikePartReceiveData.Cast(action_recive_data);
		RepairMotorbikePartActionData.Cast(action_data).m_DamageZone = receiveDataRepair.m_DamageZoneRecived;
	}
}
