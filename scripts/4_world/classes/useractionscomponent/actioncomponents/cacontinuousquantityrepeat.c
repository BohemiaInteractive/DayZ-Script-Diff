class CAContinuousQuantityRepeat : CAContinuousBase
{
	protected float 				m_ItemQuantity;
	protected float 				m_SpentQuantity;
	protected float 				m_ItemMaxQuantity;
	protected float					m_TimeElpased;
	protected float 				m_QuantityUsedPerSecond;
	protected float 				m_AdjustedQuantityUsedPerSecond;
	protected float 				m_DefaultTimeToRepeat;
	protected ref Param1<float>		m_SpentUnits;
	
	void CAContinuousQuantityRepeat( float quantity_used_per_second, float time_to_repeat )
	{
		m_QuantityUsedPerSecond = quantity_used_per_second;
		m_DefaultTimeToRepeat = time_to_repeat;
	}
	
	override void Setup( ActionData action_data )
	{
		m_TimeElpased = 0;
		m_SpentQuantity = 0;
		
		if ( !m_SpentUnits )
		{ 
			m_SpentUnits = new Param1<float>( 0 );
		}
		else
		{	
			m_SpentUnits.param1 = 0;
		}		
		
		m_ItemMaxQuantity = action_data.m_MainItem.GetQuantityMax();
		m_ItemQuantity = action_data.m_MainItem.GetQuantity();
	}
	
	
	override int Execute( ActionData action_data  )
	{
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
			if ( m_SpentQuantity < m_ItemQuantity )
			{
				m_AdjustedQuantityUsedPerSecond = action_data.m_Player.GetSoftSkillsManager().AddSpecialtyBonus( m_QuantityUsedPerSecond, m_Action.GetSpecialtyWeight(), true);		
				m_SpentQuantity += m_AdjustedQuantityUsedPerSecond * action_data.m_Player.GetDeltaT();
				m_TimeElpased += action_data.m_Player.GetDeltaT();
				
				if ( m_TimeElpased >= m_DefaultTimeToRepeat )
				{
					CalcAndSetQuantity( action_data );
					Setup(action_data);	//reset data after repeat
				}
				
				return UA_PROCESSING;
			}
			else
			{
				CalcAndSetQuantity( action_data );
				OnCompletePogress(action_data);
				return UA_FINISHED;
			}
		}
	}
	
	override int Cancel( ActionData action_data )
	{
		if ( !action_data.m_Player )
		{
			return UA_ERROR;
		}
		if (GetProgress() > 0)
		{
			CalcAndSetQuantity( action_data );
		}
		return UA_CANCEL;
	}	
	
	override float GetProgress()
	{
		//float progress = ( m_ItemQuantity - m_SpentQuantity ) / m_ItemMaxQuantity;
		return ( m_ItemQuantity - m_SpentQuantity ) / m_ItemMaxQuantity;
	}
	
	//---------------------------------------------------------------------------
	
	
	void CalcAndSetQuantity( ActionData action_data )
	{
		if ( GetGame().IsServer() )
		{
			if ( m_SpentUnits )
			{
				m_SpentUnits.param1 = m_SpentQuantity;
				SetACData(m_SpentUnits);
			}
			
			action_data.m_MainItem.AddQuantity( -m_SpentQuantity, false, false );
		}
	}
}