class CAContinuousFillPowerGenerator : CAContinuousFill
{
	void CAContinuousFillPowerGenerator( float quantity_filled_per_second , int liquid_type )
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
		
		EntityAI pg = EntityAI.Cast(action_data.m_Target.GetObject()); // get power generator
		m_QuantityFilledPerSecond *= Math.Min(action_data.m_MainItem.GetLiquidThroughputCoef(),pg.GetLiquidThroughputCoef());
		
		m_ItemQuantity = action_data.m_MainItem.GetQuantity();
		m_TargetUnits = pg.GetCompEM().GetEnergyMax() - pg.GetCompEM().GetEnergy();
		m_AdjustedQuantityFilledPerSecond = m_QuantityFilledPerSecond;//removed softskills 
	}
	
	override int Execute( ActionData action_data )
	{
		if ( !action_data.m_Player )
		{
			return UA_ERROR;
		}
		
		PowerGeneratorBase pg = PowerGeneratorBase.Cast(action_data.m_Target.GetObject()); // get power generator
		
		if ( pg.GetFuel() >= pg.GetMaxFuel() )
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

	override void CalcAndSetQuantity( ActionData action_data )
	{
		m_SpentQuantity_total += m_SpentQuantity;
	
		if ( m_SpentUnits )
		{
			m_SpentUnits.param1 = m_SpentQuantity;
			SetACData(m_SpentUnits);
		}
		
		
		if ( GetGame().IsServer() )
		{
			PowerGeneratorBase pg = PowerGeneratorBase.Cast(action_data.m_Target.GetObject()); // get power generator
			int consumed_fuel = pg.AddFuel( m_SpentQuantity );
			action_data.m_MainItem.AddQuantity( -consumed_fuel );
		}
		
		m_SpentQuantity = 0;
	}
};