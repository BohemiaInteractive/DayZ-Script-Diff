class CAContinuousQuantityLiquidTransfer : CAContinuousBase
{
	protected int 					m_TendencyDrain; //1 == drain, 0 == pour, -1 == cancel!
	protected float 				m_ItemQuantity;
	protected float 				m_SpentQuantity;
	protected float 				m_SpentQuantity_total;
	protected float 				m_ItemMaxQuantity; //basically free capacity
	protected float					m_TimeElpased;
	protected float 				m_QuantityUsedPerSecond;
	protected float 				m_AdjustedQuantityUsedPerSecond;
	protected float 				m_DefaultTimeStep;
	protected ref Param1<float>		m_SpentUnits;
	
	void CAContinuousQuantityLiquidTransfer( float quantity_used_per_second, float time_to_progress, int drain = -1) //-1 used as a safeguard; if no valid value arrives, callback is canceled
	{
		m_QuantityUsedPerSecond = quantity_used_per_second;
		m_DefaultTimeStep = time_to_progress;
		m_TendencyDrain = drain;
	}
	
	override void Setup( ActionData action_data )
	{
		ItemBase target_item = ItemBase.Cast(action_data.m_Target.GetObject());
		
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
		
		m_QuantityUsedPerSecond *= Math.Min(action_data.m_MainItem.GetLiquidThroughputCoef(),target_item.GetLiquidThroughputCoef());
		
		if ( m_TendencyDrain == 1 )
		{
			if ( target_item.GetQuantity() > (action_data.m_MainItem.GetQuantityMax() - action_data.m_MainItem.GetQuantity()) )
			{
				m_ItemMaxQuantity = action_data.m_MainItem.GetQuantityMax() - action_data.m_MainItem.GetQuantity();
				m_ItemQuantity = m_ItemMaxQuantity; //target_item.GetQuantity();
			}
			else //
			{
				m_ItemMaxQuantity = target_item.GetQuantity();
				m_ItemQuantity = m_ItemMaxQuantity; //target_item.GetQuantity();
			}
		}
		else if ( m_TendencyDrain == 0)
		{
			if ( action_data.m_MainItem.GetQuantity() > (target_item.GetQuantityMax() - target_item.GetQuantity()) )
			{
				m_ItemMaxQuantity = target_item.GetQuantityMax() - target_item.GetQuantity();
				m_ItemQuantity = m_ItemMaxQuantity; //action_data.m_MainItem.GetQuantity();
			}
			else //
			{
				m_ItemMaxQuantity = action_data.m_MainItem.GetQuantity();
				m_ItemQuantity = m_ItemMaxQuantity; //target_item.GetQuantity();
			}
		}
	}
	
	
	override int Execute( ActionData action_data  )
	{
		ItemBase target_item = ItemBase.Cast(action_data.m_Target.GetObject());
		
		if ( !action_data.m_Player || m_TendencyDrain == -1 || !action_data.m_Target )
		{
			return UA_ERROR;
		}
		
		if ( m_ItemQuantity <= 0 )
		{
			return UA_FINISHED;
		}
		else
		{
			if ( m_SpentQuantity_total < m_ItemQuantity )
			{
				m_AdjustedQuantityUsedPerSecond = action_data.m_Player.GetSoftSkillsManager().AddSpecialtyBonus( m_QuantityUsedPerSecond, m_Action.GetSpecialtyWeight(), true);
				m_SpentQuantity += m_AdjustedQuantityUsedPerSecond * action_data.m_Player.GetDeltaT();
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
		if ( !action_data.m_Player || m_TendencyDrain == -1 || !action_data.m_Target || !action_data.m_Target.GetObject() )
		{
			return UA_ERROR;
		}
		
		CalcAndSetQuantity( action_data );
		return UA_INTERRUPT;
	}	
	
	override float GetProgress()
	{
		if ( m_TendencyDrain )
		{
			return m_SpentQuantity_total / m_ItemMaxQuantity;
		}
		else
		{
			return -(m_SpentQuantity_total / m_ItemMaxQuantity);
		}
	}
	
	//---------------------------------------------------------------------------
	
	void CalcAndSetQuantity( ActionData action_data )
	{
		ItemBase target_item = ItemBase.Cast(action_data.m_Target.GetObject());
		m_SpentQuantity_total += m_SpentQuantity;
		
		if ( GetGame().IsServer() )
		{
			if ( m_SpentUnits )
			{
				m_SpentUnits.param1 = m_SpentQuantity;
				SetACData(m_SpentUnits);
			}
			
			//could move following stuff to action itself, if needed
			if ( m_TendencyDrain )
			{
				Liquid.Transfer(target_item, action_data.m_MainItem, m_SpentQuantity);
			}
			else
			{
				Liquid.Transfer(action_data.m_MainItem, target_item, m_SpentQuantity);
			}
		}
		m_SpentQuantity = 0;
	}
}