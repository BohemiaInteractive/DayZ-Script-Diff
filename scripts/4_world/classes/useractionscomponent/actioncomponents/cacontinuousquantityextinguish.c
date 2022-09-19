class CAContinuousQuantityExtinguish : CAContinuousQuantityRepeat
{
	private float 		m_WetnessGainMultiplier 	= 1.0;
	
	void CAContinuousQuantityExtinguish( float quantity_used_per_second, float time_to_repeat, float wetness_gain_multiplier )
	{
		m_QuantityUsedPerSecond = quantity_used_per_second;
		m_DefaultTimeToRepeat = time_to_repeat;
		m_WetnessGainMultiplier = wetness_gain_multiplier;
	}
		
	override void CalcAndSetQuantity( ActionData action_data )
	{	
		if ( GetGame().IsServer() ) 
		{
			if ( m_SpentUnits )
			{
				m_SpentUnits.param1 = m_SpentQuantity;
				SetACData( m_SpentUnits );
			}
			
			Object targetObject = action_data.m_Target.GetObject();
			if ( targetObject )
			{
				FireplaceBase fireplace_target = FireplaceBase.Cast( targetObject );
				
				//add wetness to fireplace targets, set fire state to extinguishing
				float wetness_inc = ( m_SpentQuantity / 1000 ) * m_WetnessGainMultiplier;
				fireplace_target.AddWetnessToFireplace( wetness_inc );
				fireplace_target.SetExtinguishingState();
				
				//subtract quantity from water source
				action_data.m_MainItem.AddQuantity( -m_SpentQuantity );
			}
		}
	}
	
	override int Interrupt( ActionData action_data )
	{				
		if ( GetGame().IsServer() )
		{
			action_data.m_Player.GetSoftSkillsManager().AddSpecialty( UASoftSkillsWeight.ROUGH_MEDIUM );
		}
		
		return super.Interrupt( action_data );
	}
}