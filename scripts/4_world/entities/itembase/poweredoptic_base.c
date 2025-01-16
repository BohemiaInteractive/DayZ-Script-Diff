class PoweredOptic_Base extends ItemOptics
{
	protected PlayerBase 				m_Player;
	protected bool 						m_IsActionActive; // sanity check for comparing to EM synched value
	
	void SetPlayer( PlayerBase player )
	{
		m_Player = player;
	}
	
	PlayerBase GetPlayer()
	{
		return m_Player;
	}
	
	void StartWorkServer()
	{
		m_IsActionActive = true;
		GetCompEM().SwitchOn();
	}
	
	void StopWorkServer()
	{
		GetCompEM().SwitchOff();
		m_IsActionActive = false;
	}
}
