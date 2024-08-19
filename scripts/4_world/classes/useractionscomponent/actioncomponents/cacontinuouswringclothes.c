class CAContinuousWringClothes : CAContinuousQuantityRepeat
{
	protected float 	m_InitItemQuantity;
	protected float 	m_SpentQuantityTotal;
	
	void CAContinuousWringClothes( float quantity_used_per_second, float time_to_repeat )
	{
		m_QuantityUsedPerSecond = quantity_used_per_second;
		m_DefaultTimeToRepeat = time_to_repeat;
		m_InitItemQuantity = -1;
	}
	
	override void Setup( ActionData action_data )
	{
		super.Setup( action_data );
		
		if (m_InitItemQuantity < 0)
		{
			m_InitItemQuantity = action_data.m_MainItem.GetWet();
		}
	}
	
	
	override int Execute( ActionData action_data  )
	{
		if ( !action_data.m_Player )
		{
			return UA_ERROR;
		}
				
		float wetness = action_data.m_MainItem.GetWet();		
		int trim_wetness = wetness * 100;		
		float rounded_wetness = trim_wetness/100;
		
		if ( rounded_wetness <= 0.25 )
		{
			return UA_FINISHED;
		}
		else
		{
			if ( m_SpentQuantityTotal < m_InitItemQuantity )
			{
				m_AdjustedQuantityUsedPerSecond = action_data.m_Player.GetSoftSkillsManager().AddSpecialtyBonus( m_QuantityUsedPerSecond, m_Action.GetSpecialtyWeight(), true);
				m_SpentQuantity += m_AdjustedQuantityUsedPerSecond * action_data.m_Player.GetDeltaT();
				m_TimeElpased += action_data.m_Player.GetDeltaT();

				if (m_TimeElpased > m_DefaultTimeToRepeat)
				{
					OnCompletePogress(action_data);
					m_TimeElpased -= m_DefaultTimeToRepeat;
				}
					
				
				if ( GetGame().IsServer() )
				{
					if ( m_SpentUnits )
					{
						m_SpentUnits.param1 = m_SpentQuantity;
						SetACData(m_SpentUnits);
					}

					action_data.m_MainItem.AddWet(-m_SpentQuantity);
				}
				
				m_SpentQuantityTotal += m_SpentQuantity;
				m_SpentQuantity = 0;

				return UA_PROCESSING;
			}
			else
			{
				OnCompletePogress(action_data);
				return UA_FINISHED;
			}
		}
	}
	
	override float GetProgress()
	{
		return ((m_TimeElpased - m_DefaultTimeToRepeat) / m_DefaultTimeToRepeat) + 1;
	}
};