class CAContinuousWaterSlot : CAContinuousQuantity
{
	protected float m_PlantThirstyness;
	protected float	m_TimeToComplete;
	protected float m_SpentQuantityTotal;
	protected float m_StartQuantity;
	protected Slot 	m_Slot;

	void CAContinuousWaterSlot( float quantity_used_per_second )
	{
		m_QuantityUsedPerSecond = quantity_used_per_second;
	}
	
	override void Setup( ActionData action_data )
	{
		GardenBase target_GB;
			
		if ( Class.CastTo(target_GB, action_data.m_Target.GetObject()) )
		{
			m_SpentQuantity = 0;
			m_StartQuantity = action_data.m_MainItem.GetQuantity();
			if ( !m_SpentUnits )
			{ 
				m_SpentUnits = new Param1<float>(0);
			}
			else
			{	
				m_SpentUnits.param1 = 0;
			}
			
			if ( action_data.m_MainItem )
				m_ItemQuantity = action_data.m_MainItem.GetQuantity();
			
			if ( target_GB ) 
			{
				array<string> selections = new array<string>;
				target_GB.GetActionComponentNameList(action_data.m_Target.GetComponentIndex(), selections);
	
				for (int s = 0; s < selections.Count(); s++)
				{
					string selection = selections[s];
					m_Slot = target_GB.GetSlotBySelection( selection );
					if (m_Slot)
						break;
				}
			
				m_PlantThirstyness = m_Slot.GetWaterUsage() - m_Slot.GetWater();
			}
			
			m_TimeToComplete = (Math.Min(m_PlantThirstyness,m_ItemQuantity))/m_QuantityUsedPerSecond;
			
			if (m_TimeToComplete <= 0.1) // Division by zero prevention
			{
				m_TimeToComplete = 0.1;
			}
		}
	}
	
	override int Execute( ActionData action_data  )
	{
		GardenBase target_GB;
		Class.CastTo(target_GB,  action_data.m_Target.GetObject() );
		m_ItemQuantity = action_data.m_MainItem.GetQuantity();
		m_ItemMaxQuantity = action_data.m_MainItem.GetQuantityMax();
		
		if ( !action_data.m_Player )
		{
			return UA_ERROR;
		}
		
		if ( m_ItemQuantity <= 0 )
		{
			return UA_FINISHED;
		}
		else
		{
			if ( m_Slot && m_SpentQuantity < m_ItemQuantity )
			{
				m_SpentQuantity += m_QuantityUsedPerSecond * action_data.m_Player.GetDeltaT();
				float water = action_data.m_Player.GetSoftSkillsManager().AddSpecialtyBonus( m_SpentQuantity, m_Action.GetSpecialtyWeight(), true );
				m_Slot.GiveWater( water );
				
				m_SpentQuantityTotal += m_SpentQuantity;
				CalcAndSetQuantity( action_data );
				
				m_SpentQuantity = 0;
				
				return UA_PROCESSING;
			}
			else
			{
				CalcAndSetQuantity( action_data );
				OnCompletePogress( action_data );
				
				return UA_FINISHED;
			}
		}
	}
	
	override float GetProgress()
	{		
		if ( m_ItemQuantity <= 0 )
			return 1;
		float progress = -(m_SpentQuantityTotal / m_StartQuantity);
		return progress;
	}
};