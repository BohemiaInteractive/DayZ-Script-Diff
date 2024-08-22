class UniversalTemperatureSourceLambdaBaseImpl : UniversalTemperatureSourceLambdaBase
{
	override void DryItemsInVicinity(UniversalTemperatureSourceSettings pSettings, vector position, out notnull array<Object> nearestObjects)
	{
		float distanceToTemperatureSource;
		
		foreach (Object nearestObject : nearestObjects)
		{
			ItemBase nearestItem = ItemBase.Cast(nearestObject);

			//! heat transfer to items (not in player possession)
			if (nearestItem && nearestItem.HasWetness() && nearestItem != pSettings.m_Parent && !nearestItem.IsInherited(Man) && !nearestItem.IsUniversalTemperatureSource())
			{
				distanceToTemperatureSource = vector.Distance(nearestItem.GetPosition(), position);
				distanceToTemperatureSource = Math.Max(distanceToTemperatureSource, 0.1);	//min distance cannot be 0 (division by zero)
				
				float dryModifier = 0;				
				
				if (nearestItem.GetWet() >= GameConstants.STATE_DAMP)
				{
					dryModifier = (-1 * m_ExecuteInterval * nearestItem.GetDryingIncrement("groundHeatSource")) / distanceToTemperatureSource;
					Math.Clamp(dryModifier, nearestItem.GetWetMin(), nearestItem.GetWetMax());
					nearestItem.AddWet(dryModifier);
				}
				
				array<EntityAI> cargoEntities = new array<EntityAI>();
				nearestItem.GetInventory().EnumerateInventory(InventoryTraversalType.INORDER, cargoEntities);
				foreach (EntityAI cargoEntity : cargoEntities)
				{
					ItemBase cargoItem = ItemBase.Cast(cargoEntity);
					if (cargoItem)
					{
						dryModifier = 0;
						if (cargoItem.GetWet() >= GameConstants.STATE_DAMP)
						{
							dryModifier = (-1 * m_ExecuteInterval * cargoItem.GetDryingIncrement("groundHeatSource")) / distanceToTemperatureSource;
							Math.Clamp(dryModifier, cargoItem.GetWetMin(), cargoItem.GetWetMax());
							cargoItem.AddWet(dryModifier);
						}
					}
				}
			}
		}
	}
	
	override void WarmAndCoolItemsInVicinity(UniversalTemperatureSourceSettings pSettings, vector position, out notnull array<Object> nearestObjects)
	{
		float distanceToTemperatureSource;
		float tempTarget = pSettings.m_TemperatureItemCap;
		float distanceBasedTemperature;
		EntityAI nearestEntity;
		
		foreach (Object nearestObject : nearestObjects)
		{
			if (Class.CastTo(nearestEntity,nearestObject) && nearestEntity != pSettings.m_Parent && !nearestEntity.IsSelfAdjustingTemperature())
			{
				float temperatureDifference = tempTarget - nearestEntity.GetTemperature();
				
				distanceToTemperatureSource = vector.Distance(nearestEntity.GetPosition(), position);
				distanceToTemperatureSource = Math.Max(distanceToTemperatureSource, 0.1);
				
				float time = m_ExecuteInterval;
				if (m_ExecuteInterval == -1) //bogus time in the first execute
					time = 1;
				
				float distFactor = 1;
				if (vector.Distance(nearestEntity.GetPosition(), position) > pSettings.m_RangeFull)
				{
					distFactor = Math.InverseLerp(pSettings.m_RangeMax,pSettings.m_RangeFull,distanceToTemperatureSource);
					distFactor = Math.Max(distFactor, 0.0); //! dist factor minimum should be at 0
				}
				
				TemperatureDataInterpolated dta = new TemperatureDataInterpolated(tempTarget,ETemperatureAccessTypes.ACCESS_UTS,time,pSettings.m_TemperatureItemCoef * distFactor);
				
				if (nearestEntity.GetInventory())
				{
					UpdateVicinityTemperatureRecursive(nearestEntity,dta);
				}
				else if (nearestEntity.CanHaveTemperature() && !nearestEntity.IsSelfAdjustingTemperature())
				{
					dta.m_HeatPermeabilityCoef = nearestEntity.GetHeatPermeabilityCoef();
					
					if (Math.AbsFloat(temperatureDifference) < GameConstants.TEMPERATURE_SENSITIVITY_THRESHOLD) //ignoring insignificant increments
						nearestEntity.RefreshTemperatureAccess(dta);
					else
						nearestEntity.SetTemperatureEx(dta);
				}
			}
		}
	}
	
	protected void UpdateVicinityTemperatureRecursive(EntityAI ent, TemperatureData dta, float heatPermeabilityCoef = 1.0) //TODO
	{
		float heatPermCoef = heatPermeabilityCoef;
		heatPermCoef *= ent.GetHeatPermeabilityCoef();
		dta.m_HeatPermeabilityCoef = heatPermCoef;
		
		//handle temperature of this entity
		if (ent.CanHaveTemperature() && !ent.IsSelfAdjustingTemperature())
		{
			float temperatureDifference = dta.m_AdjustedTarget - ent.GetTemperature();
			if (Math.AbsFloat(temperatureDifference) < GameConstants.TEMPERATURE_SENSITIVITY_THRESHOLD) //ignoring insignificant increments
				ent.RefreshTemperatureAccess(dta);
			else
				ent.SetTemperatureEx(dta);
		}
		
		// go through any attachments and cargo, recursive
		int inventoryAttCount = ent.GetInventory().AttachmentCount();
		if (inventoryAttCount > 0)
		{
			EntityAI attachmentEnt;
			for (int inAttIdx = 0; inAttIdx < inventoryAttCount; ++inAttIdx)
			{
				if (Class.CastTo(attachmentEnt,ent.GetInventory().GetAttachmentFromIndex(inAttIdx)))
				{
					UpdateVicinityTemperatureRecursive(attachmentEnt,dta,heatPermCoef);
				}
			}
		}
		
		if (ent.GetInventory().GetCargo())
		{
			int inventoryItemCount = ent.GetInventory().GetCargo().GetItemCount();
			if (inventoryItemCount > 0)
			{
				EntityAI cargoEnt;
				for (int j = 0; j < inventoryItemCount; ++j)
				{
					if (Class.CastTo(cargoEnt,ent.GetInventory().GetCargo().GetItem(j)))
					{
						UpdateVicinityTemperatureRecursive(cargoEnt,dta,heatPermCoef);
					}
				}
			}
		}
	}
	
	override void Execute(UniversalTemperatureSourceSettings pSettings, UniversalTemperatureSourceResult resultValues)
	{
		resultValues.m_Temperature = pSettings.m_TemperatureMax;
		
		array<Object> nearestObjects = new array<Object>();
		
		vector pos = pSettings.m_Position;
		if (pSettings.m_Parent != null)
			pos = pSettings.m_Parent.GetPosition();
		
		GetGame().GetObjectsAtPosition(pos, pSettings.m_RangeMax, nearestObjects, null);
		if (nearestObjects.Count() > 0)
		{
			DryItemsInVicinity(pSettings, pos, nearestObjects);
			WarmAndCoolItemsInVicinity(pSettings, pos, nearestObjects);
		}
	}
	
	//! DEPRECATED
	override void DryItemsInVicinity(UniversalTemperatureSourceSettings pSettings)
	{
		array<Object> nearestObjects = new array<Object>();
		
		vector pos = pSettings.m_Position;
		if (pSettings.m_Parent != null)
			pos = pSettings.m_Parent.GetPosition();
		
		GetGame().GetObjectsAtPosition(pos, pSettings.m_RangeMax, nearestObjects, null);

		DryItemsInVicinity(pSettings, pos, nearestObjects);
	}
}

class UniversalTemperatureSourceLambdaConstant : UniversalTemperatureSourceLambdaBaseImpl {}
class UniversalTemperatureSourceLambdaEngine : UniversalTemperatureSourceLambdaBaseImpl
{
	void UniversalTemperatureSourceLambdaEngine()
	{
		m_AffectsPlayer = false;
	}
}

