class CAContinuousFertilizeGardenSlot : CAContinuousQuantity
{
	protected float m_SlotFertilizerNeed;
	protected float m_AlreadyFilledAmount; // amount of fertilizer present within slot during the setup of action
	protected float	m_TimeToComplete;
	protected Slot 	m_Slot;
	protected string m_Selection;

	void CAContinuousFertilizeGardenSlot(float quantity_used_per_second)
	{
		m_QuantityUsedPerSecond = quantity_used_per_second;
		m_TimeToComplete = 0;
	}
	
	override void Setup(ActionData action_data)
	{
		GardenBase targetGB;
		if (Class.CastTo(targetGB,  action_data.m_Target.GetObject()))
		{
			m_SpentQuantity = 0;
		
			if (!m_SpentUnits)
			{
				m_SpentUnits = new Param1<float>(0);
			}
			else
			{	
				m_SpentUnits.param1 = 0;
			}
			
			if (action_data.m_MainItem) 
				m_ItemQuantity = action_data.m_MainItem.GetQuantity();
			
			if (targetGB) 
			{
				array<string> selections = new array<string>;
				targetGB.GetActionComponentNameList(action_data.m_Target.GetComponentIndex(), selections);
	
				for (int s = 0; s < selections.Count(); s++)
				{
					m_Selection = selections[s];
					m_Slot = targetGB.GetSlotBySelection(m_Selection);
					if (m_Slot)
						break;
				}
				
				string itemType = action_data.m_MainItem.GetType();
				float consumedQuantity = g_Game.ConfigGetFloat("cfgVehicles " + itemType + " Horticulture ConsumedQuantity");
				float actionLength = g_Game.ConfigGetFloat("cfgVehicles " + itemType + " Horticulture FertilizeLength");
				if (actionLength == 0)
					actionLength = 1;

				m_Slot.SetFertilizerQuantityMax(consumedQuantity);
				m_AlreadyFilledAmount = m_Slot.GetFertilizerQuantity();
				m_SlotFertilizerNeed = consumedQuantity - m_AlreadyFilledAmount;
			}

			float defaultTimeComplete = consumedQuantity / m_QuantityUsedPerSecond;
			float speedMultiplier = defaultTimeComplete / actionLength;
			m_QuantityUsedPerSecond *= speedMultiplier;
		}
	}
	
	
	override int Execute(ActionData action_data)
	{		
		if (!action_data.m_Player)
		{
			return UA_ERROR;
		}
				
		if (m_SpentQuantity >= m_ItemQuantity)
		{
			return UA_FINISHED;
		}
		else
		{
			if (m_SpentQuantity <= m_ItemQuantity)
			{
				//! Reset slot fertilizer type and quantity in case current type in target slot is not matching
				string itemType = action_data.m_MainItem.GetType();
				if (m_Slot.GetFertilityType() != "" && m_Slot.GetFertilityType() != itemType)
				{
					m_Slot.SetFertilityType(itemType);
				}
				
				m_SpentQuantity = m_QuantityUsedPerSecond * action_data.m_Player.GetDeltaT();
				
				GardenBase gardenBase;
				if (!Class.CastTo(gardenBase, action_data.m_Target.GetObject()))
				{
					return UA_ERROR;
				}
				
				FertilizeSlot(action_data.m_MainItem, gardenBase, m_SpentQuantity);

				if (g_Game.IsServer() || !g_Game.IsMultiplayer())
				{
					action_data.m_MainItem.AddQuantity(-m_SpentQuantity);
				}
				
				return UA_PROCESSING;
			}
			else
			{
				CalcAndSetQuantity(action_data);
				OnCompletePogress(action_data);
				return UA_FINISHED;
			}
		}
	}
	
	protected void FertilizeSlot(ItemBase item, GardenBase gardenBase, float consumedQuantity)
	{
		if (m_Slot)
		{
			string itemType = item.GetType();
			if (m_Slot.GetFertilityType() == "" || m_Slot.GetFertilityType() == itemType)
			{
				m_Slot.SetFertilityType(itemType);
				
				float addEnergyToSlot = g_Game.ConfigGetFloat(string.Format("cfgVehicles %1 Horticulture AddEnergyToSlot", itemType));
				m_Slot.m_FertilizerUsage = g_Game.ConfigGetFloat(string.Format("cfgVehicles %1 Horticulture ConsumedQuantity", itemType));
				
				float coef = Math.Clamp(consumedQuantity / m_Slot.m_FertilizerUsage, 0.0, 1.0);
				addEnergyToSlot = coef * addEnergyToSlot;
				
				float fertilizerMax = m_Slot.GetFertilizerQuantityMax();				
				float newFertilizerQuantity = (m_Slot.m_FertilizerQuantity + consumedQuantity);
				newFertilizerQuantity = Math.Clamp(newFertilizerQuantity, 0.0, fertilizerMax);
				m_Slot.SetFertilizerQuantity(newFertilizerQuantity);
				
				float newfertility = (m_Slot.m_Fertility + addEnergyToSlot);
				float fertilityMax = m_Slot.GetFertilityMax();
				newfertility = Math.Clamp(newfertility, 0.0, fertilityMax);
				m_Slot.SetFertility(newfertility);
			}
		}
	}
	
	override float GetProgress()
	{	
		return -((m_Slot.GetFertilizerQuantity() - m_AlreadyFilledAmount) / m_SlotFertilizerNeed);
	}
	
	//! DEPRECATED
	protected float m_SpentQuantityTotal;
	protected float m_StartQuantity;
};