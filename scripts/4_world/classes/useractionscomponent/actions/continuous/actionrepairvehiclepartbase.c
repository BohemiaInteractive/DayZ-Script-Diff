class RepairVehiclePartActionReciveData : ActionReciveData
{
	string m_DamageZoneRecived;
}

class RepairVehiclePartActionData : ActionData
{
	string m_DamageZone;
}

class ActionRepairVehiclePartCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousRepeat(UATimeSpent.BASEBUILDING_REPAIR_FAST);
	}
};

class ActionRepairVehiclePartBase : ActionContinuousBase
{
	protected string m_CurrentDamageZone; // needs to be assigned in action condition so it can be properly written to context

	override void CreateConditionComponents()  
	{
		m_ConditionItem 	= new CCINonRuined();
		m_ConditionTarget 	= new CCTCursor(UAMaxDistances.REPAIR);
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		AdjustVehicleHealthServer(action_data);
		AdjustItemQuantityServer(action_data);
	}
	
	protected void AdjustVehicleHealthServer(ActionData action_data)
	{
		Object tgObject = action_data.m_Target.GetObject();
		
		string damageZone = RepairVehiclePartActionData.Cast(action_data).m_DamageZone;
		if (!GetGame().IsMultiplayer())
			damageZone = m_CurrentDamageZone;

		if (tgObject && damageZone != "")
		{
			Transport vehicle = Transport.Cast(tgObject);
			if (vehicle)
			{
				int newDmgLevel = Math.Clamp(vehicle.GetHealthLevel(damageZone) - 1, GameConstants.STATE_WORN, GameConstants.STATE_RUINED);
				float zoneMax 		= vehicle.GetMaxHealth(damageZone, "");
				float randomValue 	= Math.RandomFloatInclusive(zoneMax * 0.05, zoneMax * 0.15);
				
				switch (newDmgLevel)
				{
					case GameConstants.STATE_BADLY_DAMAGED:
						vehicle.SetHealth(damageZone, "", (zoneMax * GameConstants.DAMAGE_RUINED_VALUE) + randomValue);
					break;
					case GameConstants.STATE_DAMAGED:
						vehicle.SetHealth(damageZone, "", (zoneMax * GameConstants.DAMAGE_BADLY_DAMAGED_VALUE) + randomValue);
					break;
					case GameConstants.STATE_WORN:
						vehicle.SetHealth(damageZone, "", (zoneMax * GameConstants.DAMAGE_DAMAGED_VALUE) + randomValue);
					break;
				}
			}
		}
	}
	
	protected void AdjustItemQuantityServer(ActionData action_data)
	{
		if (action_data.m_MainItem.HasQuantity())
		{
			if (action_data.m_MainItem.GetQuantity() > 1)
			{
				int qnt = action_data.m_MainItem.GetQuantity() - action_data.m_MainItem.GetQuantityMax() * 0.25;
				action_data.m_MainItem.SetQuantity(qnt);
			}
			else
			{
				action_data.m_MainItem.Delete();
			}
		}
	}

	override ActionData CreateActionData()
	{
		RepairVehiclePartActionData actionData = new RepairVehiclePartActionData();
		return actionData;
	}
	
	override void WriteToContext(ParamsWriteContext ctx, ActionData action_data)
	{
		super.WriteToContext(ctx, action_data);
		RepairVehiclePartActionData repairActionData;

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
			action_recive_data = new RepairVehiclePartActionReciveData();
		}

		super.ReadFromContext(ctx, action_recive_data);
		RepairVehiclePartActionReciveData recieveDataRepair = RepairVehiclePartActionReciveData.Cast(action_recive_data);

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

		RepairVehiclePartActionReciveData recieveDataRepair = RepairVehiclePartActionReciveData.Cast(action_recive_data);
		RepairVehiclePartActionData.Cast(action_data).m_DamageZone = recieveDataRepair.m_DamageZoneRecived;
	}
};