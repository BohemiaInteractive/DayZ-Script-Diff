class CAContinuousWaterSlot : CAContinuousQuantity
{
	protected float m_PlantThirstyness;
	protected float	m_TimeToComplete;
	protected float m_StartQuantity;
	protected Slot 	m_Slot;

	void CAContinuousWaterSlot(float quantity_used_per_second)
	{
		m_QuantityUsedPerSecond = quantity_used_per_second;
	}
	
	override void Setup(ActionData action_data)
	{
		GardenBase targetGB;
		if (Class.CastTo(targetGB, action_data.m_Target.GetObject()))
		{
			m_SpentQuantity = 0;
			m_StartQuantity = 0;
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
					string selection = selections[s];
					m_Slot = targetGB.GetSlotBySelection(selection);
					if (m_Slot)
						break;
				}
				
				if (m_Slot)
				{
					m_StartQuantity = m_Slot.GetWater();
					m_PlantThirstyness = m_Slot.GetWaterUsage() - m_Slot.GetWater();
				}
			}
			
			m_TimeToComplete = (Math.Min(m_PlantThirstyness,m_ItemQuantity)) / m_QuantityUsedPerSecond;
			if (m_TimeToComplete <= 0.1) // Division by zero prevention
			{
				m_TimeToComplete = 0.1;
			}
		}
	}
	
	override int Execute(ActionData action_data)
	{
		m_ItemQuantity = action_data.m_MainItem.GetQuantity();
		m_ItemMaxQuantity = action_data.m_MainItem.GetQuantityMax();
		
		if (!action_data.m_Player)
		{
			return UA_ERROR;
		}
		
		if (m_ItemQuantity <= 0)
		{
			return UA_FINISHED;
		}
		else
		{
			if (m_Slot && m_SpentQuantity < m_ItemQuantity)
			{
				m_SpentQuantity += m_QuantityUsedPerSecond * action_data.m_Player.GetDeltaT();
				if (m_SpentQuantity > m_ItemQuantity)	// dont transfer more water than the source's quantity
					m_SpentQuantity = m_ItemQuantity;
				
				m_Slot.GiveWater(m_SpentQuantity);

				CalcAndSetQuantity(action_data);
				
				m_SpentQuantity = 0;
				
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
	
	override float GetProgress()
	{
		return -((m_Slot.GetWater() - m_StartQuantity) / m_PlantThirstyness);
	}
	
	//! DEPRECATED
	protected float m_SpentQuantityTotal;
};