class CAContinuousQuantityBloodTransfer : CAContinuousQuantityRepeat
{
	override void CalcAndSetQuantity( ActionData action_data )
	{
		if ( GetGame().IsServer() )
		{
			if ( m_SpentUnits )
			{
				m_SpentUnits.param1 = m_SpentQuantity;
				SetACData(m_SpentUnits);
			}

			action_data.m_MainItem.AddQuantity( -m_SpentQuantity, false, false );

			ActionGiveBloodData action_data_b = ActionGiveBloodData.Cast( action_data );

			PlayerBase bloodtarget = PlayerBase.Cast( action_data.m_Target.GetObject() );
			if ( !bloodtarget )
				bloodtarget = action_data.m_Player;

			
			if( BloodTypes.MatchBloodCompatibility(action_data_b.m_ItemBloodType, bloodtarget.GetStatBloodType().Get()) )
			{
				bloodtarget.AddHealth("", "Blood", m_SpentQuantity);
			}

		}
	}
}