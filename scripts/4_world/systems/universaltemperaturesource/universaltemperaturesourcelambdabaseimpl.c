class UniversalTemperatureSourceLambdaBaseImpl : UniversalTemperatureSourceLambdaBase
{
	override void DryItemsInVicinity(UniversalTemperatureSourceSettings pSettings)
	{
		float distance;
		array<Object> nearestObjects = new array<Object>();

		vector pos = pSettings.m_Position;
		if (pSettings.m_Parent != null)
			pos = pSettings.m_Parent.GetPosition();

		GetGame().GetObjectsAtPosition(pos, pSettings.m_RangeMax, nearestObjects, null);
		foreach (Object nearestObject : nearestObjects)
		{
			ItemBase nearestItem = ItemBase.Cast(nearestObject);
			//! heat transfer to items (no in player possession)
			if (nearestItem && nearestItem.HasWetness() && nearestItem != pSettings.m_Parent && !nearestItem.IsInherited(Man))
			{
				distance = vector.Distance(nearestItem.GetPosition(), pSettings.m_Position);
				distance = Math.Max(distance, 0.1);	//min distance cannot be 0 (division by zero)
				
				float dryModifier = 0;				
				
				if (nearestItem.GetWet() >= GameConstants.STATE_DAMP)
				{
					dryModifier = (-1 * pSettings.m_UpdateInterval * nearestItem.GetDryingIncrements(EDryingIncrementCategory.GROUND_TEMPSOURCE)) / distance;
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
							dryModifier = (-1 * pSettings.m_UpdateInterval * cargoItem.GetDryingIncrements(EDryingIncrementCategory.GROUND_TEMPSOURCE)) / distance;
							Math.Clamp(dryModifier, cargoItem.GetWetMin(), cargoItem.GetWetMax());
							cargoItem.AddWet(dryModifier);
						}
					}
				}
			}
		}
	}
}

class UniversalTemperatureSourceLambdaConstant : UniversalTemperatureSourceLambdaBaseImpl
{
	override void Execute(UniversalTemperatureSourceSettings pSettings, UniversalTemperatureSourceResult resultValues)
	{
		resultValues.m_Temperature = pSettings.m_TemperatureMax;
		
		DryItemsInVicinity(pSettings);
	}
}

class UniversalTemperatureSourceLambdaEngine : UniversalTemperatureSourceLambdaBase
{
	override void Execute(UniversalTemperatureSourceSettings pSettings, UniversalTemperatureSourceResult resultValues)
	{
		resultValues.m_Temperature = pSettings.m_TemperatureMax;
	}
}
