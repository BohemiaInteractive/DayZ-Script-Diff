class CAContinuousEmpty : CAContinuousBase
{
	protected float 				m_TargetUnits;
	protected float 				m_SpentQuantity;
	protected float 				m_SpentQuantity_total;
	protected float 				m_AdjustedQuantityEmptiedPerSecond;
	protected float 				m_QuantityEmptiedPerSecond;
	protected bool 					m_WringingClothes;
	protected ref Param1<float>		m_SpentUnits;
	protected float 				m_TimeElpased;
	protected float 				m_DefaultTimeStep = 0.25;
	
	void CAContinuousEmpty( float quantity_emptied_per_second )
	{
		m_QuantityEmptiedPerSecond = quantity_emptied_per_second;
	}
	
	override void Setup( ActionData action_data )
	{
		if ( !m_SpentUnits )
		{ 
			m_SpentUnits = new Param1<float>(0);
		}
		else
		{	
			m_SpentUnits.param1 = 0;
		}

		if ( action_data.m_MainItem.IsClothing() )
		{
			m_WringingClothes = true;
			
			m_SpentQuantity = action_data.m_MainItem.GetWetMax() - action_data.m_MainItem.GetWet();
			m_TargetUnits = action_data.m_MainItem.GetWetMax();
		}
		else
		{
			m_WringingClothes = false;
			
			m_SpentQuantity = action_data.m_MainItem.GetQuantityMin();
			m_TargetUnits = action_data.m_MainItem.GetQuantity();
		}
		
		m_AdjustedQuantityEmptiedPerSecond = m_QuantityEmptiedPerSecond;//removed softskills 
	}
	
	override int Execute( ActionData action_data  )
	{
		if ( !action_data.m_Player )
		{
			return UA_ERROR;
		}
		
		if ( m_WringingClothes && action_data.m_MainItem.GetWet() >= action_data.m_MainItem.GetWetMax() )
		{
			return UA_FINISHED;
		}
		else
		{
			if ( m_SpentQuantity_total <= m_TargetUnits )
			{
				m_SpentQuantity += m_AdjustedQuantityEmptiedPerSecond * action_data.m_Player.GetDeltaT();
				m_TimeElpased += action_data.m_Player.GetDeltaT();
				
				if ( m_TimeElpased >= m_DefaultTimeStep )
				{
					CalcAndSetQuantity( action_data );
					m_TimeElpased = 0;
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
		if ( !action_data.m_Player || !action_data.m_MainItem )
		{
			return UA_ERROR;
		}
		
		CalcAndSetQuantity( action_data );
		return UA_CANCEL;
	}	
	
	override float GetProgress()
	{
		return m_SpentQuantity_total / m_TargetUnits;
	}
	
	//---------------------------------------------------------------------------
	
	void CalcAndSetQuantity( ActionData action_data )
	{
		m_SpentQuantity_total += m_SpentQuantity;
		if (GetGame().IsServer())
		{
			if ( m_SpentUnits )
			{
				m_SpentUnits.param1 = m_SpentQuantity;
				SetACData(m_SpentUnits);
			}
			
			if (action_data.m_MainItem.GetQuantity() > 0.0)
				action_data.m_MainItem.AddQuantity(-m_SpentQuantity);
		}	
		m_SpentQuantity = 0;
	}
};