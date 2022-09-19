class CASingleUseQuantityEdible : CASingleUseQuantity
{
	void CASingleUseQuantityEdible( float quantity_used_per_action )
	{
		m_QuantityUsedPerAction = quantity_used_per_action;
	}
	
	override void CalcAndSetQuantity(ActionData action_data )
	{	
		if ( m_SpentUnits )
		{
			m_SpentUnits.param1 = m_QuantityUsedPerAction;
			SetACData(m_SpentUnits);	
		}
		
		PlayerBase ntarget = PlayerBase.Cast( action_data.m_Target.GetObject() );
		if ( ntarget )
		{
			if ( GetGame().IsServer() ) 
			{
				//action_data.m_MainItem.Consume(ntarget, m_QuantityUsedPerAction);
				ntarget.Consume(action_data.m_MainItem, m_QuantityUsedPerAction, EConsumeType.ITEM_SINGLE_TIME);
			}
		}
		else
		{
			if ( GetGame().IsServer() ) 
			{
				//action_data.m_MainItem.Consume(action_data.m_Player, m_QuantityUsedPerAction);
				action_data.m_Player.Consume(action_data.m_MainItem, m_QuantityUsedPerAction, EConsumeType.ITEM_SINGLE_TIME);
			}
		}
	}
};