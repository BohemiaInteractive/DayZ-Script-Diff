class CAContinuousFish : CAContinuousBase
{
	protected FishingRod_Base m_Rod;
	
	override void Setup( ActionData action_data )
	{
		Class.CastTo(m_Rod, action_data.m_MainItem);
	}
	
	override int Execute( ActionData action_data )
	{
		if ( m_Rod != action_data.m_MainItem )
		{
			return UA_ERROR;
		}
		
		if ( GetGame().IsServer() ) 
		{
			m_Rod.AddPull(action_data.m_Player.GetDeltaT());
		}
		return UA_PROCESSING;	
	}
};
