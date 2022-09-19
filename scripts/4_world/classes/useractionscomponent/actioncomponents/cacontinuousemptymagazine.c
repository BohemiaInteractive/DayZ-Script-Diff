class CAContinuousEmptyMagazine : CAContinuousBase
{
	protected float 				m_TargetUnits;
	protected float 				m_SpentQuantity;
	protected float 				m_Time;
	protected float 				m_TimeToEjectOneBullet;
	protected ref Param1<float>	m_SpentUnits;
	
	void CAContinuousEmptyMagazine( float eject_time )
	{
		m_TimeToEjectOneBullet = eject_time;
		m_Time = 0;
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
		m_SpentQuantity = 0;
		Magazine itm;
		Class.CastTo(itm, action_data.m_MainItem);
		m_TargetUnits = itm.GetAmmoCount();			
		
	}
	
	override int Execute( ActionData action_data )
	{
		if ( !action_data.m_Player )
		{
			return UA_ERROR;
		}
		
		if ( m_TargetUnits == 0 )
		{
			return UA_FINISHED;
		}
		else
		{
			if ( m_SpentQuantity < m_TargetUnits )
			{
				m_Time += action_data.m_Player.GetDeltaT();
				if ( m_Time > m_TimeToEjectOneBullet )
				{
					m_Time = 0;
					m_SpentQuantity += 1;
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
		//float progress = m_SpentQuantity/m_TargetUnits;
		return m_SpentQuantity/m_TargetUnits;
	}
	
	//---------------------------------------------------------------------------
	
	void CalcAndSetQuantity( ActionData action_data )
	{
		if ( GetGame().IsServer() )
		{
			if ( action_data.m_MainItem )
			{
				Magazine itm;
				Class.CastTo(itm, action_data.m_MainItem);
				if ( m_SpentUnits )
				{
					m_SpentUnits.param1 = m_SpentQuantity;
					SetACData(m_SpentUnits);
				}		
				itm.ServerAddAmmoCount(-m_SpentQuantity);
			}
		}
	}
};