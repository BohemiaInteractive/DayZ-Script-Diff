class CAContinuousQuantityEdible : CAContinuousQuantityRepeat
{
	protected float 	m_InitItemQuantity;
	protected float 	m_SpentQuantityTotal;
	
	void CAContinuousQuantityEdible( float quantity_used_per_second, float time_to_repeat )
	{
		m_QuantityUsedPerSecond = quantity_used_per_second;
		m_DefaultTimeToRepeat = time_to_repeat;
		m_InitItemQuantity = -1;
		m_SpentQuantityTotal = 0;
	}
	
	override void Setup( ActionData action_data )
	{
		super.Setup( action_data );
		
		if (m_InitItemQuantity < 0)
		{
			m_InitItemQuantity = action_data.m_MainItem.GetQuantity();
		}
	}
	
	override void CalcAndSetQuantity( ActionData action_data )
	{	
		//Print("Munch!");
		if ( m_SpentUnits )
		{
			m_SpentUnits.param1 = m_SpentQuantity;
			SetACData(m_SpentUnits);
			
			m_SpentQuantityTotal += m_SpentQuantity;
		}
		//m_SpentQuantity = Math.Floor(m_SpentQuantity);

		PlayerBase ntarget = PlayerBase.Cast( action_data.m_Target.GetObject() );
		if ( ntarget )
		{
			if ( GetGame().IsServer() ) 
			{
				//action_data.m_MainItem.Consume(targetObject, m_SpentQuantity);
				ntarget.Consume(action_data.m_MainItem, m_SpentQuantity, EConsumeType.ITEM_CONTINUOUS);
				
			}
		}
		else
		{
			if ( GetGame().IsServer() ) 
			{
				//action_data.m_MainItem.Consume(action_data.m_Player, m_SpentQuantity);
				action_data.m_Player.Consume(action_data.m_MainItem, m_SpentQuantity, EConsumeType.ITEM_CONTINUOUS);
			}
		}
	}
	
	override float GetProgress()
	{
		//return super.GetProgress();
		if (m_SpentQuantity > 0)
		{
			return Math.Clamp((1 - (m_InitItemQuantity - m_SpentQuantity - m_SpentQuantityTotal ) / m_InitItemQuantity), 0, 1);
		}
		else
		{
			return Math.Clamp((1 - (m_InitItemQuantity - m_SpentQuantityTotal ) / m_InitItemQuantity), 0, 1);
		}
	}
};