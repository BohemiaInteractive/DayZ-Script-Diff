class UniversalTemperatureSourceLambdaBaseImpl : UniversalTemperatureSourceLambdaBase
{
	override void DryItemsInVicinity(UniversalTemperatureSourceSettings pSettings, vector position, out notnull array<EntityAI> nearestObjects)
	{
		float distanceToTemperatureSource;
		
		foreach (Object nearestObject : nearestObjects)
		{
			ItemBase nearestItem = ItemBase.Cast(nearestObject);
			ItemBase temperatureSource = ItemBase.Cast(pSettings.m_Parent);

			//! heat transfer to items (not in player possession)
			if (nearestItem && nearestItem.HasWetness() && nearestItem != pSettings.m_Parent && !nearestItem.IsInherited(Man) && !nearestItem.IsUniversalTemperatureSource())
			{
				distanceToTemperatureSource = vector.Distance(nearestItem.GetPosition(), position); //!getting the actual distance
				distanceToTemperatureSource = Math.Max(distanceToTemperatureSource, 0.001); //! min distance cannot be 0 (division by zero)
				distanceToTemperatureSource = 1 / distanceToTemperatureSource; //! calculating the coefficient we will use to deterimne strength of drying
				distanceToTemperatureSource = Math.Clamp(distanceToTemperatureSource, 0.0, 1.0); //! making sure it is between 0.0 and 1.0
				
				float dryValue = 0;
				
				if (nearestItem.GetWet() >= GameConstants.STATE_DAMP)
				{
					dryValue = (-1 * m_ExecuteInterval * nearestItem.GetDryingIncrement("groundHeatSource")) * pSettings.m_ItemDryModifier * distanceToTemperatureSource;
					Math.Clamp(dryValue, nearestItem.GetWetMin(), nearestItem.GetWetMax());
					nearestItem.AddWet(dryValue);
				}
				
				array<EntityAI> cargoEntities = new array<EntityAI>();
				nearestItem.GetInventory().EnumerateInventory(InventoryTraversalType.INORDER, cargoEntities);
				foreach (EntityAI cargoEntity : cargoEntities)
				{
					ItemBase cargoItem = ItemBase.Cast(cargoEntity);
					if (cargoItem)
					{
						dryValue = 0;
						if (cargoItem.GetWet() >= GameConstants.STATE_DAMP)
						{
							dryValue = (-1 * m_ExecuteInterval * cargoItem.GetDryingIncrement("groundHeatSource")) *  pSettings.m_ItemDryModifier * distanceToTemperatureSource;
							Math.Clamp(dryValue, cargoItem.GetWetMin(), cargoItem.GetWetMax());
							cargoItem.AddWet(dryValue);
						}
					}
				}
			}
		}
	}
	
	override void WarmAndCoolItemsInVicinity(UniversalTemperatureSourceSettings pSettings, vector position, out notnull array<EntityAI> nearestObjects)
	{
		float distanceToTemperatureSource;
		float tempTarget = pSettings.m_TemperatureItemCap;
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
					
					if (Math.AbsFloat(temperatureDifference) >= GameConstants.TEMPERATURE_SENSITIVITY_THRESHOLD || !nearestEntity.IsFreezeThawProgressFinished()) //ignoring insignificant increments
						nearestEntity.SetTemperatureEx(dta);
					else
						nearestEntity.RefreshTemperatureAccess(dta);
				}
			}
		}
	}
	
	protected void UpdateVicinityTemperatureRecursive(EntityAI ent, TemperatureData dta, float heatPermeabilityCoef = 1.0)
	{
		float heatPermCoef = heatPermeabilityCoef;
		heatPermCoef *= ent.GetHeatPermeabilityCoef();
		dta.m_HeatPermeabilityCoef = heatPermCoef;
		
		//handle temperature of this entity
		if (ent.CanHaveTemperature() && !ent.IsSelfAdjustingTemperature())
		{
			float temperatureDifference = dta.m_AdjustedTarget - ent.GetTemperature();
			if (Math.AbsFloat(temperatureDifference) >= GameConstants.TEMPERATURE_SENSITIVITY_THRESHOLD || !ent.IsFreezeThawProgressFinished()) //ignoring insignificant increments
				ent.SetTemperatureEx(dta);
			else
				ent.RefreshTemperatureAccess(dta);
		}
		
		// go through any attachments and cargo, recursive
		int inventoryAttCount = ent.GetInventory().AttachmentCount();
		for (int inAttIdx = 0; inAttIdx < inventoryAttCount; ++inAttIdx)
		{
			EntityAI attachmentEnt;
			if (Class.CastTo(attachmentEnt,ent.GetInventory().GetAttachmentFromIndex(inAttIdx)))
			{
				UpdateVicinityTemperatureRecursive(attachmentEnt,dta,heatPermCoef);
			}
		}
		
		CargoBase cargo = ent.GetInventory().GetCargo();
		if (cargo)
		{
			int inventoryItemCount = cargo.GetItemCount();
			for (int j = 0; j < inventoryItemCount; ++j)
			{
				EntityAI cargoEnt;
				if (Class.CastTo(cargoEnt, cargo.GetItem(j)))
				{
					UpdateVicinityTemperatureRecursive(cargoEnt, dta, heatPermCoef);
				}
			}
		}
	}
	
	override void Execute(UniversalTemperatureSourceSettings pSettings, UniversalTemperatureSourceResult resultValues)
	{
		resultValues.m_TemperatureItem = pSettings.m_TemperatureItemCap;
		resultValues.m_TemperatureHeatcomfort = pSettings.m_TemperatureCap;
				
		vector pos = pSettings.m_Position;
		if (pSettings.m_Parent != null)
			pos = pSettings.m_Parent.GetPosition();
		
		// Define half-size (range)
		float halfRange = pSettings.m_RangeMax;
		
		// Calculate min and max positions of the box
		vector minPos = pos - Vector(halfRange, halfRange / 2, halfRange);
		vector maxPos = pos + Vector(halfRange, halfRange / 2, halfRange);
		
		array<EntityAI> nearestObjects = {};
		DayZPlayerUtils.SceneGetEntitiesInBox(minPos, maxPos, nearestObjects, QueryFlags.DYNAMIC);
		
		for (int i = nearestObjects.Count() - 1; i >= 0; --i)
		{
			EntityAI entity = nearestObjects[i];
			if (entity)
			{
				vector objPos = entity.GetPosition();
				float distance = vector.Distance(objPos, pos);
				if (distance > pSettings.m_RangeMax)
					nearestObjects.Remove(i);
			}
		}
		
		if (nearestObjects.Count() > 0)
		{
			DryItemsInVicinity(pSettings, pos, nearestObjects);
			WarmAndCoolItemsInVicinity(pSettings, pos, nearestObjects);
		}
	}
	
	//! DEPRECATED
	override void DryItemsInVicinity(UniversalTemperatureSourceSettings pSettings)
	{
		vector pos = pSettings.m_Position;
		if (pSettings.m_Parent != null)
			pos = pSettings.m_Parent.GetPosition();
		
		// Define half-size (range)
		float halfRange = pSettings.m_RangeMax;
		
		// Calculate min and max positions of the box
		vector minPos = pos - Vector(halfRange, halfRange / 2, halfRange);
		vector maxPos = pos + Vector(halfRange, halfRange / 2, halfRange);
		
		array<EntityAI> nearestObjects = {};
		DayZPlayerUtils.SceneGetEntitiesInBox(minPos, maxPos, nearestObjects, QueryFlags.DYNAMIC);
		
		for (int i = nearestObjects.Count() - 1; i >= 0; --i)
		{
			EntityAI entity = nearestObjects[i];
			if (entity)
			{
				vector objPos = entity.GetPosition();
				float distance = vector.Distance(objPos, pos);
				if (distance > pSettings.m_RangeMax)
					nearestObjects.Remove(i);
			}
		}
		
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

