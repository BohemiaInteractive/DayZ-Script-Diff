class UniversalTemperatureSourceLambdaBase
{
	const float HEAT_THROUGH_AIR_COEF = 0.003;

	void UniversalTemperatureSourceLambdaBase() {};
	void ~UniversalTemperatureSourceLambdaBase() {};

	void Execute(UniversalTemperatureSourceSettings pSettings, UniversalTemperatureSourceResult resultValues) {};
	
	void DryItemsInVicinity(UniversalTemperatureSourceSettings pSettings)
	{
		float distance;
		array<Object> nearestObjects = new array<Object>;

		vector pos = pSettings.m_Position;
		if (pSettings.m_Parent != null)
		{
			pos = pSettings.m_Parent.GetPosition();
		}

		GetGame().GetObjectsAtPosition(pos, pSettings.m_RangeMax, nearestObjects, null);
		for (int i = 0; i < nearestObjects.Count(); i++)
		{
			EntityAI nearestEnt = EntityAI.Cast(nearestObjects.Get(i));
			//! heat transfer to items (no in player possession)
			if (nearestEnt && nearestEnt.HasWetness() && nearestEnt != pSettings.m_Parent && nearestEnt.GetParent() == null)
			{
				float wetness = nearestEnt.GetWet();
	
				//! drying of items around the fireplace (based on distance)
				if (wetness > 0)
				{
					distance = vector.Distance(nearestEnt.GetPosition(), pSettings.m_Position);
					distance = Math.Max(distance, 0.1);	//min distance cannot be 0 (division by zero)
					
					wetness = wetness * (HEAT_THROUGH_AIR_COEF / distance);
					wetness = Math.Clamp(wetness, nearestEnt.GetWetMin(), nearestEnt.GetWetMax());
					nearestEnt.AddWet(-wetness);
				}
			}
		}
	}
}

class UniversalTemperatureSourceLambdaConstant : UniversalTemperatureSourceLambdaBase
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
