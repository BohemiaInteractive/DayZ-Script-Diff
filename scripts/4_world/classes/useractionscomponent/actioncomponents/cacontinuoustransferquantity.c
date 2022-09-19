class CAContinuousTransferQuantity : CAContinuousBase
{
	protected float 				m_TargetUnits;
	protected float 				m_SpentQuantity;
	protected float 				m_ItemQuantity;
	protected float 				m_SourceQuantity;
	protected float 				m_QuantityFilledPerSecond;
	protected float					m_TimeToComplete;
	protected ref Param1<float>		m_SpentUnits;
	
	void CAContinuousTransferQuantity( float quantity_filled_per_second )
	{
		m_QuantityFilledPerSecond = quantity_filled_per_second;
	}
	
	override void Setup( ActionData action_data )
	{		
		m_SpentQuantity = 0;
		if ( !m_SpentUnits )
		{ 
			m_SpentUnits = new Param1<float>(0);
		}
		else
		{	
			m_SpentUnits.param1 = 0;
		}
		m_ItemQuantity = action_data.m_MainItem.GetQuantity();
		ItemBase trg;
		if ( Class.CastTo(trg, action_data.m_Target.GetObject()) )
		{
			m_SourceQuantity = trg.GetQuantity();
		}
		m_TargetUnits = action_data.m_MainItem.GetQuantityMax();	
		m_TimeToComplete= (Math.Min((m_TargetUnits-m_ItemQuantity),m_SourceQuantity))/m_QuantityFilledPerSecond;
	}
	
	override int Execute( ActionData action_data )
	{		
		if ( !action_data.m_Player )
		{
			return UA_ERROR;
		}
		ItemBase trg;
		Class.CastTo(trg, action_data.m_Target.GetObject());
		if ( action_data.m_MainItem.GetQuantity() >= action_data.m_MainItem.GetQuantityMax() || trg.GetQuantity() == 0 )
		{
			return UA_FINISHED;
		}
		else
		{
		if ( m_SpentQuantity < m_TargetUnits && m_SpentQuantity < m_SourceQuantity )
		{
			m_SpentQuantity += m_QuantityFilledPerSecond * action_data.m_Player.GetDeltaT();
			float val = m_ItemQuantity-m_SpentQuantity;
			if ( m_Action ) m_Action.SendMessageToClient(action_data.m_Player, val.ToString());
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
	
	override override int Cancel( ActionData action_data )
	{		
		if ( !action_data.m_Player )
		{
			return UA_ERROR;
		}
		
		CalcAndSetQuantity( action_data );
		return UA_CANCEL;
	}
		
	override float GetProgress()
	{	
		float progress = (m_SpentQuantity*m_QuantityFilledPerSecond)/m_TimeToComplete;
		return (m_SpentQuantity*m_QuantityFilledPerSecond)/m_TimeToComplete;
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
			action_data.m_MainItem.AddQuantity(m_SpentQuantity);
			ItemBase trg;
			if ( Class.CastTo(trg, action_data.m_Target.GetObject()) )
			{
				trg.AddQuantity(-m_SpentQuantity,false,false);
			}
		}
	}
};