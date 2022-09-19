class CASingleUseQuantity : CASingleUseBase
{	
	protected float 				m_QuantityUsedPerAction;
	protected float 				m_ItemQuantity;
	protected ref Param1<float>	m_SpentUnits;

	void CASingleUseQuantity( float quantity_used_per_action )
	{
		m_QuantityUsedPerAction = quantity_used_per_action;	
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
		m_ItemQuantity = action_data.m_MainItem.GetQuantity();
	}
	
	override int Execute( ActionData action_data )
	{
		if ( !action_data.m_Player )
		{
			return UA_ERROR;
		}
		

		CalcAndSetQuantity( action_data );
		return UA_FINISHED;	
	}
	
	//---------------------------------------------------------------------------
	
	void CalcAndSetQuantity( ActionData action_data )
	{
		if ( GetGame().IsServer() )
		{
			if ( m_SpentUnits )
			{
				m_SpentUnits.param1 = m_QuantityUsedPerAction;
				SetACData(m_SpentUnits);	
			}

			action_data.m_MainItem.AddQuantity(- m_QuantityUsedPerAction,false,false);
		}
	}
};