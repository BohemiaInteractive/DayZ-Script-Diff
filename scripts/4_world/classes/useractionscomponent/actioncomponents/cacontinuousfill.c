class CAContinuousFill : CAContinuousBase
{
	protected float 				m_TargetUnits;
	protected float 				m_SpentQuantity;
	protected float 				m_SpentQuantity_total;
	protected float 				m_ItemQuantity;
	protected float 				m_AdjustedQuantityFilledPerSecond;
	protected float 				m_QuantityFilledPerSecond;
	protected ref Param1<float>		m_SpentUnits;
	protected float 				m_TimeElpased;
	protected float 				m_DefaultTimeStep = 0.25;
	protected int 					m_liquid_type;
	
	void CAContinuousFill( float quantity_filled_per_second , int liquid_type )
	{
		m_QuantityFilledPerSecond = quantity_filled_per_second;
		m_liquid_type = liquid_type;
	}
	
	override void Setup( ActionData action_data )
	{
		m_TimeElpased = 0;
		
		if ( !m_SpentUnits )
		{ 
			m_SpentUnits = new Param1<float>(0);
		}
		else
		{	
			m_SpentUnits.param1 = 0;
		}
		
		float throughputCoef = action_data.m_MainItem.GetLiquidThroughputCoef();
		EntityAI entity;
		if (action_data.m_Target && action_data.m_Target.GetObject() && Class.CastTo(entity,action_data.m_Target.GetObject()))
		{
			throughputCoef = Math.Min(throughputCoef,entity.GetLiquidThroughputCoef());
		}
		m_QuantityFilledPerSecond *= throughputCoef;
		
		m_ItemQuantity = action_data.m_MainItem.GetQuantity();
		m_TargetUnits = action_data.m_MainItem.GetQuantityMax() - action_data.m_MainItem.GetQuantity();	
		m_AdjustedQuantityFilledPerSecond = m_QuantityFilledPerSecond;//removed softskills 
	}
	
	override int Execute( ActionData action_data )
	{
		if ( !action_data.m_Player )
		{
			return UA_ERROR;
		}
		
		if ( action_data.m_MainItem.GetQuantity() >= action_data.m_MainItem.GetQuantityMax() )
		{
			return UA_FINISHED;
		}
		else
		{
			if ( m_SpentQuantity_total < m_TargetUnits )
			{
				m_SpentQuantity += m_AdjustedQuantityFilledPerSecond * action_data.m_Player.GetDeltaT();
				m_TimeElpased += action_data.m_Player.GetDeltaT();
				
				if ( m_TimeElpased >= m_DefaultTimeStep )
				{
					CalcAndSetQuantity( action_data );
					m_TimeElpased = 0;
					//Setup(action_data);	//reset data after repeat
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
		return m_SpentQuantity_total/m_TargetUnits;
	}
	//---------------------------------------------------------------------------
	
	void CalcAndSetQuantity(ActionData action_data)
	{
		m_SpentQuantity_total += m_SpentQuantity;
		if (GetGame().IsServer())
		{
			if (m_SpentUnits)
			{
				m_SpentUnits.param1 = m_SpentQuantity;
				SetACData(m_SpentUnits);
			}

			Liquid.FillContainerEnviro(action_data.m_MainItem, m_liquid_type, m_SpentQuantity);
		}

		m_SpentQuantity = 0;
	}
}
